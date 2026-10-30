/* SPDX-License-Identifier: GPL-2.0-or-later
 * VMApple configuration layout: QEMU hw/vmapple/cfg.c, copyright Amazon.com,
 * Inc. or its affiliates (2023). This is writable guest configuration RAM,
 * including scratch/reserved bytes, not fabricated device-operation success.
 */
#include "glue.h"
#include <unicorn/unicorn.h>
#include <stddef.h>
#include "vmapple-config.h"
typedef struct {
 uint32_t version,cpus,unk1,unk2,unk3,unk4;
 uint64_t ecid,ram_size;
 uint32_t installer1,unk5,unk6,installer2,rnd,unk7;
 uint8_t macs[32],reserved[0xa0];
 uint32_t cpu_ids[0x80];uint8_t scratch[0x200];
 char serial[32],region[32],model[32];uint8_t reserved2[32];
 uint32_t unk10;char soc_name[32];
 /* Guest seed layout cross-checked against Orchard's VMApple configuration
  * (revision c73a2bda). Runtime entropy comes independently from EFI RNG. */
 uint8_t seed_padding[0x24],guest_seed[64];
} Config;
_Static_assert(offsetof(Config,cpu_ids)==0x100,"VMApple CPU table");
_Static_assert(offsetof(Config,serial)==0x500,"VMApple serial field");
_Static_assert(offsetof(Config,model)==0x540,"VMApple model field");
_Static_assert(offsetof(Config,soc_name)==0x584,"VMApple CPU name field");
_Static_assert(offsetof(Config,guest_seed)==0x5c8,"VMApple guest seed offset");
_Static_assert(sizeof(Config)==0x608,"VMApple configuration including guest seed");
uc_err vmapple_config_seed(uc_engine*u,const unsigned char*seed,unsigned bytes){
 if(!u||!seed||bytes!=64)return UC_ERR_ARG;
 unsigned nonzero=0;for(unsigned i=0;i<bytes;i++)nonzero|=seed[i];
 if(!nonzero)return UC_ERR_ARG;
 return uc_mem_write(u,0x400000+offsetof(Config,guest_seed),seed,bytes);
}
uc_err vmapple_config_map(uc_engine*u,uint64_t ram,unsigned cpus,const char*brand){
 if(!u||!ram||cpus!=1||!brand)return UC_ERR_ARG;
 Config c;memset(&c,0,sizeof(c));c.version=2;c.cpus=cpus;c.ram_size=ram;
 c.unk1=1;c.unk2=1;c.unk3=0x20;c.unk5=1;c.unk6=1;c.unk10=1;
 strcpy(c.region,"D/A");strcpy(c.model,"VirtualMac2,1");
 /* No physical Apple serial, ECID or network interfaces are invented. */
 unsigned n=0;while(brand[n]&&n<sizeof(c.soc_name)-1){c.soc_name[n]=brand[n];n++;}
 uc_err e=uc_mem_map(u,0x400000,0x10000,UC_PROT_READ|UC_PROT_WRITE);if(e)return e;
 return uc_mem_write(u,0x400000,&c,sizeof(c));
}
void vmapple_host_cpu_brand(char out[49]){
 uint32_t a,b,c,d;memset(out,0,49);
 __asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0x80000000u),"c"(0));
 if(a<0x80000004u){strcpy(out,"x86-64 translation host");return;}
 for(unsigned i=0;i<3;i++){
  __asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0x80000002u+i),"c"(0));
  memcpy(out+i*16,&a,4);memcpy(out+i*16+4,&b,4);memcpy(out+i*16+8,&c,4);memcpy(out+i*16+12,&d,4);
 }
}

/* Darwin 27 AppleVirtualPlatformARMPE::readDMAConfigRegister uses a
 * separate doorbell at 0x4ffff8. This is NOT configuration RAM. The caller
 * initializes version=1, operation=1, completed=0 and returns no OSData when
 * completed stays zero. No optional extended records are provisioned yet.
 * A rejected query must not be answered with fabricated record contents.
 */
typedef struct {uint16_t version;uint8_t operation,completed;uint32_t selector;uint64_t bytes,address;} ConfigRequest;
_Static_assert(sizeof(ConfigRequest)==24,"configuration DMA request");
void bridge_output(const char*);
static void config_bad(uc_engine*u,VmappleConfigDma*d){d->invalid++;bridge_output("Unsupported configuration DMA access\n");uc_emu_stop(u);}
static uint64_t config_read(uc_engine*u,uint64_t off,unsigned size,void*o){config_bad(u,o);return 0;}
static void config_write(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*o){
 VmappleConfigDma*d=o;ConfigRequest q;
 if(off!=0x3ff8||size!=8||value%8||sparse_ram_back(d->ram,value,sizeof(q))||uc_mem_read(u,value,&q,sizeof(q))){config_bad(u,d);return;}
 if(q.version!=1||q.operation!=1||q.completed){config_bad(u,d);return;}
 d->unavailable++;
 if(d->unavailable<=32){char line[180];snprintf(line,sizeof(line),"VMAPPLE extended config unavailable: selector=0x%x bytes=%llu destination=0x%llx\n",q.selector,(unsigned long long)q.bytes,(unsigned long long)q.address);bridge_output(line);}
 /* completed=0 is the driver's unavailable/failed-query path. No payload
  * DMA takes place and every caller-owned byte remains unchanged. */
}
uc_err vmapple_config_dma_map(uc_engine*u,VmappleConfigDma*d,SparseRam*ram){
 if(!u||!d||!ram||ram->uc!=u)return UC_ERR_ARG;
 memset(d,0,sizeof(*d));d->ram=ram;
 return uc_mmio_map(u,0x4fc000,0x4000,config_read,d,config_write,d);
}
