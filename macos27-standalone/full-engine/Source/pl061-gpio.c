/* SPDX-License-Identifier: MIT
 * PrimeCell PL061 GPIO model. Register behavior cross-checked with QEMU
 * hw/gpio/pl061.c; standard (not Luminary) identification and 8 input pins.
 * Inputs default pulled down; physical Dell GPIO is never accessed.
 */
#include "glue.h"
#include "pl061-gpio.h"
void bridge_output(const char*);
static uint8_t data(Pl061Gpio*d){return(d->output&d->dir)|(d->input&~d->dir);}
static uint8_t status(Pl061Gpio*d){return d->edge|((uint8_t)~(data(d)^d->event)&d->sense&~d->dir);}
static void bad(Pl061Gpio*d,uint64_t off,unsigned size){char s[128];d->invalid++;snprintf(s,sizeof(s),"Unsupported PL061 access: offset=0x%llx size=%u\n",(unsigned long long)off,size);bridge_output(s);uc_emu_stop(d->uc);}
static void irq(Pl061Gpio*d){if(uc_arm64_gic_level(d->uc,37,(status(d)&d->mask)!=0))bad(d,0x410,1);}
uc_err pl061_gpio_input(Pl061Gpio*d,unsigned pin,unsigned level){if(!d||!d->uc||pin>=8||level>1)return UC_ERR_ARG;uint8_t old=data(d),bit=1u<<pin;d->input=(d->input&~bit)|(level?bit:0);uint8_t next=data(d),changed=(old^next)&~d->dir&~d->sense;d->edge|=changed&(d->both|~(next^d->event));irq(d);return UC_ERR_OK;}
static int valid(Pl061Gpio*d,uint64_t off,unsigned size){d->accesses++;if((size!=1&&size!=2&&size!=4)||off%4||off>=4096){bad(d,off,size);return 0;}return 1;}
static uint64_t read_reg(uc_engine*u,uint64_t off,unsigned size,void*opaque){(void)u;Pl061Gpio*d=opaque;if(!valid(d,off,size))return 0;if(off<0x400)return data(d)&(off>>2);switch(off){case 0x400:return d->dir;case 0x404:return d->sense;case 0x408:return d->both;case 0x40c:return d->event;case 0x410:return d->mask;case 0x414:return status(d);case 0x418:return status(d)&d->mask;case 0x420:return d->alternate;default:if(off>=0xfd0&&off<=0xffc){static const uint8_t id[]={0,0,0,0,0x61,0x10,4,0,0x0d,0xf0,5,0xb1};return id[(off-0xfd0)/4];}bad(d,off,size);return 0;}}
static void write_reg(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){(void)u;Pl061Gpio*d=opaque;if(!valid(d,off,size))return;if(off<0x400){uint8_t bits=(off>>2)&d->dir;d->output=(d->output&~bits)|(value&bits);}else switch(off){case 0x400:d->dir=value;break;case 0x404:d->sense=value;break;case 0x408:d->both=value;break;case 0x40c:d->event=value;break;case 0x410:d->mask=value;break;case 0x41c:d->edge&=~value;break;case 0x420:d->alternate=value;break;default:bad(d,off,size);return;}irq(d);}
uc_err pl061_gpio_map(uc_engine*u,Pl061Gpio*d){if(!u||!d)return UC_ERR_ARG;memset(d,0,sizeof(*d));d->uc=u;return uc_mmio_map(u,0x20060000,4096,read_reg,d,write_reg,d);}
