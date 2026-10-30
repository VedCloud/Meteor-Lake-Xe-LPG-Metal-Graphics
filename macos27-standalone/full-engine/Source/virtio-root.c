/* SPDX-License-Identifier: MIT
 * Read-only VMApple VirtIO block device, modern PCI transport / split queue.
 * ABI: OASIS VirtIO 1.2 sections 2.7, 4.1, 5.2 and QEMU hw/vmapple/virtio-blk.c.
 * Only the admitted guest recovery-image span is readable. No host disk I/O.
 */
#include "glue.h"
#include "virtio-root.h"
#include <unicorn/arm64.h>
void bridge_output(const char*);
#define HOST_FEATURES ((1ULL<<32)|(1ULL<<5)|(1ULL<<6)) /* immutable ROOT */
static uint64_t host_features(VirtioRoot*d){return d->transfer?((1ULL<<32)|(1ULL<<6)|(1ULL<<9)):HOST_FEATURES;}
typedef struct {uint64_t addr;uint32_t len;uint16_t flags,next;} Descriptor;
_Static_assert(sizeof(Descriptor)==16,"virtqueue descriptor ABI");
static int mem(VirtioRoot*d,uint64_t a,void*p,unsigned n,bool wr){if(wr&&a<d->immutable_base+d->immutable_bytes&&a+n>d->immutable_base)return 0;return n&&!sparse_ram_back(d->ram,a,n)&&(wr?uc_mem_write(d->ram->uc,a,p,n):uc_mem_read(d->ram->uc,a,p,n))==UC_ERR_OK;}
static uint64_t get(const uint8_t*p,unsigned n){uint64_t x=0;for(unsigned i=0;i<n;i++)x|=(uint64_t)p[i]<<(8*i);return x;}
static void put(uint8_t*p,uint64_t x,unsigned n){for(unsigned i=0;i<n;i++)p[i]=x>>(8*i);}
static void invalid(VirtioRoot*d){d->invalid++;d->status|=64;bridge_output("VirtIO root: invalid transport access\n");uc_emu_stop(d->ram->uc);}
static void interrupt(VirtioRoot*d){
 uint16_t control=get(d->config+0xc2,2);
 if(control&0x8000){if(d->vector>1)return;uint8_t*t=d->msix+16*d->vector;if((control&0x4000)||(get(t+12,4)&1)){d->pending|=1<<d->vector;return;}d->pending&=~(1<<d->vector);
  uint64_t addr=get(t,8),data=get(t+8,4);if(addr!=0x1fff0040||data<160||data>=288){invalid(d);return;}
  unsigned long long bits=1u<<(data%32);if(uc_arm64_gic_mmio(d->ram->uc,0,0x200+data/32*4,4,1,&bits))invalid(d);
 }else {d->isr|=1;uc_arm64_gic_level(d->ram->uc,d->irq,1);}
}
/* Validate a whole chain before modifying guest memory. Unsupported writes
 * complete with IOERR; malformed queues set NEEDS_RESET and stop processing. */
