/* SPDX-License-Identifier: MIT
 * Register contract recovered from supplied Darwin 27's
 * AppleVirtualPlatformRTC: counter +0, enable +0x18, IRQ mask +0x20,
 * status +0x30, W1C +0x38, DT interrupt 51. No alarm is advertised.
 */
#include "glue.h"
#include "vmapple-rtc.h"
void bridge_output(const char*);
static void bad(VmappleRtc*d,uint64_t off,unsigned size){char line[128];d->invalid++;snprintf(line,sizeof(line),"Unsupported virtual RTC register: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(line);uc_emu_stop(d->uc);}
static uint64_t counter(VmappleRtc*d){uint64_t now=d->clock(d->opaque);return d->counter+((d->control&1)&&now>=d->anchor?now-d->anchor:0);}
static void irq(VmappleRtc*d){if(uc_arm64_gic_level(d->uc,51,(d->mask&d->status&6)!=0))bad(d,0x20,8);}
uc_err vmapple_rtc_notify(VmappleRtc*d,unsigned flags){if(!d||!d->uc||!flags||(flags&~6u))return UC_ERR_ARG;d->status|=flags;irq(d);return UC_ERR_OK;}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){(void)u;VmappleRtc*d=opaque;d->reads++;if(size!=8){bad(d,off,size);return 0;}switch(off){case 0:return counter(d);case 0x18:return d->control;case 0x20:return d->mask;case 0x30:return d->status;default:bad(d,off,size);return 0;}}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){(void)u;VmappleRtc*d=opaque;if(size!=8){bad(d,off,size);return;}switch(off){case 0x18:if(value&~1ULL){bad(d,off,size);return;}d->counter=counter(d);d->anchor=d->clock(d->opaque);d->control=value;return;case 0x20:if(value&~6ULL){bad(d,off,size);return;}d->mask=value;irq(d);return;case 0x38:d->status&=~value;irq(d);return;default:bad(d,off,size);return;}}
uc_err vmapple_rtc_map(uc_engine*u,VmappleRtc*d,RtcClock clock,void*opaque){if(!u||!d||!clock)return UC_ERR_ARG;memset(d,0,sizeof(*d));d->uc=u;d->clock=clock;d->opaque=opaque;d->anchor=d->counter=clock(opaque);d->control=1;return uc_mmio_map(u,0x30240000,0x4000,read_reg,d,write_reg,d);}
