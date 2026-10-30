/* SPDX-License-Identifier: MIT — oracle and protected-page boundary tests. */
#include "frame-equal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
static unsigned long checks;
static void check(const void*a,const void*b,size_t n){
 int expected=memcmp(a,b,n)==0;
 if(d27_frame_equal_scalar(a,b,n)!=expected)abort();
#if defined(__x86_64__)
 if(d27_frame_equal_sse2(a,b,n)!=expected)abort();
#endif
 if(d27_frame_equal_unrolled(a,b,n)!=expected)abort();
 checks++;
}
int main(void){
 unsigned char *a=malloc(4200),*b=malloc(4200);if(!a||!b)abort();
 for(size_t n=0;n<=4097;n+=n<65?1:17){
  for(unsigned x=0;x<16;x++)for(unsigned y=0;y<16;y++){
   memset(a,0x9b,4200);memset(b,0x9b,4200);check(a+x,b+y,n);
   if(n){b[y+n-1]^=0x80;check(a+x,b+y,n);b[y+n-1]^=0x80;b[y+n/2]^=0x01;check(a+x,b+y,n);}
  }
 }
 long page=sysconf(_SC_PAGESIZE);if(page<=0)abort();
 unsigned char*p=mmap(NULL,2*page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
 unsigned char*q=mmap(NULL,2*page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
 if(p==MAP_FAILED||q==MAP_FAILED||mprotect(p+page,page,PROT_NONE)||mprotect(q+page,page,PROT_NONE))abort();
 memset(p,0x3a,page);memset(q,0x3a,page);
 for(size_t n=0;n<=256;n++){
  check(p+page-n,q+page-n,n);
  if(n){q[page-1]^=1;check(p+page-n,q+page-n,n);q[page-1]^=1;}
 }
 munmap(p,2*page);munmap(q,2*page);free(a);free(b);
 printf("PASS frame equality: %lu oracle cases, all alignments, final/middle-byte mismatches and protected-page tails\n",checks);return 0;
}
