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
struct DeferredText;
struct FontState {CGFontRef font;explicit FontState(CGFontRef f):font(CGFontRetain(f)){}~FontState(){CGFontRelease(font);}};
struct Flags {bool shadow=false,clipModified=false;CGTextDrawingMode text=kCGTextFill;std::shared_ptr<FontState>font;CGFloat fontSize=0;};
struct Context {bool known=true;unsigned layers=0;Flags current;std::vector<Flags>saved;std::shared_ptr<DeferredText>deferred;};
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
static bool draw(CTLineRef,CGContextRef);
#include "LatitudeLCDDeferred.h"
#pragma clang diagnostic push
// AppKit still calls this deprecated entry point in the observed runtime.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "LatitudeLCDGlyphs.h"
// Share the same validated pixel-layout conversion with native glyph/image
// composition. AppKit commonly uses little-endian BGRA, not big-endian RGBA.
static bool draw(CTLineRef line,CGContextRef c){
 BitmapView v;if(!bitmapView(c,v))return false;
 auto m=CGContextGetTextMatrix(c);auto p=CGContextGetTextPosition(c);
 if(m.a!=1||m.b||m.c||m.d!=1||!std::isfinite(p.x)||!std::isfinite(p.y)||p.y!=floor(p.y))return false;
 auto layer=std::shared_ptr<LatitudeLCDTextLayer>(LatitudeLCDCreateTextLayer(line,p.x,v.h-p.y,0),LatitudeLCDDestroyTextLayer);
 if(!layer)return false;
 auto image=std::make_shared<DeferredText>();image->width=v.w;image->height=v.h;image->lines.push_back(layer);
 return drawDeferredImage(c,CGRectMake(0,0,v.w,v.h),image,[](){});
}
static void latitudeLineDraw(CTLineRef line,CGContextRef c){
 static auto real=&CTLineDraw;nativeCalls++;
 if(inside){fallbacks++;real(line,c);return;}inside=true;bool done=false;try{done=line&&c&&draw(line,c);}catch(...){done=false;}inside=false;
 if(done)converted++;else{bool deferred=false;inside=true;try{deferred=line&&c&&deferNativeLine(line,c);}catch(...){deferred=false;}inside=false;if(!deferred){fallbacks++;real(line,c);}}
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

INTERPOSE(latitudeBitmapImage,CGBitmapContextCreateImage)
INTERPOSE(latitudeDrawImage,CGContextDrawImage)
INTERPOSE(latitudeClip,CGContextClip)
INTERPOSE(latitudeEOClip,CGContextEOClip)
INTERPOSE(latitudeClipRect,CGContextClipToRect)
INTERPOSE(latitudeClipRects,CGContextClipToRects)
INTERPOSE(latitudeClipMask,CGContextClipToMask)
INTERPOSE(latitudeResetClip,CGContextResetClip)
INTERPOSE(latitudeSetFont,CGContextSetFont)
INTERPOSE(latitudeSetFontSize,CGContextSetFontSize)
INTERPOSE(latitudeGlyphAdvances,CGContextShowGlyphsWithAdvances)

#pragma clang diagnostic pop

// Modern libdyld exports dyld_dynamic_interpose but implements it as a no-op.
// Do not report patched image counts or attach this to Metal's late +load.
// Startup DYLD interposition remains the tested deployment contract.
extern "C" uint64_t LatitudeLCDEnableNativeInterception(){return 0;}
