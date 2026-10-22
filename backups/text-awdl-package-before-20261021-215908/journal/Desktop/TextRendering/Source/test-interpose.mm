#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#include <dlfcn.h>
#include <cassert>
#include <vector>
#include <cstdio>
using Stats=void(*)(uint64_t*,uint64_t*,uint64_t*);
static Stats stats;
static uint64_t converted(){uint64_t c=0;stats(&c,nullptr,nullptr);return c;}
static uint64_t live(){uint64_t n=0;stats(nullptr,nullptr,&n);return n;}
static int releases=0;
static void releaseData(void*info,void*data){assert(info==&releases);releases++;free(data);}
int main(){@autoreleasepool{
 stats=(Stats)dlsym(RTLD_DEFAULT,"LatitudeLCDInterposeStats");assert(stats);
 auto space=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
 auto font=CTFontCreateWithName(CFSTR("ArialMT"),20,nullptr);
 NSDictionary*attributes=@{(__bridge NSString*)kCTFontAttributeName:(__bridge id)font};
 auto text=[[NSAttributedString alloc]initWithString:@"Native office 123" attributes:attributes];
 auto line=CTLineCreateWithAttributedString((__bridge CFAttributedStringRef)text);
 for(int trial=0;trial<10;trial++){
  std::vector<uint8_t>pixels(320*80*4,255);auto base=live();
  auto c=CGBitmapContextCreate(pixels.data(),320,80,8,1280,space,kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big);assert(c&&live()==base+1);
  CGContextSetTextPosition(c,10,20);auto before=converted();
  if(trial==1)CGContextSetAlpha(c,.5);
  if(trial==2)CGContextSetBlendMode(c,kCGBlendModeMultiply);
  if(trial==3)CGContextSetShadow(c,CGSizeMake(1,1),2);
  if(trial==4)CGContextTranslateCTM(c,1,0);
  if(trial==5)CGContextSetTextDrawingMode(c,kCGTextStroke);
  if(trial==6)CGContextSetTextMatrix(c,CGAffineTransformMakeScale(1.1,1));
  if(trial==7)for(size_t n=3;n<pixels.size();n+=4)pixels[n]=0;
  if(trial==8){CGContextSaveGState(c);CGContextSetShadow(c,CGSizeMake(1,1),2);CGContextRestoreGState(c);}
  if(trial==9)CGContextClipToRect(c,CGRectMake(10,0,30,80));

  CTLineDraw(line,c);bool expected=trial==0||trial==8||trial==9;assert(converted()==before+(expected?1:0));
  if(expected){unsigned ink=0,color=0;for(unsigned y=0;y<80;y++)for(unsigned x=0;x<320;x++){auto p=&pixels[(y*320+x)*4];if(p[0]!=255||p[1]!=255||p[2]!=255){ink++;assert(y>=40&&y<65);if(trial==9)assert(x>=10&&x<40);}if(p[0]!=p[1]||p[1]!=p[2])color++;}assert(ink>50&&color>25);}
  CFRelease(c);assert(live()==base);
 }
 auto base=live();for(int n=0;n<200;n++){
  auto data=calloc(320*80,4);auto c=CGBitmapContextCreateWithData(data,320,80,8,1280,space,kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big,releaseData,&releases);assert(c);CFRelease(c);assert(live()==base);
 }assert(releases==200);
 auto owned=CGBitmapContextCreate(nullptr,32,32,8,128,space,kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big);assert(owned&&live()==base+1);CFRelease(owned);assert(live()==base);
 CFRelease(line);CFRelease(font);CFRelease(space);
 puts("PASS ordinary CTLineDraw interception, RGB coverage, native fallback, clipping, coordinate placement, state restoration and 200 CFRelease callback lifetimes. Process-local test only.");
}}
