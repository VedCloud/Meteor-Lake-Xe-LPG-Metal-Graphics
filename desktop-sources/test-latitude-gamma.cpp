#include "latitude_gamma.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(){
 LatitudeGamma g;latitudeGammaIdentity(&g);assert(!g.active);uint8_t u8[3*256];uint16_t u16[3*1024];
 for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++)u8[c*256+i]=(uint8_t)i;
 assert(latitudeGammaBuild(3,256,8,u8,&g)&&!g.active);
 for(unsigned c=0;c<3;c++)for(unsigned i=0;i<1024;i++)u16[c*1024+i]=(uint16_t)((i*65535u+511)/1023);
 assert(latitudeGammaBuild(3,1024,16,u16,&g)&&!g.active);
 uint16_t endpoints[6]={0,65535,0,59000,0,32768};assert(latitudeGammaBuild(3,2,16,endpoints,&g)&&g.active);
 uint32_t src[8]={0xa5ffffff,0xa5000000,0x11223344,0xdeadbeef,0xff887766,0x99876543,0,0},dst[8];memset(dst,0x5a,sizeof dst);
 assert(latitudeGammaCopy(dst,src,2,2,16,16,&g));assert(dst[0]==0xa5ffe680&&dst[1]==0xa5000000&&dst[2]==0x5a5a5a5a&&dst[3]==0x5a5a5a5a);assert((dst[4]&0xff000000)==0xff000000);
 assert(!latitudeGammaCopy(src,src,2,2,16,16,&g));assert(!latitudeGammaCopy(dst,src,3,2,8,16,&g));
 assert(!latitudeGammaBuild(2,256,8,u8,&g));assert(!latitudeGammaBuild(3,1025,16,u16,&g));assert(!latitudeGammaBuild(3,256,17,u16,&g));assert(!latitudeGammaBuild(3,256,8,nullptr,&g));
 for(unsigned width=1;width<=16;width++){uint16_t e[6]={0,(uint16_t)((1u<<width)-1),0,(uint16_t)((1u<<width)-1),0,(uint16_t)((1u<<width)-1)};uint8_t small[6]={0,(uint8_t)e[1],0,(uint8_t)e[3],0,(uint8_t)e[5]};assert(latitudeGammaBuild(3,2,width,width<=8?(void *)small:(void *)e,&g)&&!g.active);}
 assert(latitudeGammaBuild(0,0,0,nullptr,&g)&&!g.active);puts("PASS actual gamma decoder: 1..16bit, 2/256/1024 entries, identity reset; RGB warming to BGRA with alpha/padding preserved; invalid/in-place/overflow geometry refused. No display access.");
}
