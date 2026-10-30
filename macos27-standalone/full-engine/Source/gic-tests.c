/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "vmapple-gic.h"
#include "gic-probe.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL GICv3 controller/timer\n");abort();}}
static uint64_t mmio(uc_engine*u,unsigned r,uint64_t o,unsigned n,unsigned w,uint64_t v){unsigned long long a=v;check(uc_arm64_gic_mmio(u,r,o,n,w,&a)==0);return a;}
static uint64_t cp(uc_engine*u,unsigned crn,unsigned crm,unsigned op2,unsigned w,uint64_t v){uc_arm64_cp_reg r={.op0=3,.op1=0,.crn=crn,.crm=crm,.op2=op2,.val=v};check((w?uc_reg_write(u,UC_ARM64_REG_CP_REG,&r):uc_reg_read(u,UC_ARM64_REG_CP_REG,&r))==0);return r.val;}
static void setcp(uc_engine*u,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t v){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=v};check(uc_reg_write(u,UC_ARM64_REG_CP_REG,&r)==0);}
static uint64_t reg(uc_engine*u,unsigned id){uint64_t v=0;check(uc_reg_read(u,id,&v)==0);return v;}
int run_gic_tests(void){uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);check(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==0);check(uc_mem_map(u,0x1000,0x8000,UC_PROT_ALL)==0);check(uc_mem_write(u,0x1000,gic_code,sizeof(gic_code))==0);VmappleGic g;check(vmapple_gic_map(u,&g)==0);check(uc_arm64_monitor_counter(u,1000000)==0);
 check(mmio(u,1,8,8,0,0)==16);check(mmio(u,1,0x14,4,0,0)==6);mmio(u,1,0x14,4,1,0);check(mmio(u,1,0x14,4,0,0)==0);
 mmio(u,0,0,4,1,1);mmio(u,1,0x10100,4,1,(1u<<27)|(1u<<28));mmio(u,1,0x1041b,1,1,0x80);mmio(u,1,0x1041c,1,1,0x40);cp(u,12,12,6,1,1);cp(u,4,6,0,1,255);mmio(u,1,0x10200,4,1,(1u<<27)|(1u<<28));check(cp(u,12,8,0,0,0)==28);check(cp(u,12,11,3,0,0)==0x40);cp(u,12,8,1,1,28);cp(u,4,6,0,1,0x60);check(cp(u,12,8,0,0,0)==1023);cp(u,4,6,0,1,255);check(cp(u,12,8,0,0,0)==27);cp(u,12,8,1,1,27);check(cp(u,12,8,0,0,0)==1023);
 setcp(u,6,1,1,0,0x501);setcp(u,4,1,1,0,1ULL<<31);setcp(u,0,12,0,0,0x8000);uint32_t p=0x3c5;check(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p)==0);uint64_t future=UINT64_MAX/2;check(uc_reg_write(u,UC_ARM64_REG_X21,&future)==0);mmio(u,1,0x10200,4,1,1u<<27);check(uc_emu_start(u,0x1000,0x100c,0,100)==0);check(reg(u,UC_ARM64_REG_X19)==27&&reg(u,UC_ARM64_REG_X20)==0x55&&reg(u,UC_ARM64_REG_X22)==0x77);
 p=0x3c5;check(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p)==0);uint64_t zero=0;check(uc_reg_write(u,UC_ARM64_REG_X20,&zero)==0);setcp(u,3,14,3,2,0);setcp(u,3,14,3,1,1);check(uc_emu_start(u,0x1000,0x100c,0,100)==0);check(reg(u,UC_ARM64_REG_X19)==27&&reg(u,UC_ARM64_REG_X20)==0x55);check(!g.bad_access);check(uc_close(u)==0);bridge_output("PASS GICv3: pending/active/priority masks, acknowledge/EOI, FIQ vectors and virtual-timer PPI 27\n");return 1;}