static int request(VirtioRoot*d,uint16_t head,uint32_t*written){
 Descriptor q[256];uint16_t id=head;unsigned count=0;uint8_t seen[256]={0};
 for(;;){if(id>=d->size||seen[id]||count==256)return 0;seen[id]=1;
  if(!mem(d,d->desc+16*id,&q[count],16,false)||q[count].flags&~3)return 0;
  Descriptor*x=&q[count++];if(!(x->flags&1))break;id=x->next;
 }
 if(count<2||q[0].flags&2||q[0].len!=16)return 0;
 Descriptor*status=&q[count-1];if(status->len!=1||status->flags!=2||sparse_ram_back(d->ram,status->addr,1))return 0;
 uint32_t h[4];if(!mem(d,q[0].addr,h,16,false))return 0;uint32_t type=h[0];uint64_t sector=(uint64_t)h[2]|((uint64_t)h[3]<<32),total=0;
 for(unsigned i=1;i+1<count;i++){
  if(q[i].len>128*1024*1024u||total>128*1024*1024u-q[i].len)return 0;total+=q[i].len;
  if(q[i].len&&sparse_ram_back(d->ram,q[i].addr,q[i].len))return 0;
  if(type==0&&!(q[i].flags&2))return 0;
  if(type==1&&(q[i].flags&2))return 0;
 }
 uint8_t result=2;*written=1;
 if(d->transfer&&(type==0||type==1)){
  result=1;
  if(sector<=d->bytes/512&&total<=d->bytes-sector*512){
   uint8_t buffer[4096];uint64_t off=sector*512;bool ok=true;
   for(unsigned i=1;i+1<count&&ok;i++)for(unsigned at=0;at<q[i].len;){
    unsigned n=q[i].len-at;if(n>sizeof(buffer))n=sizeof(buffer);
    if(type==1&&!mem(d,q[i].addr+at,buffer,n,false)){ok=false;break;}
    if(!d->transfer(d->opaque,off,buffer,n,type==1)){ok=false;break;}
    if(type==0&&!mem(d,q[i].addr+at,buffer,n,true)){ok=false;break;}
    off+=n;at+=n;
   }
   if(ok){result=0;if(type==0){*written+=total;d->reads++;}else d->writes++;}
  }
  goto complete;
 }
 if(d->transfer&&(type==4||type==0x10000)&&total==0){result=d->transfer(d->opaque,0,NULL,0,true)?0:1;goto complete;}
 if(type==0){result=1;if(sector<=d->bytes/512&&total<=d->bytes-sector*512){
   uint8_t buffer[4096];uint64_t source=d->image+sector*512;
   for(unsigned i=1;i+1<count;i++)for(unsigned at=0;at<q[i].len;){unsigned n=q[i].len-at;if(n>sizeof(buffer))n=sizeof(buffer);
    if(!mem(d,source,buffer,n,false)||!mem(d,q[i].addr+at,buffer,n,true))return 0;source+=n;at+=n;
   }result=0;*written+=total;d->reads++;
  }
 }else if(type==1)result=1;/* immutable recovery media */
 else if((type==4||type==0x10000)&&total==0)result=0;/* flush/barrier: no dirty backing data */
 else if(type==8){static const uint8_t name[20]="Dell27 recovery";unsigned at=0;result=0;
  for(unsigned i=1;i+1<count&&at<20;i++){unsigned n=q[i].len;if(n>20-at)n=20-at;if(!(q[i].flags&2)||!mem(d,q[i].addr,(void*)(name+at),n,true))return 0;at+=n;}*written+=at;
 }
 complete:
 if(result)d->failed++;return mem(d,status->addr,&result,1,true);
}
static void notify(VirtioRoot*d){
 d->notifications++;if((d->status&64)||!(d->status&4)||!d->enabled||!d->size)return;
 uint16_t idx,flags;if(!mem(d,d->avail+2,&idx,2,false)||!mem(d,d->avail,&flags,2,false))goto bad;
 if((uint16_t)(idx-d->last_avail)>d->size)goto bad;
 /* Validate completion ring up front; no partial completion into MMIO. */
 if(sparse_ram_back(d->ram,d->used,6+8*d->size))goto bad;
 bool completed=false;
 while(d->last_avail!=idx){uint16_t head;uint32_t used[2];
  if(!mem(d,d->avail+4+2*(d->last_avail%d->size),&head,2,false))goto bad;used[0]=head;
  if(!request(d,head,&used[1]))goto bad;
  if(!mem(d,d->used+4+8*(d->used_idx%d->size),used,8,true))goto bad;
  completed=true;d->last_avail++;d->used_idx++;if(!mem(d,d->used+2,&d->used_idx,2,true))goto bad;
 }
 /* A queue kick can merely re-arm notification; only real used-ring
  * progress may generate a fresh completion interrupt. */
 if(completed&&!(flags&1))interrupt(d);return;
bad:d->failed++;d->status|=64;bridge_output("VirtIO root: malformed queue rejected\n");
}
static uint64_t common(VirtioRoot*d,unsigned o,unsigned n){
 uint8_t c[64]={0};put(c,d->feature_sel,4);put(c+4,d->feature_sel==0?(uint32_t)host_features(d):d->feature_sel==1?host_features(d)>>32:0,4);
 put(c+8,d->driver_sel,4);put(c+12,d->driver_sel<2?d->features>>(32*d->driver_sel):0,4);put(c+16,d->config_vector,2);put(c+18,1,2);c[20]=d->status;
 put(c+22,0,2);put(c+24,d->size,2);put(c+26,d->vector,2);put(c+28,d->enabled,2);put(c+30,0,2);put(c+32,d->desc,8);put(c+40,d->avail,8);put(c+48,d->used,8);
 return get(c+o,n);
}
static uint64_t bar_read(uc_engine*u,uint64_t off,unsigned n,void*p){(void)u;VirtioRoot*d=p;
 for(;d;d=d->next){uint64_t b=d->bar&~0x3fffULL;if(b&&d->bar!=UINT32_MAX&&off+0x50000000>=b&&off+0x50000000+n<=b+0x4000)break;}
 if(!d)return n==8?UINT64_MAX:(1ULL<<(n*8))-1;
 uint64_t base=d->bar&~0x3fffULL;
 if(!base||off+0x50000000<base||off+0x50000000+n>base+0x4000)return n==8?UINT64_MAX:(1ULL<<(n*8))-1;
 unsigned a=off+0x50000000-base;if((n!=1&&n!=2&&n!=4&&n!=8)||a%n){invalid(d);return 0;}
 if(a+n<=56)return common(d,a,n);
 if(a==0x1000&&n==1){uint8_t x=d->isr;d->isr=0;uc_arm64_gic_level(u,d->irq,0);return x;}
 if(a>=0x2000&&a+n<=0x2100){uint8_t cfg[256]={0};put(cfg,d->bytes/512,8);put(cfg+20,512,4);put(cfg+60,d->apple_type,4);/* VMApple ROOT1 / AUX2 */return get(cfg+a-0x2000,n);}
 if(a>=0x3000&&a+n<=0x3020)return get(d->msix+a-0x3000,n);
 if(a>=0x3100&&a+n<=0x3108)return ((uint64_t)d->pending)>>((a-0x3100)*8);
 return 0;
}
static void bar_write(uc_engine*u,uint64_t off,unsigned n,uint64_t v,void*p){(void)u;VirtioRoot*d=p;
 for(;d;d=d->next){uint64_t b=d->bar&~0x3fffULL;if(b&&d->bar!=UINT32_MAX&&off+0x50000000>=b&&off+0x50000000+n<=b+0x4000)break;}
 if(!d)return;
 uint64_t base=d->bar&~0x3fffULL;
 if(!base||off+0x50000000<base||off+0x50000000+n>base+0x4000)return;unsigned a=off+0x50000000-base;
 if((n!=1&&n!=2&&n!=4&&n!=8)||a%n){invalid(d);return;}
 if(a>=0x2000&&a+n<=0x2100)return;/* read-only config fields: native Apple driver writes blk_size back bytewise */
 if(a>=0x3000&&a+n<=0x3020){put(d->msix+a-0x3000,v,n);if(d->vector<2&&(d->pending&(1<<d->vector)))interrupt(d);return;}
 if(a==0x800&&(n==2||n==4)){if(v==0)notify(d);else invalid(d);return;}
 switch(a){
 case 0:if(n==4)d->feature_sel=v;else goto bad;return;
 case 8:if(n==4)d->driver_sel=v;else goto bad;return;
 case 12:if(n!=4||d->driver_sel>1)goto bad;d->features=(d->features&~(0xffffffffULL<<(32*d->driver_sel)))|((v&0xffffffffULL)<<(32*d->driver_sel));return;
 case 16:if(n!=2)goto bad;d->config_vector=v;return;
 case 20:if(n!=1)goto bad;if(!v){d->status=d->enabled=d->isr=d->pending=0;d->last_avail=d->used_idx=0;d->desc=d->avail=d->used=d->features=0;d->size=256;d->vector=d->config_vector=0xffff;uc_arm64_gic_level(u,d->irq,0);return;}d->status=v;if((v&8)&&((d->features&~host_features(d))||!(d->features&(1ULL<<32))))d->status&=~8;return;
 case 22:if(n!=2||v!=0)goto bad;return;
 case 24:if(n!=2||!v||v>256||(v&(v-1))||d->enabled)goto bad;d->size=v;return;
 case 26:if(n!=2)goto bad;d->vector=v;return;
 case 28:if(n!=2||v!=1||d->enabled||!d->desc||!d->avail||!d->used||(d->desc&15)||(d->avail&1)||(d->used&3))goto bad;d->enabled=1;return;
 default:if(a>=32&&a+n<=56){uint64_t*x=a<40?&d->desc:a<48?&d->avail:&d->used;unsigned shift=(a%8)*8;if(d->enabled||shift+n*8>64)goto bad;uint64_t mask=n==8?UINT64_MAX:((1ULL<<(8*n))-1)<<shift;*x=(*x&~mask)|((v<<shift)&mask);return;}
 }
bad:{char line[160];snprintf(line,sizeof(line),"VirtIO root rejected write: offset=0x%x width=%u value=0x%llx\n",a,n,(unsigned long long)v);bridge_output(line);invalid(d);}
}
uint64_t virtio_root_config_read(VirtioRoot*d,unsigned o,unsigned n){if(o+n>256)return n==4?UINT32_MAX:n==2?0xffff:0xff;
 if(o>=0x10&&o+n<=0x14){uint32_t val=d->bar==UINT32_MAX?0xffffc000:(uint32_t)d->bar;return (val>>((o-0x10)*8))&(n==4?UINT32_MAX:(1ULL<<(n*8))-1);}
 return get(d->config+o,n);
}
void virtio_root_config_write(VirtioRoot*d,unsigned o,unsigned n,uint64_t v){
 if(o==0x10&&n==4){d->bar=(uint32_t)v==UINT32_MAX?UINT32_MAX:v&0xffffc000;if(d->bar!=UINT32_MAX&&d->bar&&(d->bar<0x50000000||d->bar>0x6ffec000))invalid(d);return;}
 for(unsigned i=0;i<n&&o+i<256;i++){unsigned a=o+i;uint8_t mask=a==4?7:a==5?4:a==0xc||a==0x3c?255:a==0xc3?0xc0:0;d->config[a]=(d->config[a]&~mask)|((v>>(8*i))&mask);}
 if(d->vector<2&&(d->pending&(1<<d->vector)))interrupt(d);
}
static void cap(VirtioRoot*d,unsigned o,unsigned next,unsigned type,unsigned offset,unsigned length){uint8_t*c=d->config+o;c[0]=9;c[1]=next;c[2]=16;c[3]=type;c[4]=0;put(c+8,offset,4);put(c+12,length,4);}
uc_err virtio_root_map(uc_engine*u,VirtioRoot*d,SparseRam*r,uint64_t image,uint64_t bytes){
 if(!u||!d||!r||r->uc!=u||!bytes||bytes%512||image<r->base||image-r->base>r->size||bytes>r->size-(image-r->base))return UC_ERR_ARG;
 memset(d,0,sizeof(*d));d->ram=r;d->image=image;d->bytes=bytes;d->size=256;d->vector=d->config_vector=0xffff;d->immutable_base=image;d->immutable_bytes=bytes;d->apple_type=1;d->irq=36;
 put(d->config,0x1a00106b,4);d->config[6]=0x10;d->config[8]=1;d->config[0xb]=1;d->config[0xa]=0;d->config[0x34]=0x40;put(d->config+0x2c,0x1100106b,4);d->config[0x3d]=1;
 cap(d,0x40,0x50,1,0,56);cap(d,0x50,0x64,2,0x800,4);d->config[0x52]=20;put(d->config+0x60,4,4);
 cap(d,0x64,0x74,3,0x1000,1);cap(d,0x74,0x84,4,0x2000,256);
 /* Apple vendor cfg_type 9, vendor106b, transport encryption disabled. */
 d->config[0x84]=9;d->config[0x85]=0xc0;d->config[0x86]=14;d->config[0x87]=9;put(d->config+0x88,0x106b,2);put(d->config+0x8a,1,4);
 d->config[0xc0]=0x11;put(d->config+0xc2,1,2);put(d->config+0xc4,0x3000,4);put(d->config+0xc8,0x3100,4);put(d->msix+12,1,4);put(d->msix+28,1,4);
 return uc_mmio_map(u,0x50000000,0x1fff0000,bar_read,d,bar_write,d);
}

/* AUX is backed only by the existing private fixed-size utility file.
 * Root and auxiliary queues, feature negotiation and interrupts are separate.
 * Flush succeeds only after the backend's actual durability operation.
 */
uc_err virtio_aux_attach(VirtioRoot*root,VirtioRoot*aux,uint64_t bytes,VirtioBackingTransfer transfer,void*opaque){
 if(!root||!aux||root==aux||root->next||!root->ram||!bytes||bytes%512||!transfer)return UC_ERR_ARG;
 memset(aux,0,sizeof(*aux));aux->ram=root->ram;aux->bytes=bytes;aux->size=256;aux->vector=aux->config_vector=0xffff;
 memcpy(aux->config,root->config,sizeof(aux->config));
 /* Copy identity/capabilities, never mutable command or interrupt state. */
 put(aux->config+4,0,2);put(aux->config+0xc2,1,2);put(aux->msix+12,1,4);put(aux->msix+28,1,4);
 aux->transfer=transfer;aux->opaque=opaque;aux->apple_type=2;aux->irq=37;
 aux->immutable_base=root->immutable_base;aux->immutable_bytes=root->immutable_bytes;root->next=aux;return UC_ERR_OK;
}
