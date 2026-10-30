/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "vmapple-rtc.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL virtual RTC\n");abort();}}
static uint64_t fake(void*p){return *(uint64_t*)p;}
int run_rtc_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));check(!uc_arm64_gic_init(u));
 uint64_t now=1000000;VmappleRtc d;check(!vmapple_rtc_map(u,&d,fake,&now));
 const uint32_t code[]={0xf9400001,0xf9000c02,0xf9400003,0xf9001004,0xf9401805,0xf9001c06,0xf9401807};
 check(!uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t base=0x30240000,zero=0,mask=6,ack=4,v;
 check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X2,&zero));check(!uc_reg_write(u,UC_ARM64_REG_X4,&mask));check(!uc_reg_write(u,UC_ARM64_REG_X6,&ack));
 now+=250;check(!vmapple_rtc_notify(&d,6));check(!uc_emu_start(u,0x1000,0x101c,0,30));
 check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==1000250);check(!uc_reg_read(u,UC_ARM64_REG_X3,&v)&&v==1000250);check(!uc_reg_read(u,UC_ARM64_REG_X5,&v)&&v==6);check(!uc_reg_read(u,UC_ARM64_REG_X7,&v)&&v==2);
 now+=500;check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==1000250);
 uint64_t one=1;check(!uc_reg_write(u,UC_ARM64_REG_X2,&one));check(!uc_emu_start(u,0x1004,0x1008,0,10));now+=100;check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==1000350);
 check(vmapple_rtc_notify(&d,1)==UC_ERR_ARG);base+=8;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(d.invalid==1);check(!uc_close(u));
 bridge_output("PASS RTC: ARM microsecond clock, freeze/resume, notification mask/status/W1C and strict register bounds\n");return 1;
}
