#pragma once
#include <stdint.h>
#include <stddef.h>
// IEEE binary32 to signed Q16, without executing floating-point arithmetic.
static inline bool latitudeFloatQ16(uint32_t bits,int32_t *value){
 if(!value)return false;unsigned e=(bits>>23)&255u;uint64_t m=bits&0x7fffffu;
 if(e==255)return false;if(e==0){*value=0;return true;}m|=0x800000u;
 int shift=(int)e-127-23+16;
 if(shift>=0){if(shift>8)return false;m<<=shift;}
 else {unsigned n=(unsigned)-shift;m=n>=64?0:(m+((uint64_t)1<<(n-1)))>>n;}
 if(m>4u*65536u)return false;*value=(bits>>31)?-(int32_t)m:(int32_t)m;return true;
}
static inline uint16_t latitudeColourClamp(int64_t v){return v<=0?0:v>=65535?65535:(uint16_t)v;}
static inline uint16_t latitudeColourLookup(const uint16_t table[256],uint16_t v){
 unsigned lo=v/257,rem=v%257,hi=lo<255?lo+1:lo;
 return (uint16_t)(((uint32_t)table[lo]*(257-rem)+(uint32_t)table[hi]*rem+128)/257);
}
