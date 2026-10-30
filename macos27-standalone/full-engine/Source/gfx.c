/* SPDX-License-Identifier: MIT
 * Stub for APV_GFX (Apple Platform Virtual Graphics) device.
 * Minimal implementation to allow kernel to probe/init without crashing.
 */
#include "glue.h"
#include "gfx.h"
#include "sparse-ram.h"
void bridge_output(const char*);
static void bad(GfxDevice*d,uint64_t off,unsigned size){char s[150];d->invalid++;snprintf(s,sizeof(s),"Unsupported GFX access: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(s);uc_emu_stop(d->uc);}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){(void)u;GfxDevice*d=opaque;d->reads++;if((size!=4&&size!=8)||off%4){bad(d,off,size);return 0;}if(off>=0x1000&&off<0x2000)return 0;switch(off){case 0:case 8:case 0x10:case 0x18:case 0x20:case 0x28:case 0x30:case 0x38:case 0x40:case 0x48:case 0x50:case 0x58:case 0x60:case 0x68:case 0x70:case 0x78:case 0x80:case 0x88:case 0x90:case 0x98:case 0xa0:case 0xa8:case 0xb0:case 0xb8:case 0xc0:case 0xc8:case 0xd0:case 0xd8:case 0xe0:case 0xe8:case 0xf0:case 0xf8:case 0x100:case 0x108:case 0x110:case 0x118:return 0;default:bad(d,off,size);return 0;}}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){(void)u;GfxDevice*d=opaque;d->writes++;if((size!=4&&size!=8)||off%4){bad(d,off,size);return;}if(off>=0x1000&&off<0x2000)return;switch(off){case 0:case 8:case 0x10:case 0x18:case 0x20:case 0x28:case 0x30:case 0x38:case 0x40:case 0x48:case 0x50:case 0x58:case 0x60:case 0x68:case 0x70:case 0x78:case 0x80:case 0x88:case 0x90:case 0x98:case 0xa0:case 0xa8:case 0xb0:case 0xb8:case 0xc0:case 0xc8:case 0xd0:case 0xd8:case 0xe0:case 0xe8:case 0xf0:case 0xf8:case 0x100:case 0x108:case 0x110:case 0x118:return;default:bad(d,off,size);return;}}
void gfx_close(GfxDevice*d){(void)d;}
uc_err gfx_map(uc_engine*u,GfxDevice*d,SparseRam*ram){if(!u||!d||!ram||ram->uc!=u)return UC_ERR_ARG;memset(d,0,sizeof(*d));d->ram=ram;d->uc=u;bridge_output("GFX: mapping at 0x30200000 size 0x10000\n");return uc_mmio_map(u,0x30200000,0x10000,read_reg,d,write_reg,d);}