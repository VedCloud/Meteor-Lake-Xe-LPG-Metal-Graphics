// Process-local native CTLineDraw integration for verified opaque sRGB bitmap
// contexts created through the public CGBitmapContext APIs. No system install.
#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#include "LatitudeLCDCoreText.h"
#include <dlfcn.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <new>
#include <cmath>
struct Flags {bool shadow=false;CGTextDrawingMode text=kCGTextFill;};
struct Context {bool known=true;unsigned layers=0;Flags current;std::vector<Flags>saved;};
static std::mutex stateLock;static std::unordered_map<CGContextRef,Context>contexts;
static std::atomic<uint64_t>nativeCalls{0},converted{0},fallbacks{0};
static thread_local bool inside=false;
struct ReleaseBox {CGContextRef context=nullptr;CGBitmapContextReleaseDataCallback callback=nullptr;void*info=nullptr;std::atomic<unsigned>references{2};std::atomic<bool>fired{false};};
static void drop(ReleaseBox*b){if(b->references.fetch_sub(1)==1)delete b;}
static void released(void*info,void*data){auto b=(ReleaseBox*)info;b->fired=true;{std::lock_guard<std::mutex>g(stateLock);if(b->context)contexts.erase(b->context);}if(b->callback)b->callback(b->info,data);drop(b);}
// References from the interposing image bind to the original implementation.
// dlsym(RTLD_NEXT) is itself subject to dyld interposition and recurses here.
static CGContextRef nativeCreate(void*data,size_t w,size_t h,size_t bits,size_t pitch,CGColorSpaceRef space,CGBitmapInfo info,CGBitmapContextReleaseDataCallback callback,void*releaseInfo){
 using F=decltype(&CGBitmapContextCreateWithData);static F real=&CGBitmapContextCreateWithData;if(!real)return nullptr;
 auto box=new(std::nothrow)ReleaseBox;if(!box)return real(data,w,h,bits,pitch,space,info,callback,releaseInfo);box->callback=callback;box->info=releaseInfo;
 auto c=real(data,w,h,bits,pitch,space,info,released,box);box->context=c;
 if(c){try{std::lock_guard<std::mutex>g(stateLock);if(contexts.size()<2048)contexts.emplace(c,Context{});}catch(...){}}
 else if(!box->fired.exchange(true))drop(box);
 drop(box);return c;
}
static CGContextRef latitudeBitmapCreate(void*d,size_t w,size_t h,size_t b,size_t p,CGColorSpaceRef s,CGBitmapInfo i){return nativeCreate(d,w,h,b,p,s,i,nullptr,nullptr);}
static CGContextRef latitudeBitmapCreateWithData(void*d,size_t w,size_t h,size_t b,size_t p,CGColorSpaceRef s,CGBitmapInfo i,CGBitmapContextReleaseDataCallback cb,void*ci){return nativeCreate(d,w,h,b,p,s,i,cb,ci);}
static void latitudeSave(CGContextRef c){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end()){try{if(i->second.saved.size()<64)i->second.saved.push_back(i->second.current);else i->second.known=false;}catch(...){i->second.known=false;}}}
 static auto real=&CGContextSaveGState;real(c);
}
static void latitudeRestore(CGContextRef c){
 static auto real=&CGContextRestoreGState;real(c);
 std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end()){if(i->second.saved.empty())i->second.known=false;else{i->second.current=i->second.saved.back();i->second.saved.pop_back();}}
}
static void latitudeShadow(CGContextRef c,CGSize size,CGFloat blur){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.shadow=true;}
 static auto real=&CGContextSetShadow;real(c,size,blur);
}
static void latitudeShadowColor(CGContextRef c,CGSize size,CGFloat blur,CGColorRef color){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.shadow=color&&CGColorGetAlpha(color)>0;}
 static auto real=&CGContextSetShadowWithColor;real(c,size,blur,color);
}
static void latitudeTextMode(CGContextRef c,CGTextDrawingMode mode){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.text=mode;}
 static auto real=&CGContextSetTextDrawingMode;real(c,mode);
}
static void latitudeBegin(CGContextRef c,CFDictionaryRef aux){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end()){if(i->second.layers<64)i->second.layers++;else i->second.known=false;}}
 static auto real=&CGContextBeginTransparencyLayer;real(c,aux);
}
static void latitudeBeginRect(CGContextRef c,CGRect r,CFDictionaryRef aux){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end()){if(i->second.layers<64)i->second.layers++;else i->second.known=false;}}
 static auto real=&CGContextBeginTransparencyLayerWithRect;real(c,r,aux);
}
static void latitudeEnd(CGContextRef c){
 static auto real=&CGContextEndTransparencyLayer;real(c);
 std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end()){if(i->second.layers)i->second.layers--;else i->second.known=false;}
}
static bool draw(CTLineRef line,CGContextRef c){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i==contexts.end()||!i->second.known||i->second.layers||i->second.current.shadow||i->second.current.text!=kCGTextFill)return false;}
 // Private query signatures are documented by the archived primary reference.
 // Absence falls back; the renderer never guesses opaque graphics-state fields.
 using Alpha=CGFloat(*)(CGContextRef);using Blend=CGBlendMode(*)(CGContextRef);
 static Alpha alpha=reinterpret_cast<Alpha>(dlsym(RTLD_DEFAULT,"CGContextGetAlpha"));static Blend blend=reinterpret_cast<Blend>(dlsym(RTLD_DEFAULT,"CGContextGetBlendMode"));
 if(!alpha||!blend||alpha(c)!=1||blend(c)!=kCGBlendModeNormal)return false;
 auto textMatrix=CGContextGetTextMatrix(c);
 if(!CGAffineTransformIsIdentity(CGContextGetCTM(c))||textMatrix.a!=1||textMatrix.b||textMatrix.c||textMatrix.d!=1)return false;
 size_t width=CGBitmapContextGetWidth(c),height=CGBitmapContextGetHeight(c),pitch=CGBitmapContextGetBytesPerRow(c);auto data=(uint8_t*)CGBitmapContextGetData(c);
 if(!data||!width||!height||width>4096||height>4096||pitch<width*4||pitch>65536||CGBitmapContextGetBitsPerPixel(c)!=32||CGBitmapContextGetBitsPerComponent(c)!=8)return false;
 auto format=CGBitmapContextGetBitmapInfo(c);auto alphaInfo=CGBitmapContextGetAlphaInfo(c);if((format&kCGBitmapByteOrderMask)!=kCGBitmapByteOrder32Big||(alphaInfo!=kCGImageAlphaPremultipliedLast&&alphaInfo!=kCGImageAlphaNoneSkipLast))return false;
 auto space=CGBitmapContextGetColorSpace(c);CFStringRef name=space?CGColorSpaceCopyName(space):nullptr;bool srgb=name&&CFEqual(name,kCGColorSpaceSRGB);if(name)CFRelease(name);if(!srgb)return false;
 CGPoint position=CGContextGetTextPosition(c);if(!std::isfinite(position.x)||!std::isfinite(position.y)||position.y!=std::floor(position.y))return false;
 // Rotation is explicitly provided by a future validated display-policy link;
 // until then this adapter handles the native horizontal RGB layout only.
 std::unique_ptr<LatitudeLCDTextLayer,decltype(&LatitudeLCDDestroyTextLayer)>layer(LatitudeLCDCreateTextLayer(line,position.x,height-position.y,0),LatitudeLCDDestroyTextLayer);if(!layer)return false;
 int x,y;unsigned w,h;if(!LatitudeLCDTextLayerBounds(layer.get(),&x,&y,&w,&h))return false;
 int right=std::min((int)width,x+(int)w),bottom=std::min((int)height,y+(int)h);x=std::max(0,x);y=std::max(0,y);if(right<=x||bottom<=y)return false;w=(unsigned)(right-x);h=(unsigned)(bottom-y);if((size_t)w*h>1024*1024)return false;
 std::vector<uint8_t>pixels((size_t)w*h*4);for(unsigned row=0;row<h;row++)memcpy(pixels.data()+(size_t)row*w*4,data+(size_t)(y+row)*pitch+x*4,(size_t)w*4);
 if(alphaInfo==kCGImageAlphaNoneSkipLast)for(size_t i=3;i<pixels.size();i+=4)pixels[i]=255;
 if(!LatitudeLCDCompositeTextLayerAt(layer.get(),pixels.data(),pixels.size(),w,h,w*4,0,x,y))return false;
 CFDataRef copy=CFDataCreate(kCFAllocatorDefault,pixels.data(),(CFIndex)pixels.size());CGDataProviderRef provider=copy?CGDataProviderCreateWithCFData(copy):nullptr;if(copy)CFRelease(copy);if(!provider)return false;
 CGImageRef image=CGImageCreate(w,h,8,32,w*4,space,kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big,provider,nullptr,false,kCGRenderingIntentDefault);CGDataProviderRelease(provider);if(!image)return false;
 // Draw through Quartz so even nonrectangular clip paths remain respected.
 static auto save=&CGContextSaveGState;static auto restore=&CGContextRestoreGState;save(c);CGContextSetInterpolationQuality(c,kCGInterpolationNone);CGContextDrawImage(c,CGRectMake(x,height-y-h,w,h),image);restore(c);CGImageRelease(image);return true;
}
static void latitudeLineDraw(CTLineRef line,CGContextRef c){
 static auto real=&CTLineDraw;nativeCalls++;
 if(inside){fallbacks++;real(line,c);return;}inside=true;bool done=false;try{done=line&&c&&draw(line,c);}catch(...){done=false;}inside=false;
 if(done)converted++;else{fallbacks++;real(line,c);}
}
extern "C" void LatitudeLCDInterposeStats(uint64_t*convertedOut,uint64_t*fallbackOut,uint64_t*liveOut){if(convertedOut)*convertedOut=converted.load();if(fallbackOut)*fallbackOut=fallbacks.load();if(liveOut){std::lock_guard<std::mutex>g(stateLock);*liveOut=contexts.size();}}
#define INTERPOSE(replacement,replacee) __attribute__((used)) static const struct{const void*r;const void*o;} pair_##replacement __attribute__((section("__DATA,__interpose")))={(const void*)replacement,(const void*)replacee};
INTERPOSE(latitudeBitmapCreate,CGBitmapContextCreate)
INTERPOSE(latitudeBitmapCreateWithData,CGBitmapContextCreateWithData)
INTERPOSE(latitudeSave,CGContextSaveGState)
INTERPOSE(latitudeRestore,CGContextRestoreGState)
INTERPOSE(latitudeShadow,CGContextSetShadow)
INTERPOSE(latitudeShadowColor,CGContextSetShadowWithColor)
INTERPOSE(latitudeTextMode,CGContextSetTextDrawingMode)
INTERPOSE(latitudeBegin,CGContextBeginTransparencyLayer)
INTERPOSE(latitudeBeginRect,CGContextBeginTransparencyLayerWithRect)
INTERPOSE(latitudeEnd,CGContextEndTransparencyLayer)
INTERPOSE(latitudeLineDraw,CTLineDraw)
