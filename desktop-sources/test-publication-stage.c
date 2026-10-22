#include "nvmtl_surface_publication.h"
#include <assert.h>
#include <stdio.h>
int main(){
 void *a[3]={0};int l[3];for(int i=0;i<3;i++){l[i]=nvmtl_publication_acquire(4096,&a[i]);assert(l[i]>=0);for(int j=0;j<i;j++)assert(a[i]!=a[j]);}
 void *extra=0;assert(nvmtl_publication_acquire(4096,&extra)==-1);
 nvmtl_publication_release(l[1]);int n=nvmtl_publication_acquire(4096,&extra);assert(n==l[1]&&extra==a[1]);nvmtl_publication_release(n);nvmtl_publication_release(l[0]);nvmtl_publication_release(l[2]);
 assert(nvmtl_publication_acquire(NVMTL_PUBLICATION_MAX+1,&extra)<0);
 for(size_t bpp=1;bpp<=8;bpp*=2)for(size_t w=1;w<130;w++){
  size_t packed=0,bytes=0;assert(!nvmtl_publication_geometry(w,13,bpp,&packed,&bytes));
  unsigned char *s=malloc(bytes),*d=malloc((packed+31)*13);for(size_t i=0;i<bytes;i++)s[i]=(unsigned char)(i*17);
  memset(d,0xa5,(packed+31)*13);assert(!nvmtl_publication_copy(d,packed+31,s,packed,13));
  for(size_t y=0;y<13;y++){assert(!memcmp(d+y*(packed+31),s+y*packed,packed));for(size_t p=packed;p<packed+31;p++)assert(d[y*(packed+31)+p]==0xa5);}
  assert(!nvmtl_publication_copy(d,packed,s,packed,13));assert(!memcmp(d,s,bytes));free(s);free(d);
 }
 size_t p,b;assert(nvmtl_publication_geometry(SIZE_MAX,2,4,&p,&b));assert(nvmtl_publication_copy(a[0],3,a[1],4,2));
 puts("PASS actual 3-slot publication staging: exclusive leases/no waiting/reuse/cap; 516 odd-width R8/RG8/RGBA8/RGBA16 layouts with exact pixels and untouched padded rows; overflow rejected.");
}
