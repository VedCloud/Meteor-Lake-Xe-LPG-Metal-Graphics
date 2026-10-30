/* SPDX-License-Identifier: MIT
 * ARM PrimeCell PL031 register contract. Cross-checked against QEMU
 * hw/rtc/pl031.c (CodeSourcery, 2007), retained separately as reference.
 * A guest LR write adjusts only this virtual clock, never host firmware time.
 */
#include "glue.h"
#include "pl031-rtc.h"
void bridge_output(const char*);
static void bad(Pl031Rtc*d,uint64_t off,unsigned size){char line[128];d->invalid++;snprintf(line,sizeof(line),"Unsupported PL031 register: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(line);uc_emu_stop(d->uc);}
static uint32_t count(Pl031Rtc*d){uint64_t now=d->clock(d->opaque);return d->count+(uint32_t)(now>=d->anchor?(now-d->anchor)/1000000:0);}
static void irq(Pl031Rtc*d){if(uc_arm64_gic_level(d->uc,34,(d->mask&d->status&1)!=0))bad(d,0x10,4);}
void pl031_rtc_poll(Pl031Rtc*d){if(!d||!d->uc)return;if(d->due!=UINT64_MAX&&d->clock(d->opaque)>=d->due){d->due=UINT64_MAX;d->status=1;irq(d);}}
static void alarm(Pl031Rtc*d){uint32_t ticks=d->match-count(d);uint64_t now=d->clock(d->opaque);/* uint32 wrap is part of the PL031 match contract. */d->due=now+(uint64_t)ticks*1000000;pl031_rtc_poll(d);}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){(void)u;Pl031Rtc*d=opaque;d->reads++;if(size!=4||off%4){bad(d,off,size);return 0;}pl031_rtc_poll(d);switch(off){case 0:return count(d);case 4:return d->match;case 8:return d->load;case 0xc:return 1;case 0x10:return d->mask;case 0x14:return d->status;case 0x18:return d->status&d->mask;default:if(off>=0xfe0&&off<=0xffc){static const uint8_t id[]={0x31,0x10,0x14,0,0x0d,0xf0,5,0xb1};return id[(off-0xfe0)/4];}bad(d,off,size);return 0;}}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){(void)u;Pl031Rtc*d=opaque;if(size!=4||off%4){bad(d,off,size);return;}pl031_rtc_poll(d);switch(off){case 4:d->match=value;alarm(d);return;case 8:d->load=d->count=value;d->anchor=d->clock(d->opaque);alarm(d);return;case 0xc:return;/* PL031 is permanently enabled. */case 0x10:d->mask=value&1;irq(d);return;case 0x1c:d->status&=~value;irq(d);return;default:bad(d,off,size);return;}}
uc_err pl031_rtc_map(uc_engine*u,Pl031Rtc*d,RtcClock clock,void*opaque){if(!u||!d||!clock)return UC_ERR_ARG;memset(d,0,sizeof(*d));d->uc=u;d->clock=clock;d->opaque=opaque;d->anchor=clock(opaque);d->count=d->anchor/1000000;d->due=UINT64_MAX;return uc_mmio_map(u,0x20050000,4096,read_reg,d,write_reg,d);}
