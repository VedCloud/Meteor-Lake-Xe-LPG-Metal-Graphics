/* SPDX-License-Identifier: MIT
 * Read-only comparison of ordinary allocated RAM, never device MMIO.
 * The SSE2 entry requires CPU+CR0/CR4 qualification by its firmware caller.
 */
#ifndef D27_FRAME_EQUAL_H
#define D27_FRAME_EQUAL_H
#include <stddef.h>
#include <stdint.h>
typedef uint64_t d27_scan_word __attribute__((aligned(1),may_alias));
static inline int d27_frame_equal_scalar(const void *left,const void *right,size_t bytes){
 const unsigned char*a=left,*b=right;
 while(bytes>=8){if(*(const d27_scan_word*)a!=*(const d27_scan_word*)b)return 0;a+=8;b+=8;bytes-=8;}
 while(bytes--){if(*a++!=*b++)return 0;}return 1;
}
/* Whole-cacheline scalar comparison: no SIMD state required. */
static inline int d27_frame_equal_unrolled(const void *left,const void *right,size_t bytes){
 const unsigned char*a=left,*b=right;
 while(bytes>=64){
  const d27_scan_word*x=(const void*)a,*y=(const void*)b;
  uint64_t different=(x[0]^y[0])|(x[1]^y[1])|(x[2]^y[2])|(x[3]^y[3])|
                     (x[4]^y[4])|(x[5]^y[5])|(x[6]^y[6])|(x[7]^y[7]);
  if(different)return 0;a+=64;b+=64;bytes-=64;
 }
 return d27_frame_equal_scalar(a,b,bytes);
}
#endif
