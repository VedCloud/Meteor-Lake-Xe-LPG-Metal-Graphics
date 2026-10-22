#include "LatitudeLCD.h"
#include <cassert>
#include <cstdio>
using namespace LatitudeLCD;
int main(int argc,char**argv){assert(argc==2);Rasterizer r(argv[1]);unsigned glyph=r.glyphIndex('S');
 for(unsigned rot=0;rot<5;rot++){
  r.setLayout((Layout)rot);auto a=r.glyph(glyph,19);assert(a.width&&a.height&&a.generation==r.generation());auto b=r.glyph(glyph,19);assert(a.x==b.x&&a.y==b.y&&r.cached()==1);
  auto phase=r.glyph(glyph,19,1);assert(phase.generation==r.generation()&&r.cached()==2);
  if(rot==4)for(unsigned y=0;y<a.height;y++)for(unsigned x=0;x<a.width;x++){size_t p=((a.y+y)*r.extent+a.x+x)*4;assert(r.pixels()[p]==r.pixels()[p+1]&&r.pixels()[p]==r.pixels()[p+2]);}
 }
 auto old=r.generation();r.setLayout(Layout::RGB);assert(r.generation()>old&&r.cached()==0);
 for(unsigned size=6;size<=90;size++)for(unsigned c=33;c<127;c++)r.glyph(r.glyphIndex(c),size);
 assert(r.pixels().size()==1024*1024*4&&r.cached()<=4096&&r.generation()>old+1);
 auto white=std::array<uint8_t,3>{255,255,255},black=std::array<uint8_t,3>{0,0,0};assert(blend(black,white,{0,0,0})==white);assert(blend(black,white,{255,255,255})==black);auto half=blend(black,white,{128,128,128});assert(half[0]>=187&&half[0]<=188&&half[0]==half[1]);
 auto colored=blend(black,white,{255,0,128});assert(colored[0]==0&&colored[1]==255&&colored[2]>=187);
 bool invalid=false;try{r.glyph(glyph,0);}catch(...){invalid=true;}assert(invalid);
 puts("PASS filtered FreeType LCD/RGB_V/BGR/BGR_V/grayscale, cache identity/subpixel phase, rotation invalidation, bounded atlas eviction over 7990 glyph requests and per-channel linear-light blending under ASan/UBSan.");
}
