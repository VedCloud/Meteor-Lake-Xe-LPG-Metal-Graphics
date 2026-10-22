#pragma once
#include <stdint.h>
#include <stddef.h>
// The arguments describe validated linear RAM/framebuffer spans, not registers.
// REP MOVSB uses no floating-point/SIMD register state in the kernel.
static inline bool latitudeFrontCopy(volatile void *destination,const volatile void *source,size_t rows,size_t bytes,size_t dpitch,size_t spitch) {
 if(!destination || !source || !rows || !bytes || bytes>dpitch || bytes>spitch || (bytes&3) ||
    (rows>1 && (dpitch>(SIZE_MAX-bytes)/(rows-1) || spitch>(SIZE_MAX-bytes)/(rows-1))))return false;
 if(destination==source && dpitch==spitch)return true;
 auto d=(volatile uint8_t *)destination;auto s=(const volatile uint8_t *)source;
 if(dpitch==bytes && spitch==bytes){if(rows>SIZE_MAX/bytes)return false;bytes*=rows;rows=1;}
 for(size_t y=0;y<rows;y++){
#if defined(__x86_64__)
  auto to=d;auto from=s;size_t n=bytes;
  __asm__ __volatile__("rep movsb" : "+D"(to),"+S"(from),"+c"(n) :: "memory");
#else
  auto to=(volatile uint32_t *)d;auto from=(const volatile uint32_t *)s;
  for(size_t x=0;x<bytes/4;x++)to[x]=from[x];
#endif
  if(y+1<rows){d+=dpitch;s+=spitch;}
 }
#if defined(__x86_64__)
 __asm__ __volatile__("sfence" ::: "memory");
#endif
 return true;
}
static inline bool latitudeFrontNextDeadline(uint64_t now,uint64_t previous,uint64_t period,uint64_t *next,uint64_t *skipped) {
 if(!period || !next || !skipped)return false;*skipped=0;
 if(!previous){if(now>UINT64_MAX-period)return false;*next=now+period;return true;}
 if(previous>UINT64_MAX-period)return false;uint64_t d=previous+period;
 if(d<=now){uint64_t count=(now-d)/period+1;if(count>(UINT64_MAX-d)/period)return false;d+=count*period;*skipped=count;}
 *next=d;return true;
}
