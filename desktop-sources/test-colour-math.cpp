#include <assert.h>
#include <initializer_list>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "latitude_colour_math.h"
int main(){
 for(int i=-262144;i<=262144;i+=13){float f=(float)i/65536;uint32_t b;memcpy(&b,&f,4);int32_t q;assert(latitudeFloatQ16(b,&q)&&q==i);}
 for(uint32_t b:{0x7f800000u,0xff800000u,0x7fc00000u,0x40a00000u}){int32_t q;assert(!latitudeFloatQ16(b,&q));}
 uint16_t t[256];for(unsigned i=0;i<256;i++)t[i]=i*257;
 for(unsigned i=0;i<65536;i++)assert(latitudeColourLookup(t,i)==i);
 assert(latitudeColourClamp(-1)==0&&latitudeColourClamp(80000)==65535);
 puts("PASS colour math: finite signed Q16 conversion/bounds, NaN/Inf refusal, exact identity interpolation, clamp.");
}
