#import <Cocoa/Cocoa.h>
#include <dlfcn.h>
#include <vector>
#include <cassert>
#include <cstdio>
using Stats=void(*)(uint64_t*,uint64_t*,uint64_t*);
using Deferred=void(*)(uint64_t*,uint64_t*,uint64_t*,uint64_t*);
static void scene(NSFont*font,bool transparent,bool bgra=false,bool targetBGRA=false){@autoreleasepool{
 auto space=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);std::vector<uint8_t>pixels(400*100*4,transparent?0:255),target(400*100*4,255);
 auto sourceFormat=bgra?(kCGImageAlphaPremultipliedFirst|kCGBitmapByteOrder32Little):(kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big);
 auto targetFormat=targetBGRA?(kCGImageAlphaPremultipliedFirst|kCGBitmapByteOrder32Little):(kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big);
 auto c=CGBitmapContextCreate(pixels.data(),400,100,8,1600,space,sourceFormat);assert(c);
 [NSGraphicsContext saveGraphicsState];NSGraphicsContext.currentContext=[NSGraphicsContext graphicsContextWithCGContext:c flipped:NO];
 [@"Native AppKit office 0123" drawAtPoint:NSMakePoint(10,30) withAttributes:@{NSFontAttributeName:font}];
 [NSGraphicsContext restoreGraphicsState];
 if(transparent){
  auto image=CGBitmapContextCreateImage(c);assert(image);auto destination=CGBitmapContextCreate(target.data(),400,100,8,1600,space,targetFormat);assert(destination);
  CGContextDrawImage(destination,CGRectMake(0,0,400,100),image);CFRelease(image);CFRelease(destination);
 }
 auto&result=transparent?target:pixels;unsigned color=0;for(size_t n=0;n<result.size();n+=4)if(result[n]!=result[n+1]||result[n+1]!=result[n+2])color++;
 if(dlsym(RTLD_DEFAULT,"LatitudeLCDInterposeStats"))assert(color>50);
 CFRelease(c);CFRelease(space);
}}
int main(int argc,const char*argv[]){@autoreleasepool{
 if(argc==2){
  // Initialize and draw through AppKit BEFORE loading the renderer, matching
  // late Metal-plugin loading rather than a startup DYLD environment.
  scene([NSFont fontWithName:@"ArialMT" size:20],false);
  auto library=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);if(!library){fprintf(stderr,"%s\n",dlerror());return 2;}
  auto activate=(uint64_t(*)())dlsym(library,"LatitudeLCDEnableNativeInterception");assert(activate&&activate()==0);
  puts("PASS late activation explicitly reports unsupported; no false patched-image claim.");return 3;
 }
 auto stats=(Stats)dlsym(RTLD_DEFAULT,"LatitudeLCDInterposeStats");auto deferred=(Deferred)dlsym(RTLD_DEFAULT,"LatitudeLCDDeferredStats");
 // RTLD_LOCAL deliberately hides the late library from unrelated dlsym callers.
 if(argc==2){auto h=dlopen(argv[1],RTLD_NOW|RTLD_NOLOAD);assert(h);stats=(Stats)dlsym(h,"LatitudeLCDInterposeStats");deferred=(Deferred)dlsym(h,"LatitudeLCDDeferredStats");}
 assert(stats&&deferred);uint64_t before=0,after=0;stats(&before,nullptr,nullptr);
 scene([NSFont fontWithName:@"ArialMT" size:20],false);scene([NSFont systemFontOfSize:20],false);scene([NSFont boldSystemFontOfSize:18],false);
 stats(&after,nullptr,nullptr);assert(after>=before+3);
 uint64_t images=0,imageAfter=0;deferred(nullptr,&images,nullptr,nullptr);
 scene([NSFont systemFontOfSize:20],true);deferred(nullptr,&imageAfter,nullptr,nullptr);assert(imageAfter==images+1);
 // Check both native layouts and every source/destination channel-order pair.
 scene([NSFont systemFontOfSize:20],false,true);
 deferred(nullptr,&images,nullptr,nullptr);
 for(bool sourceBGRA:{false,true})for(bool destBGRA:{false,true})scene([NSFont systemFontOfSize:20],true,sourceBGRA,destBGRA);
 deferred(nullptr,&imageAfter,nullptr,nullptr);assert(imageAfter==images+4);
 puts("PASS ordinary AppKit NSAttributedString drawing: Arial, native San Francisco variable regular/bold, opaque RGB coverage and transparent image composition. RGBA/BGRA opaque and all four transparent source/destination format pairs verified. Startup loading verified; late activation is unsupported. No Apple system-app deployment or GPU-backed CA claim.");
}}
