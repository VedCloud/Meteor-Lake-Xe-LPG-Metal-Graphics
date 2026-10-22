#pragma once
#include <stddef.h>
#include <stdint.h>
#if defined(__x86_64__)
#include <cpuid.h>
#include <emmintrin.h>
#endif

static size_t nvmtl_cpu_cache_line_size(void)
{
#if defined(__x86_64__)
    unsigned a,b,c,d;
    if(!__get_cpuid(1,&a,&b,&c,&d) || !(d&(1u<<19)))return 0;
    size_t size=((b>>8)&255)*8;
    return size && !(size&(size-1)) ? size:0;
#else
    return 0;
#endif
}

static int nvmtl_cpu_cache_range(uintptr_t address,size_t bytes,size_t capacity,
                                size_t line,uintptr_t *last)
{
    if(!address || !bytes || bytes>capacity || !line || (line&(line-1)) ||
       bytes-1>UINTPTR_MAX-address)return -1;
    *last=address+bytes-1;
    return 0;
}

/* Addresses remain inside the allocation, including an unaligned first line. */
static int nvmtl_cpu_cache_next(uintptr_t address,uintptr_t last,size_t line,uintptr_t *next)
{
    size_t step=line-(address&(line-1));
    if(last-address<step)return 0;
    *next=address+step;
    return 1;
}

/* Port workaround for retained IOKit mappings. This supplements rather than
   replaces the Vulkan host barrier and successful GPU fence. */
static int nvmtl_cpu_cache_sync(void *base,size_t bytes,size_t capacity)
{
    const size_t line=nvmtl_cpu_cache_line_size();
    uintptr_t address=(uintptr_t)base,last;
    if(nvmtl_cpu_cache_range(address,bytes,capacity,line,&last))return -1;
#if defined(__x86_64__)
    _mm_mfence();
    do { _mm_clflush((const void *)address); }
    while(nvmtl_cpu_cache_next(address,last,line,&address));
    _mm_mfence();
    return 0;
#else
    return -1;
#endif
}
