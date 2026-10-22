#include "latitude_front_copy.h"
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector>
int main(){
 const size_t page=(size_t)sysconf(_SC_PAGESIZE);
 for(size_t rows: {1ul,3ul,1080ul})for(size_t bytes:{4ul,20ul,7680ul})for(size_t pad:{0ul,4ul,64ul}) {
  size_t sp=bytes+pad,dp=bytes+pad+4,sz=sp*(rows-1)+bytes,dz=dp*(rows-1)+bytes;
  size_t sa=(sz+page-1)/page*page,da=(dz+page-1)/page*page;
  auto sm=(uint8_t *)mmap(0,sa+page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
  auto dm=(uint8_t *)mmap(0,da+page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);assert(sm!=MAP_FAILED&&dm!=MAP_FAILED);
  assert(!mprotect(sm+sa,page,PROT_NONE)&&!mprotect(dm+da,page,PROT_NONE));auto s=sm+sa-sz,d=dm+da-dz;
  for(size_t n=0;n<sz;n++)s[n]=(uint8_t)(n*29+17);memset(d,0xa5,dz);
  assert(latitudeFrontCopy(d,s,rows,bytes,dp,sp));
  for(size_t y=0;y<rows;y++){assert(!memcmp(d+y*dp,s+y*sp,bytes));if(y+1<rows)for(size_t n=bytes;n<dp;n++)assert(d[y*dp+n]==0xa5);}
  // Tight copy exercises a single repeat instruction over the entire span.
  memset(d,0xa5,dz);assert(latitudeFrontCopy(d,s,rows,bytes,bytes,bytes));assert(!memcmp(d,s,rows*bytes));
  munmap(sm,sa+page);munmap(dm,da+page);
 }
 uint32_t s[8]={},d[8]={};assert(!latitudeFrontCopy(d,s,2,16,8,16));assert(!latitudeFrontCopy(d,s,2,3,4,4));
 assert(!latitudeFrontCopy(d,s,SIZE_MAX,4,8,8));assert(latitudeFrontCopy(s,s,1,32,32,32));
 uint64_t next=0,skipped=0;assert(latitudeFrontNextDeadline(100,0,20,&next,&skipped)&&next==120&&!skipped);
 assert(latitudeFrontNextDeadline(123,120,20,&next,&skipped)&&next==140&&!skipped);
 assert(latitudeFrontNextDeadline(181,140,20,&next,&skipped)&&next==200&&skipped==2);
 assert(!latitudeFrontNextDeadline(UINT64_MAX-1,0,20,&next,&skipped));assert(!latitudeFrontNextDeadline(100,0,0,&next,&skipped));
 puts("PASS real front-copy helper: 81 tight/padded/last-byte guard-page layouts incl1080rows; exact pixels/padding, self-copy and overflow rejection. No framebuffer access.");
}
