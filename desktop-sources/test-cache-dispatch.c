#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <cpuid.h>
#include <immintrin.h>
static int available=1,optimized;
static unsigned fences,regular_calls,opt_calls;
static uintptr_t low,high;
static int fake_cpuid(unsigned leaf,unsigned *a,unsigned *b,unsigned *c,unsigned *d)
{
    assert(leaf==1);*a=*c=0;*b=8<<8;*d=available ? 1<<19:0;return 1;
}
static int fake_count(unsigned leaf,unsigned sub,unsigned *a,unsigned *b,unsigned *c,unsigned *d)
{
    assert(leaf==7&&!sub);*a=*c=*d=0;*b=optimized ? 1<<23:0;return 1;
}
static void fake_fence(void){assert(fences<2);fences++;}
static void check(const void *p){assert(fences==1&&(uintptr_t)p>=low&&(uintptr_t)p<=high);}
static void fake_regular(const void *p){check(p);regular_calls++;}
static void fake_opt(const void *p){check(p);opt_calls++;}
#define __get_cpuid fake_cpuid
#define __get_cpuid_count fake_count
#define _mm_mfence fake_fence
#define _mm_clflush fake_regular
#define _mm_clflushopt fake_opt
#include "nvmtl_cpu_cache.h"
int main(void)
{
    low=0x1003;high=low+129-1;
    for(optimized=0;optimized<=1;optimized++){
        fences=regular_calls=opt_calls=0;
        assert(!nvmtl_cpu_cache_sync((void *)low,129,129));
        assert(fences==2&&regular_calls==(optimized?0:3)&&opt_calls==(optimized?3:0));
    }
    fences=regular_calls=opt_calls=0;available=0;
    assert(nvmtl_cpu_cache_sync((void *)low,129,129));
    assert(!fences&&!regular_calls&&!opt_calls);
    puts("PASS actual cache dispatch with injected CPUID/instruction callbacks: optimized/legacy selection, both fence boundaries, exact in-range line coverage, unsupported refusal; optimized instruction not executed on unsupported build host");
}
