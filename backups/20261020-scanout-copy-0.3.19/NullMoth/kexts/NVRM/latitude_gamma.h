#pragma once
#include <stdint.h>
#include <stddef.h>
#define LATITUDE_GAMMA_VERSION 1u
#define LATITUDE_GAMMA_FUNCTION "latitudeGetGamma"
struct LatitudeGamma {uint32_t version,active;uint64_t generation;uint8_t rgb[3][256];};
static inline void latitudeGammaIdentity(LatitudeGamma *g){g->version=LATITUDE_GAMMA_VERSION;g->active=0;g->generation=0;for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++)g->rgb[c][i]=(uint8_t)i;}
static inline uint32_t latitudeGammaEntry(const uint8_t *p,size_t index,unsigned width){return width<=8?p[index]:(uint32_t)p[index*2]|((uint32_t)p[index*2+1]<<8);}
static inline bool latitudeGammaBuild(unsigned channels,unsigned count,unsigned width,const void *data,LatitudeGamma *g){
 if(!g)return false;
 if(!channels&&!count&&!width&&!data){latitudeGammaIdentity(g);return true;}
 if((channels!=1&&channels!=3)||count<2||count>1024||width<1||width>16||!data)return false;
 const uint8_t *p=(const uint8_t *)data;const uint32_t maximum=(1u<<width)-1;g->version=LATITUDE_GAMMA_VERSION;g->generation=0;g->active=0;
 for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++){
  const unsigned src=channels==1?0:c,pos=i*(count-1),lo=pos/255,frac=pos%255,hi=lo+1<count?lo+1:lo;
  const uint32_t a=latitudeGammaEntry(p,(size_t)src*count+lo,width)&maximum,b=latitudeGammaEntry(p,(size_t)src*count+hi,width)&maximum;
  const uint32_t value=(a*(255-frac)+b*frac+maximum/2)/maximum;
  g->rgb[c][i]=(uint8_t)value;if(value!=i)g->active=1;
 }
 return true;
}
static inline bool latitudeGammaCopy(volatile uint32_t *dst,const volatile uint32_t *src,size_t width,size_t rows,size_t dpitch,size_t spitch,const LatitudeGamma *g){
 if(!g||g->version!=LATITUDE_GAMMA_VERSION||!src||!dst||dst==src||((uintptr_t)dst&3)||((uintptr_t)src&3)||!width||!rows||width>SIZE_MAX/4)return false;
 const size_t bytes=width*4;if(dpitch<bytes||spitch<bytes||(dpitch&3)||(spitch&3)||
   (rows>1&&(dpitch>(SIZE_MAX-bytes)/(rows-1)||spitch>(SIZE_MAX-bytes)/(rows-1))))return false;
 for(size_t y=0;y<rows;y++){
  auto d=(volatile uint32_t *)((volatile uint8_t *)dst+y*dpitch);auto s=(const volatile uint32_t *)((const volatile uint8_t *)src+y*spitch);
  for(size_t x=0;x<width;x++){uint32_t v=s[x];d[x]=(v&0xff000000u)|((uint32_t)g->rgb[0][(v>>16)&255]<<16)|((uint32_t)g->rgb[1][(v>>8)&255]<<8)|g->rgb[2][v&255];}
 }
#if defined(__x86_64__)
 __asm__ __volatile__("sfence" ::: "memory");
#endif
 return true;
}
