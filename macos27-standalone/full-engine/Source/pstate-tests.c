/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <unicorn/unicorn.h>
#include "pstate-probe.h"
void bridge_output(const char*);
static void check(int x){if(!x){bridge_output("FAIL DIT state\n");abort();}}
static uint64_t reg(uc_engine*u,int id){uint64_t v=0;check(!uc_reg_read(u,id,&v));return v;}
int run_pstate_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));
 check(!uc_mem_map(u,0x1000,0x8000,UC_PROT_ALL));check(!uc_mem_write(u,0x1000,pstate_code,sizeof(pstate_code)));
 uc_arm64_cp_reg cp={.op0=3,.op1=0,.crn=12,.crm=0,.op2=0,.val=0x8000};check(!uc_reg_write(u,UC_ARM64_REG_CP_REG,&cp));
 cp.op1=6;cp.crn=1;cp.crm=1;cp.val=0x501;check(!uc_reg_write(u,UC_ARM64_REG_CP_REG,&cp));
 cp.op1=4;cp.val=1ULL<<31;check(!uc_reg_write(u,UC_ARM64_REG_CP_REG,&cp));
 uint32_t ps=0x3c0;check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&ps));check(!uc_arm64_monitor_exceptions(u,1));
 check(!uc_emu_start(u,0x1000,0x1028,0,100));
 check(reg(u,UC_ARM64_REG_X1)==0x1000000&&reg(u,UC_ARM64_REG_X2)==0&&reg(u,UC_ARM64_REG_X3)==0x1000000&&reg(u,UC_ARM64_REG_X4)==0);
 check(reg(u,UC_ARM64_REG_X12)==0x10003c0&&reg(u,UC_ARM64_REG_X13)==0x1000000&&reg(u,UC_ARM64_REG_PSTATE)==0x3c0);
 check(!uc_close(u));bridge_output("PASS DIT: register and immediate writes, EL0 access, exception inheritance and ERET restoration\n");return 1;
}
