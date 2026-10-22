// Native glyph route used by the host AppKit NSAttributedString drawing path.
// Preserve the original call's text-position/state effects exactly once.
static std::atomic<uint64_t>glyphCalls{0},glyphConverted{0};
static void latitudeSetFont(CGContextRef c,CGFontRef font){
 try{auto retained=font?std::make_shared<FontState>(font):nullptr;std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.font=retained;}
 catch(...){std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.known=false;}
 CGContextSetFont(c,font);
}
static void latitudeSetFontSize(CGContextRef c,CGFloat size){
 {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())i->second.current.fontSize=size;}
 CGContextSetFontSize(c,size);
}
static bool glyphLayerColor(CGContextRef c,uint8_t rgb[3]){
 using GetColor=CGColorRef(*)(CGContextRef);static auto getColor=(GetColor)dlsym(RTLD_DEFAULT,"CGContextGetFillColorAsColor");if(!getColor)return false;
 auto color=getColor(c);if(!color||CGColorGetAlpha(color)!=1)return false;
 auto space=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);auto copy=space?CGColorCreateCopyByMatchingToColorSpace(space,kCGRenderingIntentDefault,color,nullptr):nullptr;if(space)CGColorSpaceRelease(space);if(!copy)return false;
 auto components=CGColorGetComponents(copy);for(unsigned n=0;n<3;n++)rgb[n]=(uint8_t)lround(std::clamp(components[n],0.,1.)*255);CGColorRelease(copy);return true;
}
static void latitudeGlyphAdvances(CGContextRef c,const CGGlyph*glyphs,const CGSize*advances,size_t count){
 glyphCalls++;
 auto native=[&](){CGContextShowGlyphsWithAdvances(c,glyphs,advances,count);};
 if(inside||!c||!glyphs||!advances||!count||count>16384){native();return;}
 inside=true;bool handled=false;
 try{
  BitmapView v;Flags flags;
  {std::lock_guard<std::mutex>g(stateLock);auto i=contexts.find(c);if(i!=contexts.end())flags=i->second.current;}
  auto matrix=CGContextGetTextMatrix(c);uint8_t rgb[3];
  if(flags.font&&std::isfinite(flags.fontSize)&&flags.fontSize>=6&&flags.fontSize<=256&&bitmapView(c,v)&&matrix.a==1&&!matrix.b&&!matrix.c&&matrix.d==1&&glyphLayerColor(c,rgb)){
   auto font=CTFontCreateWithGraphicsFont(flags.font->font,flags.fontSize,nullptr,nullptr);
   if(font){
    std::vector<CGPoint>positions(count);auto p=CGContextGetTextPosition(c);
    bool valid=true;for(size_t n=0;n<count;n++){positions[n]=p;if(!std::isfinite(advances[n].width)||!std::isfinite(advances[n].height)){valid=false;break;}p.x+=advances[n].width;p.y+=advances[n].height;}
    std::shared_ptr<LatitudeLCDTextLayer>layer(valid?LatitudeLCDCreateGlyphLayer(font,glyphs,positions.data(),count,v.h,rgb,0):nullptr,LatitudeLCDDestroyTextLayer);CFRelease(font);
    if(layer){
     auto image=std::make_shared<DeferredText>();image->width=v.w;image->height=v.h;image->lines.push_back(layer);
     handled=drawDeferredImage(c,CGRectMake(0,0,v.w,v.h),image,native);
     if(handled){converted++;glyphConverted++;}
     else handled=deferLayerNative(c,layer,native);
    }
   }
  }
 }catch(...){handled=false;}
 inside=false;if(!handled)native();
}
extern "C" void LatitudeLCDGlyphStats(uint64_t*calls,uint64_t*convertedOut){if(calls)*calls=glyphCalls.load();if(convertedOut)*convertedOut=glyphConverted.load();}
