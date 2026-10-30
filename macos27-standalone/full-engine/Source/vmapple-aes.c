/* SPDX-License-Identifier: GPL-2.0-or-later
 * Derived ABI: QEMU hw/vmapple/aes.c, copyright Amazon.com, Inc. or affiliates,
 * 2023. AES primitives retain QEMU's existing OpenSSL-derived implementation.
 * Only guest-supplied keys are supported. UID/GID slots are disabled, not faked.
 */
#include "glue.h"
#include "vmapple-aes.h"
#include "../framebuffer-engine/qemu/include/crypto/aes.h"
void bridge_output(const char*);
static void irq(VmappleAes*s){s->irq_asserted=!!(s->irq_status&s->irq_enable);uc_arm64_gic_level(s->ram->uc,50,s->irq_asserted);}
static void reject(VmappleAes*s){s->irq_status|=4;s->rejected++;s->count=0;irq(s);}
static int span(VmappleAes*s,uint64_t p,unsigned n){return n&&sparse_ram_back(s->ram,p,n)==0;}
static int data(VmappleAes*s){
 uint32_t c=s->fifo[0];unsigned kc=(c>>27)&1,ic=(c>>25)&3,n=c&0xffffff;GuestAesKey*k=&s->key[kc];
 uint64_t src=s->fifo[2]|(((uint64_t)s->fifo[1]<<16)&0xffff00000000ULL);
 uint64_t dst=s->fifo[3]|(((uint64_t)s->fifo[1]<<32)&0xffff00000000ULL);
 if(!n||(n&15)||!k->bytes||!span(s,src,n)||!span(s,dst,n))return 0;
 AES_KEY key;int e=k->encrypt?AES_set_encrypt_key(k->key,k->bytes*8,&key):AES_set_decrypt_key(k->key,k->bytes*8,&key);if(e)return 0;
 /* Chunking supports in-place buffers. Reject shifted overlap, whose DMA
  * ordering would otherwise overwrite bytes still required as input. */
 if(src!=dst&&src<dst+n&&dst<src+n){memset(&key,0,sizeof(key));return 0;}
 int success=1;
 for(unsigned at=0;at<n;){unsigned take=n-at;if(take>sizeof(s->buffer))take=sizeof(s->buffer);
  if(uc_mem_read(s->ram->uc,src+at,s->buffer,take)){success=0;break;}
  for(unsigned j=0;j<take;j+=16){unsigned char block[16],old[16];memcpy(old,s->buffer+j,16);
   if(k->encrypt){if(k->mode)for(unsigned b=0;b<16;b++)old[b]^=s->iv[ic][b];AES_encrypt(old,block,&key);if(k->mode)memcpy(s->iv[ic],block,16);}
   else {AES_decrypt(old,block,&key);if(k->mode){for(unsigned b=0;b<16;b++)block[b]^=s->iv[ic][b];memcpy(s->iv[ic],old,16);}}
   memcpy(s->buffer+j,block,16);
  }
  if(uc_mem_write(s->ram->uc,dst+at,s->buffer,take)){success=0;break;}at+=take;
 }
 memset(&key,0,sizeof(key));memset(s->buffer,0,sizeof(s->buffer));if(success)s->operations++;return success;
}
static void process(VmappleAes*s){
 while(s->running&&s->count){
 unsigned c=s->fifo[0]>>28,needed=1;uint32_t h=s->fifo[0];
 if(c==1){unsigned selector=(h>>24)&7,k=(h>>27)&1,words=4+2*((h>>22)&3);
  if(selector||words>8||((h>>16)&3)>1){s->key[k].bytes=0;reject(s);return;}needed+=words;
 }else if(c==2)needed=5;else if(c==5)needed=4;else if(c==6)needed=2;
 else if(c!=3&&c!=8){reject(s);return;}
 if(s->count<needed)return;
 switch(c){
 case 1:{unsigned k=(h>>27)&1;s->key[k].bytes=(needed-1)*4;s->key[k].mode=(h>>16)&3;s->key[k].encrypt=(h>>20)&1;memcpy(s->key[k].key,s->fifo+1,s->key[k].bytes);break;}
 case 2:memcpy(s->iv[(h>>26)&3],s->fifo+1,16);break;
 case 3:break; /* All earlier synchronous operations are already complete. */
 case 5:if(!data(s)){reject(s);return;}break;
 case 6:{uint64_t at=s->fifo[1]|(((uint64_t)h<<32)&0xff00000000ULL);if(!span(s,at,16)||uc_mem_write(s->ram->uc,at,s->iv[(h>>26)&3],16)){reject(s);return;}break;}
 case 8:s->flag=h&255;if(h&(1u<<27))s->irq_status|=32;break;
 }
 s->count-=needed;memmove(s->fifo,s->fifo+needed,s->count*sizeof(s->fifo[0]));irq(s);
 }
}
static int access(uc_engine*u,VmappleAes*s,uint64_t off,unsigned size){if(size!=4||off%4){s->bad_access=1;bridge_output("Unsupported AES access width/alignment\n");uc_emu_stop(u);return 0;}return 1;}
static uint64_t read1(uc_engine*u,uint64_t off,unsigned size,void*opaque){VmappleAes*s=opaque;if(!access(u,s,off,size))return 0;switch(off){case 8:return s->running;case 0xc:return s->status;case 0x18:return s->irq_status;case 0x1c:return s->irq_enable;case 0x20:return s->watermark;case 0x24:return (s->count<<8)|(s->count==128?4:0)|(s->count==0?2:0);case 0x30:return s->flag;default:s->bad_access=1;bridge_output("Unsupported AES register read\n");uc_emu_stop(u);return 0;}}
static void write1(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){VmappleAes*s=opaque;if(!access(u,s,off,size))return;switch(off){
/* START/STOP/CLEAR_FIFO are documented by Asahi m1n1's AESControlReg.
 * Execution is synchronous, so STOP observes all prior commands completed.
 * A stopped engine retains queued words until START; CLEAR_FIFO discards them.
 * RESET remains unsupported because its hardware semantics are uncertain. */
case 8:
 if((value&~7ULL)||((value&3)==3))break;
 if(value&2)s->running=0;
 if(value&4){s->count=0;memset(s->fifo,0,sizeof(s->fifo));}
 if(value&1){s->running=1;process(s);}return;
case 0x20:s->watermark=value;return;
case 0x18:s->irq_status&=~value;irq(s);return;case 0x1c:s->irq_enable=value;irq(s);return;case 0x200:if(s->count==128){reject(s);return;}s->fifo[s->count++]=value;process(s);return;default:break;}s->bad_access=1;char msg[120];snprintf(msg,sizeof(msg),"Unsupported AES register write offset=0x%llx width=%u value=0x%llx\n",(unsigned long long)off,size,(unsigned long long)value);bridge_output(msg);uc_emu_stop(u);}
static uint64_t read2(uc_engine*u,uint64_t off,unsigned size,void*opaque){VmappleAes*s=opaque;if(!access(u,s,off,size))return 0;if(off==0)return s->key_disable; /* All four hardware-key handles unavailable. */s->bad_access=1;bridge_output("Unsupported AES key-access register\n");uc_emu_stop(u);return 0;}
static void write2(uc_engine*u,uint64_t off,unsigned size,uint64_t v,void*opaque){VmappleAes*s=opaque;if(!access(u,s,off,size))return;
 /* 26A428 disableHardwareKey() writes one of bits 0..3 to this
  * sticky disable register. A write cannot re-enable unavailable keys. */
 if(off==0&&!(v&~15ULL)){s->key_disable|=v;return;}
 s->bad_access=1;char msg[120];snprintf(msg,sizeof(msg),"Unsupported AES key-access write offset=0x%llx value=0x%llx\n",(unsigned long long)off,(unsigned long long)v);bridge_output(msg);uc_emu_stop(u);}
uc_err vmapple_aes_map(uc_engine*u,VmappleAes*s,SparseRam*ram){if(!u||!s||!ram||ram->uc!=u)return UC_ERR_ARG;memset(s,0,sizeof(*s));s->ram=ram;s->status=0x3f80;s->key_disable=15;uc_err e=uc_mmio_map(u,0x30220000,0x4000,read1,s,write1,s);if(e)return e;return uc_mmio_map(u,0x30230000,0x4000,read2,s,write2,s);}
