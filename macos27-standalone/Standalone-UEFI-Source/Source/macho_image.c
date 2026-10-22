/* SPDX-License-Identifier: MIT */
#include "macho_image.h"
static uint32_t u32(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint64_t u64(const uint8_t*p){return u32(p)|(uint64_t)u32(p+4)<<32;}
int d27_image_plan(const uint8_t*p,size_t n,D27Image*out){
 if(!p||!out||n<32||u32(p)!=0xfeedfacf||u32(p+4)!=0x0100000c||u32(p+12)!=2)return 1;
 uint32_t count=u32(p+16),bytes=u32(p+20);
 if(!count||count>4096||bytes>1024*1024||bytes>n-32||count>bytes/8)return 2;
 D27Image plan={0};plan.base=UINT64_MAX;uint64_t top=0;unsigned entries=0;
 size_t at=32,end=32+(size_t)bytes;
 for(uint32_t i=0;i<count;i++){
  if(end-at<8)return 3;
  uint32_t kind=u32(p+at),size=u32(p+at+4);
  if(size<8||(size&7)||size>end-at)return 4;
  if(kind==0x19){
   if(size<72||u32(p+at+64)>(size-72)/80||plan.count==D27_MAX_SEGMENTS)return 5;
   D27Segment s={u64(p+at+24),u64(p+at+32),u64(p+at+40),u64(p+at+48),u32(p+at+60)};
   unsigned maximum=u32(p+at+56);
   if(!s.bytes||s.bytes>D27_MAX_IMAGE_BYTES||s.address>UINT64_MAX-s.bytes||
      s.file_bytes>s.bytes||s.offset>n||s.file_bytes>n-s.offset||
      (maximum&~7u)||(s.permissions&~maximum)||(s.permissions&(D27_WRITE|D27_EXEC))==(D27_WRITE|D27_EXEC))return 6;
   for(size_t j=0;j<plan.count;j++)if(s.address<plan.segments[j].address+plan.segments[j].bytes&&
      plan.segments[j].address<s.address+s.bytes)return 7;
   plan.segments[plan.count++]=s;
   if(s.address<plan.base)plan.base=s.address;
   if(s.address+s.bytes>top)top=s.address+s.bytes;
  }else if(kind==5){
   if(size!=288||u32(p+at+8)!=6||u32(p+at+12)!=68||entries++)return 8;
   plan.entry=u64(p+at+272);
  }else if(kind==0x80000028||kind==0x21||kind==0x2c){
   /* LC_MAIN and encrypted binaries are outside this loader contract. */
   return 9;
  }
  at+=size;
 }
 if(at!=end||entries!=1||!plan.count||top-plan.base>D27_MAX_IMAGE_BYTES||(plan.entry&3))return 10;
 plan.bytes=top-plan.base;int executable=0;
 for(size_t i=0;i<plan.count;i++){
  D27Segment*s=&plan.segments[i];
  if((s->permissions&D27_EXEC)&&plan.entry>=s->address&&plan.entry-s->address<s->file_bytes&&
     s->file_bytes-(plan.entry-s->address)>=4)executable=1;
 }
 if(!executable)return 11;
 *out=plan;return 0;
}
int d27_image_load(const uint8_t*file,size_t n,uint64_t base,uint8_t*buffer,size_t cap,
 D27Region*regions,size_t region_cap,D27Image*out,uint64_t*entry){
 D27Image plan;
 if(!buffer||!regions||!out||!entry||d27_image_plan(file,n,&plan))return 1;
 if(plan.bytes>cap||plan.count>region_cap||base>UINT64_MAX-plan.bytes)return 2;
 /* The caller supplies disjoint source and staging storage. Reject aliasing
  * before zero-fill; uintptr_t arithmetic itself is checked for overflow. */
 uintptr_t src=(uintptr_t)file,dst=(uintptr_t)buffer;
 if(src>UINTPTR_MAX-n||dst>UINTPTR_MAX-(size_t)plan.bytes||
    (src<dst+plan.bytes&&dst<src+n))return 3;
 for(size_t i=0;i<(size_t)plan.bytes;i++)buffer[i]=0;
 for(size_t i=0;i<plan.count;i++){
  D27Segment*s=&plan.segments[i];size_t relative=(size_t)(s->address-plan.base);
  for(size_t j=0;j<(size_t)s->file_bytes;j++)buffer[relative+j]=file[(size_t)s->offset+j];
  regions[i]=(D27Region){base+relative,buffer+relative,(size_t)s->bytes,s->permissions};
 }
 *entry=base+(plan.entry-plan.base);*out=plan;return 0;
}
