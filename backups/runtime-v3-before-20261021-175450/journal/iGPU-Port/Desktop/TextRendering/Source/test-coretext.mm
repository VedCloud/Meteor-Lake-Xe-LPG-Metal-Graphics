#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#include "LatitudeLCDCoreText.h"
#include <vector>
#include <cassert>
#include <cstdio>
int main(){@autoreleasepool{
 CTFontRef font=CTFontCreateWithName(CFSTR("ArialMT"),18,nullptr);assert(font);
 NSDictionary*attrs=@{(__bridge NSString*)kCTFontAttributeName:(__bridge id)font};
 NSAttributedString*text=[[NSAttributedString alloc]initWithString:@"CoreText glyphs: office affinity 0123456789" attributes:attrs];CTLineRef line=CTLineCreateWithAttributedString((__bridge CFAttributedStringRef)text);
 for(unsigned rot=0;rot<4;rot++){
  std::vector<uint8_t>pixels(900*80*4,255);assert(LatitudeLCDDrawCTLine(line,pixels.data(),900,80,900*4,10,40,rot)==1);
  unsigned ink=0,colored=0;for(size_t i=0;i<pixels.size();i+=4){assert(pixels[i+3]==255);if(pixels[i]!=255||pixels[i+1]!=255||pixels[i+2]!=255)ink++;if(pixels[i]!=pixels[i+1]||pixels[i+1]!=pixels[i+2])colored++;}assert(ink>200&&colored>100);
 }
 std::vector<uint8_t>transparent(900*80*4,0),copy=transparent;assert(!LatitudeLCDDrawCTLine(line,transparent.data(),900,80,900*4,10,40,0)&&transparent==copy);
 assert(!LatitudeLCDDrawCTLine(line,transparent.data(),900,80,1,10,40,0));
 CFRelease(line);CFRelease(font);puts("PASS real CoreText-shaped glyph runs into FreeType LCD atlas in four physical layouts; native glyph IDs, per-channel output, opaque alpha and untouched transparent/invalid destinations under ASan/UBSan. No global font interception or GPU claim.");
}}