int run_msi_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));VmappleGic g;check(!vmapple_gic_map(u,&g));
 const uint32_t code[]={0xb9400801,0xb9004002};check(!uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t base=0x1fff0000,id=160;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X2,&id));check(!uc_emu_start(u,0x1000,0x1008,0,20));check(reg(u,UC_ARM64_REG_X1)==0x80a00080);
 check(mmio(u,0,0x214,4,0,0)==1);mmio(u,1,0x14,4,1,0);mmio(u,0,0,4,1,1);mmio(u,0,0x114,4,1,1);mmio(u,0,0x120,4,1,1u<<31);cp(u,12,12,6,1,1);cp(u,4,6,0,1,255);
 check(cp(u,12,8,0,0,0)==160);check(!uc_emu_start(u,0x1004,0x1008,0,20));check(cp(u,12,8,0,0,0)==1023);cp(u,12,8,1,1,160);check(cp(u,12,8,0,0,0)==160);cp(u,12,8,1,1,160);
 id=287;check(!uc_reg_write(u,UC_ARM64_REG_X2,&id));check(!uc_emu_start(u,0x1004,0x1008,0,20));check(cp(u,12,8,0,0,0)==287);cp(u,12,8,1,1,287);
 id=159;check(!uc_reg_write(u,UC_ARM64_REG_X2,&id));check(!uc_emu_start(u,0x1004,0x1008,0,20));check(g.bad_access&&cp(u,12,8,0,0,0)==1023);check(!uc_close(u));
 bridge_output("PASS MSI: ARM GICv2m discovery, ID 160/287 delivery, pending during active/EOI and bounds\n");return 1;
}

int run_sgi_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));VmappleGic g;check(!vmapple_gic_map(u,&g));uint32_t pstate=0x3c5;check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&pstate));
 const uint32_t code[]={0xd518cba1,0xd518cbe1,0xd518cbc1};check(!uc_mem_write(u,0x1000,code,sizeof(code)));
 mmio(u,1,0x14,4,1,0);mmio(u,0,0,4,1,7);mmio(u,1,0x10080,4,1,1u<<7);mmio(u,1,0x10100,4,1,(1u<<7)|(1u<<8));cp(u,12,12,6,1,1);cp(u,12,12,7,1,1);cp(u,4,6,0,1,255);
 uint64_t value=(7ULL<<24)|1;check(!uc_reg_write(u,UC_ARM64_REG_X1,&value));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(cp(u,12,12,0,0,0)==7);cp(u,12,12,1,1,7);
 const uint64_t excluded[]={2,1ULL<<40,(1ULL<<16)|1,(1ULL<<32)|1,(1ULL<<48)|1,(1ULL<<44)|1};
 for(unsigned i=0;i<sizeof(excluded)/sizeof(excluded[0]);i++){value=(7ULL<<24)|excluded[i];check(!uc_reg_write(u,UC_ARM64_REG_X1,&value));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(cp(u,12,12,0,0,0)==1023);}
 value=(8ULL<<24)|1;check(!uc_reg_write(u,UC_ARM64_REG_X1,&value));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(cp(u,12,8,0,0,0)==1023);
 check(!uc_emu_start(u,0x1004,0x1008,0,10));check(cp(u,12,8,0,0,0)==8);cp(u,12,8,1,1,8);
 value=(7ULL<<24)|1;check(!uc_reg_write(u,UC_ARM64_REG_X1,&value));check(!uc_emu_start(u,0x1008,0x100c,0,10));check(cp(u,12,12,0,0,0)==1023);check(!g.bad_access);check(!uc_close(u));bridge_output("PASS SGI: ARM SGI1/SGI0 self delivery, group targeting, IRM/affinity/range exclusions and EOI\n");return 1;
}

/* Unlike setcp(), these paths execute the guest MSR handlers. Raw API writes
 * deliberately bypass writefn and cannot validate actual timer programming. */
