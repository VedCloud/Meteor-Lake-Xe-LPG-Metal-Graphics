/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <unicorn/unicorn.h>
#include "exception-probe.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL guest exception delivery\n");abort();}}
static void cp(uc_engine*u,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t value){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=value};check(uc_reg_write(u,UC_ARM64_REG_CP_REG,&r)==0);}
static uint64_t reg(uc_engine*u,int id){uint64_t value=0;check(uc_reg_read(u,id,&value)==0);return value;}
int run_exception_tests(void){uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);check(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==0);check(uc_mem_map(u,0x1000,0x8000,UC_PROT_ALL)==0);check(uc_mem_write(u,0x1000,exception_code,sizeof(exception_code))==0);cp(u,6,1,1,0,0x501);cp(u,4,1,1,0,1ULL<<31);cp(u,0,12,0,0,0x8000);uint32_t p=0x3c0;check(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p)==0);
 check(uc_emu_start(u,0x1000,0x1008,0,100)==UC_ERR_EXCEPTION);check(uc_arm64_monitor_exceptions(u,1)==0);
 check(uc_emu_start(u,0x1000,0x1008,0,100)==0);check(reg(u,UC_ARM64_REG_X11)==0x55 && reg(u,UC_ARM64_REG_X12)==0x1004 && reg(u,UC_ARM64_REG_X13)==0x3c0 && reg(u,UC_ARM64_REG_X14)==0x56000042 && reg(u,UC_ARM64_REG_X15)==0x77 && (reg(u,UC_ARM64_REG_PSTATE)&15)==0);
 check(uc_emu_start(u,0x1100,0x1108,0,100)==0);check(reg(u,UC_ARM64_REG_X12)==0x1100 && (reg(u,UC_ARM64_REG_X14)>>26)==0 && reg(u,UC_ARM64_REG_X15)==0x88 && (reg(u,UC_ARM64_REG_PSTATE)&15)==0);check(uc_close(u)==0);bridge_output("PASS guest exceptions: default stop, EL0 SVC/UDF vectors, ESR/ELR/SPSR and ERET\n");return 1;}
