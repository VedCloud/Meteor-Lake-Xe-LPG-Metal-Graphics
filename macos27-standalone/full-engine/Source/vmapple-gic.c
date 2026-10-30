/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "vmapple-gic.h"
void bridge_output(const char*);
static void fail(uc_engine*u,GicRegion*r,uint64_t off,unsigned size){r->owner->bad_access=1;char s[150];snprintf(s,sizeof(s),"Unsupported GIC access: region=%u offset=0x%llx size=%u\n",r->region,(unsigned long long)off,size);bridge_output(s);uc_emu_stop(u);}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){GicRegion*r=opaque;unsigned long long value=0;r->owner->accesses++;if(uc_arm64_gic_mmio(u,r->region,off,size,0,&value))fail(u,r,off,size);return value;}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t v,void*opaque){GicRegion*r=opaque;unsigned long long value=v;r->owner->accesses++;if(uc_arm64_gic_mmio(u,r->region,off,size,1,&value))fail(u,r,off,size);}
/* VMApple GICv2m frame: 128 SPIs, absolute IDs 160..287. The Apple
 * platform ABI uses MSI_TYPER bit 31 (not a standard SBSA capability bit).
 * Routing writes latch GIC pending state; a transient high/low level pair
 * would lose an edge in this monitor and must not be used for MSI. */
static uint64_t msi_read(uc_engine*u,uint64_t off,unsigned size,void*opaque){
 GicRegion*r=opaque;r->owner->accesses++;
 if(size!=4||off%4){fail(u,r,off,size);return 0;}
 if(off==8)return 0x80000000u|(160u<<16)|128;
 if(off==0xfcc||(off>=0xfd0&&off<=0xffc))return 0;/* no assigned implementor */
 fail(u,r,off,size);return 0;
}
static void msi_write(uc_engine*u,uint64_t off,unsigned size,uint64_t v,void*opaque){
 GicRegion*r=opaque;r->owner->accesses++;
 if(off!=0x40||(size!=2&&size!=4)){fail(u,r,off,size);return;}
 unsigned id=v&0x3ff;
 if(id<160||id>=288){fail(u,r,off,size);return;}
 unsigned long long pending=1u<<(id%32);
 if(uc_arm64_gic_mmio(u,0,0x200+(id/32)*4,4,1,&pending))fail(u,r,off,size);
}
uc_err vmapple_gic_map(uc_engine*u,VmappleGic*g){memset(g,0,sizeof(*g));uc_err e=uc_arm64_gic_init(u);if(e)return e;for(unsigned i=0;i<3;i++){g->context[i].owner=g;g->context[i].region=i;e=uc_mmio_map(u,i==2?0x1fff0000:i?0x10010000:0x10000000,i==1?0x400000:0x10000,i==2?msi_read:read_reg,&g->context[i],i==2?msi_write:write_reg,&g->context[i]);if(e)return e;}return 0;}
