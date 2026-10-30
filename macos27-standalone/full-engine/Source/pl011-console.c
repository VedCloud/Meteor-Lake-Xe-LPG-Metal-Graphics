/* SPDX-License-Identifier: GPL-2.0-or-later
 * Polled PL011 transport, register layout/reset semantics from QEMU pl011.c.
 * TX goes to the monitor console; no RX input or wired IRQ is claimed yet.
 */
#include "pl011-console.h"
#include <string.h>
static int valid(uc_engine*u,Pl011Console*s,uint64_t off,unsigned size){if(off%4||(size!=1&&size!=2&&size!=4)||!((off<=0x48)||(off>=0xfe0&&off<=0xffc))){s->bad_access=1;uc_emu_stop(u);return 0;}return 1;}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){Pl011Console*s=opaque;if(!valid(u,s,off,size))return 0;if(off>=0xfe0){static const unsigned char id[8]={0x11,0x10,0x14,0,0x0d,0xf0,0x05,0xb1};return id[(off-0xfe0)/4];}switch(off){case 0x18:return 0x90; /* TXFE, RXFE */case 0x40:return s->reg[15]&s->reg[14];case 0x44:return 0;default:return s->reg[off/4];}}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t v,void*opaque){Pl011Console*s=opaque;if(!valid(u,s,off,size))return;if(off>=0xfe0||off==0x18||off==0x3c||off==0x40)return;if(off==0){s->sink(s->opaque,(unsigned char)v);s->transmitted++;s->reg[15]|=0x20;return;}if(off==4){s->reg[1]=0;return;}if(off==0x44){s->reg[15]&=~v;return;}s->reg[off/4]=v;}
uc_err pl011_console_map(uc_engine*u,Pl011Console*s,uint64_t base,UartSink sink,void*opaque){if(!s||!sink)return UC_ERR_ARG;memset(s,0,sizeof(*s));s->reg[12]=0x300;s->reg[13]=0x12;s->sink=sink;s->opaque=opaque;return uc_mmio_map(u,base,0x10000,read_reg,s,write_reg,s);}
