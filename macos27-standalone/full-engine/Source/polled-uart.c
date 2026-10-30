/* SPDX-License-Identifier: GPL-2.0-or-later
 * Polled Samsung UART transport. Register semantics follow QEMU's
 * hw/char/exynos4210_uart.c (GPL-2.0-or-later). Transmit completes when the
 * monitor sink accepts the byte. Interrupt state is recorded, but no interrupt
 * controller is claimed or signalled by this polled device.
 */
#include "polled-uart.h"
#include <string.h>
static void update(PolledUart*s){s->reg[4]=6|(s->count?1:0);s->reg[6]=s->count==16?0x100:s->count;s->reg[12]=s->reg[13]&~s->reg[14];}
int polled_uart_receive(PolledUart*s,unsigned char c){if(s->count==16){s->reg[5]|=1;s->reg[13]|=2;update(s);return 0;}s->rx[(s->head+s->count)%16]=c;s->count++;s->reg[13]|=1;update(s);return 1;}
static int valid(uc_engine*u,PolledUart*s,uint64_t off,unsigned size){if(off>=0x40||off%4||(size!=4&&!(off==0x20||off==0x24))||(size!=1&&size!=4)){s->bad_access=1;uc_emu_stop(u);return 0;}return 1;}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){PolledUart*s=opaque;if(!valid(u,s,off,size))return 0;update(s);uint32_t value=s->reg[off/4];if(off==0x14)s->reg[5]=0;if(off==0x24){if(s->count){value=s->rx[s->head];s->head=(s->head+1)%16;s->count--;}else value=0;update(s);}return value;}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){PolledUart*s=opaque;if(!valid(u,s,off,size))return;switch(off){
 case 0x08:s->reg[2]=value&~6u;if(value&2){s->head=s->count=0;s->reg[13]&=~1u;}break;
 case 0x10:if(value&8)s->reg[4]&=~8u;break;
 case 0x14:case 0x18:case 0x1c:case 0x24:break;
 case 0x20:if(s->sink)s->sink(s->opaque,(unsigned char)value);s->transmitted++;s->reg[13]|=4;break;
 case 0x30:s->reg[12]&=~value;s->reg[13]&=~value;break;
 case 0x34:s->reg[13]&=~value;break;
 default:s->reg[off/4]=value;break;
 }update(s);}
uc_err polled_uart_map(uc_engine*u,PolledUart*s,uint64_t base,UartSink sink,void*opaque){if(!s||!sink)return UC_ERR_ARG;memset(s,0,sizeof(*s));s->reg[1]=0x3000;s->reg[4]=6;s->reg[8]=0x5c5c5c5c;s->sink=sink;s->opaque=opaque;return uc_mmio_map(u,base,0x4000,read_reg,s,write_reg,s);}
