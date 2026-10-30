/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "pl061-gpio.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL PL061 GPIO\n");abort();}}
int run_gpio_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));check(!uc_arm64_gic_init(u));Pl061Gpio d;check(!pl061_gpio_map(u,&d));
 /* Byte writes match the observed Apple driver access width. */
 const uint32_t code[]={0x39103001,0x39104001,0x39506002,0x39107001};check(!uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t base=0x20060000,bit=64,v;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X1,&bit));check(!uc_emu_start(u,0x1000,0x1008,0,20));check(d.event==64&&d.mask==64);
 check(!pl061_gpio_input(&d,6,1));check(!uc_emu_start(u,0x1008,0x1010,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X2,&v)&&v==64);check(!d.edge);
 check(!pl061_gpio_input(&d,6,0));check(!d.edge);d.both=64;check(!pl061_gpio_input(&d,6,1));check(!pl061_gpio_input(&d,6,0));check(d.edge==64);d.edge=0;d.sense=64;d.event=64;check(!pl061_gpio_input(&d,6,1));check(!uc_emu_start(u,0x1008,0x1010,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X2,&v)&&v==64);check(!pl061_gpio_input(&d,6,0));check(!uc_emu_start(u,0x1008,0x100c,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X2,&v)&&v==0);
 check(pl061_gpio_input(&d,8,1)==UC_ERR_ARG);check(!d.invalid);check(!uc_close(u));bridge_output("PASS GPIO: ARM byte configuration, rising/both-edge and level status, clear/reassert and input bounds\n");return 1;
}
