/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "pl031-rtc.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL PL031 RTC\n");abort();}}
static uint64_t fake(void*p){return *(uint64_t*)p;}
int run_pl031_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));check(!uc_arm64_gic_init(u));uint64_t now=10000000,base=0x20050000,v;Pl031Rtc d;check(!pl031_rtc_map(u,&d,fake,&now));
 /* ldr w1,[x0]; str w2,[x0,#8]; str w3,[x0,#4]; str w4,[x0,#16]; ldr w5,[x0,#24]; str w6,[x0,#28] */
 const uint32_t code[]={0xb9400001,0xb9000802,0xb9000403,0xb9001004,0xb9401805,0xb9001c06};check(!uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t load=20,match=22,one=1;
 check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X2,&load));check(!uc_reg_write(u,UC_ARM64_REG_X3,&match));check(!uc_reg_write(u,UC_ARM64_REG_X4,&one));check(!uc_reg_write(u,UC_ARM64_REG_X6,&one));
 check(!uc_emu_start(u,0x1000,0x1010,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==10);check(d.load==20&&d.match==22&&d.mask==1&&!d.status);
 now+=2000000;pl031_rtc_poll(&d);check(d.status==1);check(!uc_emu_start(u,0x1010,0x1018,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X5,&v)&&v==1);check(!d.status);
 check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==22);
 load=UINT32_MAX-1;match=1;check(!uc_reg_write(u,UC_ARM64_REG_X2,&load));check(!uc_reg_write(u,UC_ARM64_REG_X3,&match));check(!uc_emu_start(u,0x1004,0x1010,0,20));now+=3000000;pl031_rtc_poll(&d);check(d.status==1);check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==1);
 base+=0xfe0;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==0x31);check(!d.invalid);check(!uc_close(u));bridge_output("PASS PL031: ARM seconds/calendar, guest-only load, match/IRQ acknowledgement, wraparound and identity\n");return 1;
}
