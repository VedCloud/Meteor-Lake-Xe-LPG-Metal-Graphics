#include "latitude_gamma.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
int main(){
 LatitudeGamma g;latitudeGammaIdentity(&g);g.active=1;
 for(unsigned c=0;c<3;c++)for(unsigned x=0;x<256;x++)g.rgb[c][x]=(uint8_t)((x*(c+3)+17*c)&255);
 const size_t page=(size_t)sysconf(_SC_PAGESIZE);unsigned cases=0;
 for(size_t w:{1ul,44ul,127ul,128ul,129ul,255ul,256ul,257ul,1876ul,1920ul})for(size_t rows:{1ul,3ul})for(size_t pad:{0ul,4ul,64ul}){
  size_t sp=w*4+pad,dp=w*4+pad+4,sz=sp*(rows-1)+w*4,dz=dp*(rows-1)+w*4;
  size_t sa=(sz+page-1)/page*page,da=(dz+page-1)/page*page;
  auto sm=(uint8_t*)mmap(nullptr,sa+page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
  auto dm=(uint8_t*)mmap(nullptr,da+page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);assert(sm!=MAP_FAILED&&dm!=MAP_FAILED);
  assert(!mprotect(sm+sa,page,PROT_NONE)&&!mprotect(dm+da,page,PROT_NONE));
  auto s=sm+sa-sz,d=dm+da-dz;for(size_t i=0;i<sz;i++)s[i]=(uint8_t)(i*29+11);memset(d,0xa5,dz);
  std::vector<uint8_t> original(s,s+sz);
  assert(latitudeGammaCopy((uint32_t*)d,(uint32_t*)s,w,rows,dp,sp,&g));
  for(size_t y=0;y<rows;y++){
   for(size_t x=0;x<w;x++){uint32_t v;memcpy(&v,s+y*sp+x*4,4);uint32_t expected=(v&0xff000000u)|((uint32_t)g.rgb[0][(v>>16)&255]<<16)|((uint32_t)g.rgb[1][(v>>8)&255]<<8)|g.rgb[2][v&255];uint32_t actual;memcpy(&actual,d+y*dp+x*4,4);assert(actual==expected);}
   if(y+1<rows)for(size_t i=w*4;i<dp;i++)assert(d[y*dp+i]==0xa5);
  }
  assert(!memcmp(original.data(),s,sz));munmap(sm,sa+page);munmap(dm,da+page);cases++;
 }
 uint32_t s[4]={},d[4]={};assert(!latitudeGammaCopy(d,s,1,SIZE_MAX,8,8,&g));assert(!latitudeGammaCopy(d,s,2,1,4,8,&g));assert(!latitudeGammaCopy(s,s,2,1,8,8,&g));
 printf("PASS %u gamma tile guard-page layouts: scalar-reference pixels/alpha, padding/source unchanged, 127/128/129 tile edges and overflow guards.\n",cases);
}
