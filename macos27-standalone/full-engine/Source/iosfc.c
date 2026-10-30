/* SPDX-License-Identifier: GPL-2.0-or-later
 * First-form IOSurface mapper transport port, based on ReimsVGPU contributors'
 * regs.rs, ring.rs, descriptor.rs, walk.rs and table.rs, revision
 * 2ce320931917e8a07a00243f53656f8cd11306b2 (GPL-2.0-or-later option).
 * This backend owns bounded guest-page mappings and CPU access, not an Apple
 * host IOSurface or a GPU renderer. Second-form/host-only allocation is NOT
 * offered. Unknown ring operations return unsupported, never fake success.
 */
#include "glue.h"
#include "iosfc.h"
#define PAGE 16384u
#define BADARG 0xe00002c2u
#define NOMEM 0xe00002bdu
#define UNSUPPORTED 0xe00002c7u
void bridge_output(const char*);
static int dma(IosfcDevice*d,uint64_t at,void*buf,unsigned n,int write){if(!n||sparse_ram_back(d->ram,at,n))return 0;return(write?uc_mem_write(d->ram->uc,at,buf,n):uc_mem_read(d->ram->uc,at,buf,n))==0;}
static uint32_t u32(const void*p){uint32_t v;memcpy(&v,p,4);return v;}
static uint64_t page_address(uint32_t entry){return(uint64_t)(entry>>2)<<14;}
static void release(IosfcSurface*s){if(s){free(s->pages);free(s);}}
void iosfc_close(IosfcDevice*d){if(!d)return;for(unsigned i=1;i<8192;i++){release(d->slot[i]);d->slot[i]=NULL;}}
static void bad(IosfcDevice*d,uint64_t off,unsigned size){char s[150];d->invalid++;snprintf(s,sizeof(s),"Unsupported IOSFC access: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(s);uc_emu_stop(d->ram->uc);}
static uint32_t descriptor_address(IosfcDevice*d,unsigned id,uint64_t*out){uint32_t entry;if(!id||id>=8192)return BADARG;if(!d->table||!dma(d,d->table+(id/32)*4,&entry,4,0)||!(entry&1))return NOMEM;uint64_t page=page_address(entry);if(sparse_ram_back(d->ram,page,PAGE))return NOMEM;*out=page+(id%32)*512;return 0;}
static uint32_t make_surface(IosfcDevice*d,unsigned id){
 uint64_t descriptor;uint32_t status=descriptor_address(d,id,&descriptor);if(status)return status;if(d->slot[id])return 0;
 IosfcSurface*s=calloc(1,sizeof(*s));if(!s)return NOMEM;s->descriptor=descriptor;
 if(!dma(d,descriptor,s->metadata,512,0)){release(s);return NOMEM;}
 uint32_t word=u32(s->metadata),bytes=u32(s->metadata+0x10);if(!(word&1)||!bytes||s->metadata[0x24]>7){release(s);return BADARG;}
 if(bytes>128*1024*1024u){release(s);return NOMEM;}
 s->bytes=bytes;s->page_count=((uint64_t)bytes+PAGE-1)/PAGE;s->pages=calloc(s->page_count,sizeof(uint64_t));if(!s->pages){release(s);return NOMEM;}
 uint64_t root=page_address(word);if(sparse_ram_back(d->ram,root,PAGE)){release(s);return NOMEM;}
 for(unsigned i=0;i<s->page_count;i++){
  uint64_t at=root+(uint64_t)i*4;
  if(s->page_count>4096){uint32_t table;if(!dma(d,root+(i/4096)*4,&table,4,0)||((table&3)!=3)){release(s);return BADARG;}at=page_address(table)+(i%4096)*4;}
  uint32_t entry;if(!dma(d,at,&entry,4,0)||((entry&3)!=1)){release(s);return BADARG;}
  s->pages[i]=page_address(entry);if(sparse_ram_back(d->ram,s->pages[i],PAGE)){release(s);return NOMEM;}
 }
 word|=2;if(!dma(d,descriptor,&word,4,1)){release(s);return NOMEM;}d->slot[id]=s;d->maps++;return 0;
}
static int copy_surface(IosfcDevice*d,IosfcSurface*s,uint64_t off,void*buf,unsigned bytes,int write){if(!s||off>s->bytes||bytes>s->bytes-off)return 0;for(unsigned done=0;done<bytes;){unsigned n=PAGE-(off%PAGE);if(n>bytes-done)n=bytes-done;if(!dma(d,s->pages[off/PAGE]+off%PAGE,(uint8_t*)buf+done,n,write))return 0;off+=n;done+=n;}return 1;}
int iosfc_copy(IosfcDevice*d,unsigned id,uint64_t off,void*buf,unsigned bytes,int write){if(!d||!id||id>=8192||!buf||!bytes||!d->slot[id]||(write&&(d->slot[id]->metadata[0x27]&1)))return 0;return copy_surface(d,d->slot[id],off,buf,bytes,write);}
static uint32_t command(IosfcDevice*d,uint32_t op,uint32_t id){if(!id||id>=8192)return BADARG;if(op==1)return make_surface(d,id);IosfcSurface*s=d->slot[id];if(op==2){if(!s)return 0;uint32_t word;if(!dma(d,s->descriptor,&word,4,0))return NOMEM;word&=~2u;if(!dma(d,s->descriptor,&word,4,1))return NOMEM;release(s);d->slot[id]=NULL;d->unmaps++;return 0;}if(op==3){if(!s)return BADARG;unsigned total=s->bytes&~7u;for(unsigned off=0;off<total;){unsigned n=total-off;if(n>PAGE)n=PAGE;for(unsigned j=0;j<n;j+=8){uint64_t value=(uint64_t)off+j;memcpy(d->scratch+j,&value,8);}if(!copy_surface(d,s,off,d->scratch,n,1))return NOMEM;off+=n;}return 0;}return UNSUPPORTED;}
static void ring(IosfcDevice*d){
 if(!d->ring_mapped||!d->table)return;
 if(d->wr<d->rd||d->wr-d->rd>d->ring_size){bad(d,0x1018,8);return;}
 while(d->rd<d->wr){uint64_t at=d->ring_base+(d->rd%d->ring_size)*16;uint32_t words[4];if(!dma(d,at,words,16,0)){bad(d,0x1018,8);return;}words[2]=command(d,words[0],words[1]);if(!dma(d,at+8,words+2,4,1)){bad(d,0x1018,8);return;}d->rd++;d->commands++;/* Complete guest status before publishing the consumer and IRQ. */unsigned long long bit=1u<<16;if(uc_arm64_gic_mmio(d->ram->uc,0,0x204,4,1,&bit)){bad(d,0x1018,8);return;}if(words[2]){char s[160];snprintf(s,sizeof(s),"IOSFC command=%u mapping=%u status=0x%x\n",words[0],words[1],words[2]);bridge_output(s);}}
}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){(void)u;IosfcDevice*d=opaque;if((size!=4&&size!=8)||off%8){bad(d,off,size);return 0;}uint64_t v;switch(off){case 0x1000:v=d->ring_base;break;case 0x1008:v=d->ring_size;break;case 0x1010:v=d->table;break;case 0x1018:v=d->wr;break;case 0x1020:v=d->rd;break;case 0x1028:case 0x1030:v=0;break;default:bad(d,off,size);return 0;}return size==4?(uint32_t)v:v;}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){(void)u;IosfcDevice*d=opaque;if((size!=4&&size!=8)||off%8){bad(d,off,size);return;}if(size==4)value=(uint32_t)value;switch(off){case 0x1000:if(value%PAGE){bad(d,off,size);return;}d->ring_base=value;d->ring_mapped=0;return;case 0x1008:if(!value||!d->ring_base)return;if(value>65536||value%1024||sparse_ram_back(d->ram,d->ring_base,value*16)){bad(d,off,size);return;}d->ring_size=value;d->ring_mapped=1;return;case 0x1010:if(value&&(value%PAGE||sparse_ram_back(d->ram,value,PAGE))){bad(d,off,size);return;}iosfc_close(d);d->table=value;return;case 0x1018:d->wr=value;ring(d);return;case 0x1020:if(!d->ring_mapped)d->rd=value;return;case 0x1030:if(value){bad(d,off,size);return;}iosfc_close(d);d->table=0;return;case 0x1028:return;default:bad(d,off,size);return;}}
uc_err iosfc_map(uc_engine*u,IosfcDevice*d,SparseRam*ram){if(!u||!d||!ram||ram->uc!=u)return UC_ERR_ARG;memset(d,0,sizeof(*d));d->ram=ram;bridge_output("IOSFC: mapping at 0x30210000 size 0x10000\n");return uc_mmio_map(u,0x30210000,0x10000,read_reg,d,write_reg,d);}
