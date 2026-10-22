#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "nvmtl_cpu_cache.h"
int main(void)
{
    uintptr_t last,next;
    assert(nvmtl_cpu_cache_range(0,1,1,64,&last));
    assert(nvmtl_cpu_cache_range(16,0,1,64,&last));
    assert(nvmtl_cpu_cache_range(16,2,1,64,&last));
    assert(nvmtl_cpu_cache_range(UINTPTR_MAX,2,2,64,&last));
    assert(nvmtl_cpu_cache_range(16,1,1,3,&last));
    assert(nvmtl_cpu_cache_range(16,1,1,0,&last));
    for(size_t line=8;line<=256;line*=2)
      for(uintptr_t offset=0;offset<line;offset++)
        for(size_t bytes=1;bytes<=1024;bytes++){
            uintptr_t base=4096+offset,address=base;
            assert(!nvmtl_cpu_cache_range(base,bytes,bytes,line,&last));
            unsigned count=0;
            do{
                assert(address>=base&&address<=last);
                if(count)assert(!(address&(line-1)));
                count++;
            }while(nvmtl_cpu_cache_next(address,last,line,&address));
            assert(count==((base&(line-1))+bytes+line-1)/line);
        }
    assert(!nvmtl_cpu_cache_range(UINTPTR_MAX,1,1,64,&last));
    assert(!nvmtl_cpu_cache_next(UINTPTR_MAX,last,64,&next));
    size_t page=(size_t)sysconf(_SC_PAGESIZE);
    char *mapping=mmap(NULL,3*page,PROT_NONE,MAP_PRIVATE|MAP_ANON,-1,0);
    assert(mapping!=MAP_FAILED);
    char *data=mapping+page;assert(!mprotect(data,page,PROT_READ|PROT_WRITE));
    for(size_t i=0;i<page;i++)data[i]=(char)i;
    if(nvmtl_cpu_cache_line_size()){
        assert(!nvmtl_cpu_cache_sync(data+1,page-1,page-1));
        for(size_t i=0;i<page;i++)assert(data[i]==(char)i);
        assert(!nvmtl_cpu_cache_sync(data+page-1,1,1));
    }else assert(nvmtl_cpu_cache_sync(data,1,1));
    assert(nvmtl_cpu_cache_sync(data,page+1,page));
    assert(!munmap(mapping,3*page));
    puts("PASS CPU cache range: all alignments/8..256-byte lines, overflow/capacity/zero rejection; guard-page boundaries; actual x86 CLFLUSH/MFENCE preserves bytes");
}
