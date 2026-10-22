/* SPDX-License-Identifier: MIT */
#include "boot_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc==2&&!strcmp(argv[1],"sha")){
  D27Sha256 s;d27_sha_init(&s);uint8_t b[137],h[32];size_t n;
  while((n=fread(b,1,sizeof(b),stdin)))d27_sha_update(&s,b,n);
  d27_sha_final(&s,h);for(unsigned i=0;i<32;i++)printf("%02x",h[i]);puts("");return 0;
 }
 if(argc!=2)return 2;FILE*f=fopen(argv[1],"rb");if(!f)return 2;
 if(fseek(f,0,SEEK_END))return 2;long size=ftell(f);if(size<0)return 2;rewind(f);
 size_t n=(size_t)size;if(n>1024*1024+32)n=1024*1024+32;
 uint8_t *b=malloc(n?n:1);if(fread(b,1,n,f)!=n)return 2;fclose(f);
 int result=d27_kernel_header(b,n,(uint64_t)size);printf("%d\n",result);free(b);return result?1:0;
}
