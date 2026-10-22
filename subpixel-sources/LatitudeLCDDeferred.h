// Included by the process-local interposer. Keep native grayscale pixels for
// unknown consumers, while retaining immutable component coverage for a known
// opaque CGContextDrawImage destination. No cross-process or CA claim.
static std::atomic<size_t> deferredBytes{0};
static std::atomic<uint64_t> deferredLines{0},deferredImages{0};
struct DeferredText {
 unsigned width=0,height=0;size_t charge=0,glyphBytes=0;CGBitmapInfo format=kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big;
 std::vector<uint8_t>expected;
 std::vector<std::shared_ptr<LatitudeLCDTextLayer>>lines;
 ~DeferredText(){if(charge)deferredBytes.fetch_sub(charge);}
 bool reserve(size_t n){if(n>8*1024*1024)return false;size_t used=deferredBytes.load();do{if(used>64*1024*1024-n)return false;}while(!deferredBytes.compare_exchange_weak(used,used+n));charge=n;return true;}
};
struct BitmapView {uint8_t*data=nullptr;unsigned w=0,h=0;size_t pitch=0;CGColorSpaceRef space=nullptr;bool skip=false,bgra=false;CGBitmapInfo format=0;};
static bool bitmapView(CGContextRef c,BitmapView&v){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i==contexts.end()||!i->second.known||i->second.layers||i->second.current.shadow||i->second.current.text!=kCGTextFill)return false;}
 using Alpha=CGFloat(*)(CGContextRef);using Blend=CGBlendMode(*)(CGContextRef);
 static auto alpha=(Alpha)dlsym(RTLD_DEFAULT,"CGContextGetAlpha");static auto blend=(Blend)dlsym(RTLD_DEFAULT,"CGContextGetBlendMode");
 if(!alpha||!blend||alpha(c)!=1||blend(c)!=kCGBlendModeNormal||!CGAffineTransformIsIdentity(CGContextGetCTM(c)))return false;
 size_t w=CGBitmapContextGetWidth(c),h=CGBitmapContextGetHeight(c);v.data=(uint8_t*)CGBitmapContextGetData(c);v.pitch=CGBitmapContextGetBytesPerRow(c);v.space=CGBitmapContextGetColorSpace(c);
 if(!v.data||!w||!h||w>4096||h>4096||v.pitch<w*4||v.pitch>65536||CGBitmapContextGetBitsPerComponent(c)!=8||CGBitmapContextGetBitsPerPixel(c)!=32)return false;
 auto a=CGBitmapContextGetAlphaInfo(c);v.format=CGBitmapContextGetBitmapInfo(c);
 auto order=v.format&kCGBitmapByteOrderMask;
 bool rgba=order==kCGBitmapByteOrder32Big&&(a==kCGImageAlphaPremultipliedLast||a==kCGImageAlphaNoneSkipLast);
 v.bgra=order==kCGBitmapByteOrder32Little&&(a==kCGImageAlphaPremultipliedFirst||a==kCGImageAlphaNoneSkipFirst);
 if(!rgba&&!v.bgra)return false;
 v.skip=a==kCGImageAlphaNoneSkipLast||a==kCGImageAlphaNoneSkipFirst;
 CFStringRef name=v.space?CGColorSpaceCopyName(v.space):nullptr;bool srgb=name&&CFEqual(name,kCGColorSpaceSRGB);if(name)CFRelease(name);if(!srgb)return false;
 v.w=(unsigned)w;v.h=(unsigned)h;return true;
}
static bool samePixels(const DeferredText&d,const BitmapView&v){if(d.width!=v.w||d.height!=v.h||d.format!=v.format)return false;for(unsigned y=0;y<v.h;y++)if(memcmp(d.expected.data()+(size_t)y*v.w*4,v.data+y*v.pitch,(size_t)v.w*4))return false;return true;}
static bool clearPixels(const BitmapView&v){if(v.skip)return false;for(unsigned y=0;y<v.h;y++)for(unsigned x=0;x<v.w*4;x++)if(v.data[y*v.pitch+x])return false;return true;}
static void invalidateDeferred(CGContextRef c){std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.deferred.reset();}
template<class Native>static bool deferLayerNative(CGContextRef c,const std::shared_ptr<LatitudeLCDTextLayer>&layer,Native native){
 BitmapView v;if(!layer||!bitmapView(c,v)||v.skip)return false;
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i==contexts.end()||i->second.current.clipModified)return false;}
 std::shared_ptr<DeferredText>prior;
 {std::lock_guard<std::mutex>g(stateLock);prior=contexts.at(c).deferred;}
 if(prior&&!samePixels(*prior,v)){invalidateDeferred(c);prior.reset();}
 if(!prior&&!clearPixels(v))return false;
 size_t glyphBytes=(prior?prior->glyphBytes:0)+LatitudeLCDTextLayerByteSize(layer.get());
 if(glyphBytes>4*1024*1024||(prior&&prior->lines.size()>=128)){invalidateDeferred(c);return false;}
 auto next=std::make_shared<DeferredText>();if(!next->reserve((size_t)v.w*v.h*4+glyphBytes)){invalidateDeferred(c);return false;}
 next->width=v.w;next->height=v.h;next->format=v.format;next->glyphBytes=glyphBytes;if(prior)next->lines=prior->lines;next->lines.push_back(layer);next->expected.resize((size_t)v.w*v.h*4);
 // Exactly one original draw, retaining a valid grayscale image for consumers
 // that cannot carry the RGB sidecar. Allocation is complete before this call.
 native();
 for(unsigned y=0;y<v.h;y++)memcpy(next->expected.data()+(size_t)y*v.w*4,v.data+y*v.pitch,(size_t)v.w*4);
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.deferred=next;}
 deferredLines++;return true;
}
static bool deferNativeLine(CTLineRef line,CGContextRef c){
 BitmapView v;if(!bitmapView(c,v))return false;
 auto matrix=CGContextGetTextMatrix(c);auto p=CGContextGetTextPosition(c);
 if(matrix.a!=1||matrix.b||matrix.c||matrix.d!=1||!std::isfinite(p.x)||!std::isfinite(p.y)||p.y!=floor(p.y))return false;
 auto layer=std::shared_ptr<LatitudeLCDTextLayer>(LatitudeLCDCreateTextLayer(line,p.x,v.h-p.y,0),LatitudeLCDDestroyTextLayer);
 return deferLayerNative(c,layer,[&](){CTLineDraw(line,c);});
}
static std::unordered_map<CGImageRef,std::weak_ptr<DeferredText>>imageLayers;
struct ImageRelease {CGImageRef image=nullptr;std::shared_ptr<DeferredText>layer;std::atomic<unsigned>references{2};std::atomic<bool>fired{false};};
static void dropImage(ImageRelease*b){if(b->references.fetch_sub(1)==1)delete b;}
static void imageReleased(void*info,const void*,size_t){auto b=(ImageRelease*)info;b->fired=true;{std::lock_guard<std::mutex>g(stateLock);if(b->image)imageLayers.erase(b->image);}dropImage(b);}
static CGImageRef latitudeBitmapImage(CGContextRef c){
 CGImageRef originalImage=CGBitmapContextCreateImage(c);if(!originalImage)return nullptr;
 try{
  std::shared_ptr<DeferredText>d;{std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())d=i->second.deferred;if(imageLayers.size()>=2048)return originalImage;}
  BitmapView v;if(!d||!bitmapView(c,v)||!samePixels(*d,v)){invalidateDeferred(c);return originalImage;}
  auto b=new(std::nothrow)ImageRelease;if(!b)return originalImage;b->layer=d;
  auto provider=CGDataProviderCreateWithData(b,d->expected.data(),d->expected.size(),imageReleased);
  if(!provider){if(!b->fired.exchange(true))dropImage(b);dropImage(b);return originalImage;}
  auto image=CGImageCreate(v.w,v.h,8,32,v.w*4,v.space,d->format,provider,nullptr,CGImageGetShouldInterpolate(originalImage),CGImageGetRenderingIntent(originalImage));b->image=image;
  if(image){try{std::lock_guard<std::mutex>g(stateLock);imageLayers.emplace(image,d);}catch(...){}}
  CGDataProviderRelease(provider);dropImage(b);
  if(!image)return originalImage;CGImageRelease(originalImage);return image;
 }catch(...){return originalImage;}
}
template<class Native>static bool drawDeferredImage(CGContextRef c,CGRect r,const std::shared_ptr<DeferredText>&d,Native beforePaint){
 BitmapView v;if(!bitmapView(c,v)||!std::isfinite(r.origin.x)||!std::isfinite(r.origin.y)||fabs(r.origin.x)>32768||fabs(r.origin.y)>32768||r.origin.x!=floor(r.origin.x)||r.origin.y!=floor(r.origin.y)||r.size.width!=d->width||r.size.height!=d->height)return false;
 int sourceX=(int)r.origin.x,sourceY=(int)v.h-(int)r.origin.y-(int)d->height;
 int left=(int)d->width,top=(int)d->height,inkRight=0,inkBottom=0;
 for(const auto&line:d->lines){int lx,ly;unsigned lw,lh;if(LatitudeLCDTextLayerBounds(line.get(),&lx,&ly,&lw,&lh)){left=std::min(left,lx);top=std::min(top,ly);inkRight=std::max(inkRight,lx+(int)lw);inkBottom=std::max(inkBottom,ly+(int)lh);}}
 left=std::max(0,left);top=std::max(0,top);inkRight=std::min((int)d->width,inkRight);inkBottom=std::min((int)d->height,inkBottom);
 int x=std::max(0,sourceX+left),y=std::max(0,sourceY+top),right=std::min((int)v.w,sourceX+inkRight),bottom=std::min((int)v.h,sourceY+inkBottom);if(right<=x||bottom<=y)return false;
 unsigned w=(unsigned)(right-x),h=(unsigned)(bottom-y);if((size_t)w*h>1024*1024)return false;std::vector<uint8_t>pixels((size_t)w*h*4);
 for(unsigned row=0;row<h;row++){memcpy(pixels.data()+(size_t)row*w*4,v.data+(size_t)(row+y)*v.pitch+x*4,w*4);for(unsigned col=0;col<w;col++){auto pixel=pixels.data()+((size_t)row*w+col)*4;if(v.bgra)std::swap(pixel[0],pixel[2]);auto&a=pixel[3];if(v.skip)a=255;else if(a!=255)return false;}}
 for(const auto&line:d->lines)if(!LatitudeLCDCompositeTextLayerAt(line.get(),pixels.data(),pixels.size(),w,h,w*4,0,x-sourceX,y-sourceY))return false;
 CFDataRef copy=CFDataCreate(kCFAllocatorDefault,pixels.data(),(CFIndex)pixels.size());auto provider=copy?CGDataProviderCreateWithCFData(copy):nullptr;if(copy)CFRelease(copy);if(!provider)return false;
 auto image=CGImageCreate(w,h,8,32,w*4,v.space,kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big,provider,nullptr,false,kCGRenderingIntentDefault);CGDataProviderRelease(provider);if(!image)return false;
 beforePaint();CGContextSaveGState(c);CGContextSetInterpolationQuality(c,kCGInterpolationNone);CGContextDrawImage(c,CGRectMake(x,v.h-y-h,w,h),image);CGContextRestoreGState(c);CGImageRelease(image);deferredImages++;return true;
}
static void latitudeDrawImage(CGContextRef c,CGRect r,CGImageRef image){
 bool done=false;
 if(!inside&&c&&image){inside=true;try{std::shared_ptr<DeferredText>d;{std::lock_guard<std::mutex>g(stateLock);auto i=imageLayers.find(image);if(i!=imageLayers.end())d=i->second.lock();}if(d)done=drawDeferredImage(c,r,d,[](){});}catch(...){done=false;}inside=false;}
 if(!done)CGContextDrawImage(c,r,image);
}
static void changedClip(CGContextRef c){std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.clipModified=true;}
static void latitudeClip(CGContextRef c){changedClip(c);CGContextClip(c);}
static void latitudeEOClip(CGContextRef c){changedClip(c);CGContextEOClip(c);}
static void latitudeClipRect(CGContextRef c,CGRect r){changedClip(c);CGContextClipToRect(c,r);}
static void latitudeClipRects(CGContextRef c,const CGRect*r,size_t n){changedClip(c);CGContextClipToRects(c,r,n);}
static void latitudeClipMask(CGContextRef c,CGRect r,CGImageRef mask){changedClip(c);CGContextClipToMask(c,r,mask);}
static void latitudeResetClip(CGContextRef c){changedClip(c);CGContextResetClip(c);}
extern "C" void LatitudeLCDDeferredStats(uint64_t*lines,uint64_t*images,uint64_t*liveImages,uint64_t*bytes){if(lines)*lines=deferredLines.load();if(images)*images=deferredImages.load();if(bytes)*bytes=deferredBytes.load();if(liveImages){std::lock_guard<std::mutex>g(stateLock);*liveImages=imageLayers.size();}}
