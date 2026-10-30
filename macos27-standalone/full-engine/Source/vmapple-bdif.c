/* SPDX-License-Identifier: GPL-2.0-or-later
 * VMApple Backdoor Interface ABI from QEMU hw/vmapple/bdif.c, copyright
 * Amazon.com, Inc. or its affiliates (2023), GPL-2.0-or-later.
 * Bounds-checked synchronous DMA bridge; completion follows backend I/O.
 */
#include "glue.h"
#include "vmapple-bdif.h"
#include <string.h>
void bridge_output(const char*);
typedef struct {uint64_t addr;uint32_t len,flags;} BdifDescriptor;
_Static_assert(sizeof(BdifDescriptor)==16,"BDIF descriptor layout");
static int dma(VmappleBdif*d,uint64_t at,void*data,unsigned bytes,bool write){
 if(!bytes||sparse_ram_back(d->ram,at,bytes))return 0;
 return (write?uc_mem_write(d->ram->uc,at,data,bytes):uc_mem_read(d->ram->uc,at,data,bytes))==UC_ERR_OK;
}
static void command(VmappleBdif*d,uint64_t address){
 BdifDescriptor q[3];uint8_t status=1;uint32_t sector[4];
 if(!dma(d,address,q,sizeof(q),false)){d->failed++;return;}
 if(q[2].len!=1||q[2].flags!=0x20001||q[0].len!=16||q[0].flags!=0x10001)goto done;
 /* Reject an unreportable completion before issuing any backend I/O. */
 if(sparse_ram_back(d->ram,q[2].addr,1))goto done;
 if(!dma(d,q[0].addr,sector,sizeof(sector),false))goto done;
 bool write=q[1].flags==0x10001;
 if(!write&&q[1].flags!=0x30001)goto done;
 uint64_t off=(uint64_t)sector[2]*512;
 unsigned length=q[1].len;
 if(!length||length>128*1024*1024u||off>d->bytes||length>d->bytes-off)goto done;
 /* Validate the full DMA span before a backend write can have side effects. */
 if(sparse_ram_back(d->ram,q[1].addr,length))goto done;
 for(unsigned at=0;at<length;){unsigned n=length-at;if(n>sizeof(d->buffer))n=sizeof(d->buffer);
  if(write&&!dma(d,q[1].addr+at,d->buffer,n,false))goto done;
  if(!d->transfer(d->opaque,off+at,d->buffer,n,write))goto done;
  if(!write&&!dma(d,q[1].addr+at,d->buffer,n,true))goto done;
  at+=n;
 }
 status=0;if(write)d->writes++;else d->reads++;
done:
 if(status)d->failed++;
 /* An invalid completion address cannot target monitor memory or MMIO. */
 if(q[2].len==1&&q[2].flags==0x20001)dma(d,q[2].addr,&status,1,true);
}
static void bad(uc_engine*u,VmappleBdif*d,uint64_t off,unsigned size){
 d->bad_access=1;char line[140];snprintf(line,sizeof(line),"Unsupported BDIF access: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(line);uc_emu_stop(u);
}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){
 VmappleBdif*d=opaque;
 if((off&0xffff0000)!=0x10000||(size!=4&&size!=8)||off%size){bad(u,d,off,size);return 0;}
 switch(off&0xffff){case 0:return 1;case 4:return 2;case 8:return 0x420;case 0x10:return 1;case 0x400:return 1;case 0x420:return 0x10000;case 0x434:return 0;default:bad(u,d,off,size);return 0;}
}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){
 VmappleBdif*d=opaque;
 if((off&0xffff0000)!=0x10000||(size!=4&&size!=8)||off%size){bad(u,d,off,size);return;}
 switch(off&0xffff){case 0x408:if(size!=8){bad(u,d,off,size);return;}command(d,value);return;case 0:case 4:case 8:case 0x10:case 0x400:case 0x434:return;default:bad(u,d,off,size);}
}
uc_err vmapple_bdif_map(uc_engine*u,VmappleBdif*d,SparseRam*ram,uint64_t bytes,BdifTransfer transfer,void*opaque){
 if(!u||!d||!ram||ram->uc!=u||!bytes||bytes%512||!transfer)return UC_ERR_ARG;
 memset(d,0,sizeof(*d));d->ram=ram;d->bytes=bytes;d->transfer=transfer;d->opaque=opaque;
 return uc_mmio_map(u,0x30000000,0x200000,read_reg,d,write_reg,d);
}