#include "timer-msr-probe.h"
int run_timer_msr_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));check(!uc_mem_write(u,0x1000,timer_msr_code,sizeof(timer_msr_code)));VmappleGic g;check(!vmapple_gic_map(u,&g));check(!uc_arm64_monitor_counter(u,1000000));
 setcp(u,6,1,1,0,0x501);setcp(u,4,1,1,0,1ULL<<31);uint32_t state=0x3c5;check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&state));
 uint64_t deadline=UINT64_MAX/2;check(!uc_reg_write(u,UC_ARM64_REG_X0,&deadline));check(!uc_emu_start(u,0x1000,0x1008,0,20));
 if(reg(u,UC_ARM64_REG_X1)!=deadline){bridge_output("FAIL guest timer MSR CVAL readback (raw API tests did not cover this path)\n");abort();}
 uint64_t control=3;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check((reg(u,UC_ARM64_REG_X3)&7)==3);
 uint64_t interval=1000000000;check(!uc_reg_write(u,UC_ARM64_REG_X4,&interval));check(!uc_emu_start(u,0x1010,0x1018,0,20));uint64_t remaining=reg(u,UC_ARM64_REG_X5);check(remaining>500000000&&remaining<=interval);
 control=UINT64_MAX;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check((reg(u,UC_ARM64_REG_X3)&~7ULL)==0);
 control=0;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check((reg(u,UC_ARM64_REG_X3)&3)==0);
 /* Keep DAIF masked while checking delivery eligibility without taking it. */
 mmio(u,1,0x14,4,1,0);mmio(u,0,0,4,1,1);mmio(u,1,0x10100,4,1,1u<<27);cp(u,12,12,6,1,1);cp(u,4,6,0,1,255);
 deadline=0;check(!uc_reg_write(u,UC_ARM64_REG_X0,&deadline));check(!uc_emu_start(u,0x1000,0x1008,0,20));control=3;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check(reg(u,UC_ARM64_REG_X3)==7&&cp(u,12,8,2,0,0)==1023);
 control=1;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check(reg(u,UC_ARM64_REG_X3)==5&&cp(u,12,8,2,0,0)==27);
 deadline=UINT64_MAX/2;check(!uc_reg_write(u,UC_ARM64_REG_X0,&deadline));check(!uc_emu_start(u,0x1000,0x1008,0,20));check(cp(u,12,8,2,0,0)==1023);
 interval=UINT64_MAX;check(!uc_reg_write(u,UC_ARM64_REG_X4,&interval));check(!uc_emu_start(u,0x1010,0x1018,0,20));check(cp(u,12,8,2,0,0)==27);
 control=0;check(!uc_reg_write(u,UC_ARM64_REG_X2,&control));check(!uc_emu_start(u,0x1008,0x1010,0,20));check((reg(u,UC_ARM64_REG_X3)&7)==0&&cp(u,12,8,2,0,0)==1023);
 check(!g.bad_access);check(!uc_close(u));bridge_output("PASS guest timer MSR: CVAL/CTL/TVAL, signed deadlines, PPI 27 assert/deassert, masks and disable through translated instructions\n");return 1;
}

#include "timer-preemption-probe.h"
uint64_t bridge_video_now_ns(void);
typedef struct { uint64_t deadline,blocks;int delivered; } TimerPreemption;
static void timer_busy_loop(uc_engine*u,uint64_t pc,uint32_t size,void*opaque){
 (void)size;TimerPreemption*t=opaque;t->blocks++;
 if((t->blocks&1023)==0)check(!uc_arm64_monitor_poll(u,pc));
 if(reg(u,UC_ARM64_REG_X20)==0x55){t->delivered=1;uc_emu_stop(u);}
 else if(bridge_video_now_ns()>=t->deadline)uc_emu_stop(u);
}
int run_timer_preemption_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,0x1000,0x8000,UC_PROT_ALL));check(!uc_mem_write(u,0x1000,gic_code,sizeof(gic_code)));check(!uc_mem_write(u,0x1000,timer_preemption_code,sizeof(timer_preemption_code)));VmappleGic g;check(!vmapple_gic_map(u,&g));check(!uc_arm64_monitor_counter(u,1000000));
 mmio(u,1,0x14,4,1,0);mmio(u,0,0,4,1,1);mmio(u,1,0x10100,4,1,1u<<27);cp(u,12,12,6,1,1);cp(u,4,6,0,1,255);setcp(u,6,1,1,0,0x501);setcp(u,4,1,1,0,1ULL<<31);setcp(u,0,12,0,0,0x8000);uint32_t state=0x3c5;check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&state));
 uint64_t interval=100000,enable=1,future=UINT64_MAX/2;check(!uc_reg_write(u,UC_ARM64_REG_X0,&interval));check(!uc_reg_write(u,UC_ARM64_REG_X1,&enable));check(!uc_reg_write(u,UC_ARM64_REG_X21,&future));TimerPreemption t={.deadline=bridge_video_now_ns()+5000000000ULL};uc_hook hook;check(!uc_hook_add(u,&hook,UC_HOOK_BLOCK,timer_busy_loop,&t,0x100c,0x100c));check(!uc_emu_start(u,0x1000,0,0,0));
 if(!t.delivered){bridge_output("FAIL future virtual timer did not preempt a chained busy loop within 5 seconds\n");abort();}
 check(reg(u,UC_ARM64_REG_X19)==27);check(!uc_close(u));bridge_output("PASS future timer preemption: translated 100ms deadline interrupts a chained busy loop without the internal instruction-count hook\n");return 1;
}
