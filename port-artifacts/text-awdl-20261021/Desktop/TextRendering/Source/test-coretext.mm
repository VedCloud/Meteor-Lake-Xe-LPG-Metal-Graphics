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

 LatitudeLCDTextLayer*layer=LatitudeLCDCreateTextLayer(line,10,40,0);assert(layer);
 // A layer owns its glyph coverage after the original font and text are gone.
 CFRelease(line);line=nullptr;CFRelease(font);font=nullptr;
 std::vector<uint8_t>white(900*80*4,255),gray(900*80*4,140);for(size_t i=3;i<gray.size();i+=4)gray[i]=255;
 auto untouched=white;
 assert(!LatitudeLCDCompositeTextLayer(layer,white.data(),white.size(),900,80,900*4,1)&&white==untouched);
 assert(!LatitudeLCDCompositeTextLayer(layer,white.data(),white.size()-1,900,80,900*4,0)&&white==untouched);
 assert(!LatitudeLCDCompositeTextLayer(layer,transparent.data(),transparent.size(),900,80,900*4,0)&&transparent==copy);
 assert(LatitudeLCDCompositeTextLayer(layer,white.data(),white.size(),900,80,900*4,0));
 int left,top;unsigned regionWidth,regionHeight;assert(LatitudeLCDTextLayerBounds(layer,&left,&top,&regionWidth,&regionHeight));assert(left>=0&&top>=0&&left+(int)regionWidth<=900&&top+(int)regionHeight<=80);
 std::vector<uint8_t>region((size_t)regionWidth*regionHeight*4,255);
 assert(LatitudeLCDCompositeTextLayerAt(layer,region.data(),region.size(),regionWidth,regionHeight,regionWidth*4,0,left,top));
 for(unsigned y=0;y<regionHeight;y++)for(unsigned x=0;x<regionWidth*4;x++)assert(region[(size_t)y*regionWidth*4+x]==white[(size_t)(y+top)*900*4+left*4+x]);
 assert(LatitudeLCDCompositeTextLayer(layer,gray.data(),gray.size(),900,80,900*4,0));assert(white!=untouched&&white!=gray);
 LatitudeLCDDestroyTextLayer(layer);
puts("PASS real CoreText-shaped glyph runs into FreeType LCD atlas in four physical layouts; native glyph IDs, per-channel output, opaque alpha and untouched transparent/invalid destinations under ASan/UBSan. Deferred component-alpha ownership, late opaque composition, short-buffer refusal and rotation invalidation passed. No global font interception or GPU claim.");
}}
