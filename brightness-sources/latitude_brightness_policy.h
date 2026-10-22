#pragma once
#include <stdint.h>
#include <math.h>
static inline int latitudeBrightnessKey(unsigned code,uint64_t mods){
 const uint64_t mask=(1ull<<17)|(1ull<<18)|(1ull<<19)|(1ull<<20);
 mods&=mask;if(mods!=0&&mods!=((1ull<<18)|(1ull<<19)))return 0;
 return code==97?-1:code==98?1:0;
}
static inline int latitudeBrightnessSystem(unsigned subtype,unsigned code,unsigned state){return subtype==8&&state==10?(code==2?1:code==3?-1:0):0;}
static inline float latitudeBrightnessStep(float value,int dir){return fmaxf(0,fminf(1,value+dir/16.0f));}
