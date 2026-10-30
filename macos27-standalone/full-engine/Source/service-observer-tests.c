/* SPDX-License-Identifier: MIT — translated, read-only diagnostic fixture. */
#include "glue.h"
#include <unicorn/unicorn.h>
#include <unicorn/arm64.h>
#include "../Standalone/boot_input.h"
void bridge_output(const char*);
static int guest_read(uc_engine*u,uint64_t va,void*out,unsigned n){unsigned long long pa=0;return !uc_arm64_monitor_translate(u,va,0,&pa)&&!uc_mem_read(u,pa,out,n);}
#include "service-entry-observer.h"
#include "service-fixtures.h"
static void check(int ok){if(!ok){bridge_output("FAIL service observer fixture\n");abort();}}
static D27ServiceObserver capture;
static void stop(uc_engine*u,uint64_t pc,uint32_t size,void*o){(void)size;(void)o;
 uint64_t before[34],after;int regs[34];for(int i=0;i<29;i++)regs[i]=UC_ARM64_REG_X0+i;regs[29]=UC_ARM64_REG_X29;regs[30]=UC_ARM64_REG_X30;regs[31]=UC_ARM64_REG_SP;regs[32]=UC_ARM64_REG_PC;regs[33]=UC_ARM64_REG_NZCV;
 for(int i=0;i<34;i++){before[i]=0;check(!uc_reg_read(u,regs[i],&before[i]));}
 observe_service_entry(u,pc,&capture);
 for(int i=0;i<34;i++){after=0;check(!uc_reg_read(u,regs[i],&after));check(after==before[i]);}
 uc_emu_stop(u);
}
static void trial(unsigned point,uint64_t slide,int corrupt,int kernel,int limit,int other_error){
 uc_engine*u=0;uint64_t pc=service_points[point].pc+slide;
 check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));
 check(!uc_mem_map(u,pc&~0x3fffULL,0x8000,UC_PROT_ALL));check(!uc_mem_write(u,pc,service_fixtures[point],96));
 if(corrupt){unsigned char b=service_fixtures[point][95]^1;check(!uc_mem_write(u,pc+95,&b,1));}
 check(!uc_mem_map(u,0x40000000,0x4000,UC_PROT_ALL));uint64_t frame[]={0x40000110,0x1872340b4ULL+slide,0x40000110,0x1872340b4ULL+slide};check(!uc_mem_write(u,0x40000100,frame,sizeof(frame)));
 uint64_t fp=0x40000100,lr=0x1872340b4ULL+slide,value=other_error?0x80010912U:0x80010911U;uint32_t mode=kernel?5:0;
 check(!uc_reg_write(u,UC_ARM64_REG_X29,&fp));check(!uc_reg_write(u,UC_ARM64_REG_X30,&lr));check(!uc_reg_write(u,UC_ARM64_REG_X0,&value));check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&mode));
 uc_arm64_cp_reg tcr={.op0=3,.op1=0,.crn=2,.crm=0,.op2=2,.val=25};check(!uc_reg_write(u,UC_ARM64_REG_CP_REG,&tcr));
 memset(&capture,0,sizeof(capture));if(limit)capture.seen[point]=8;uc_hook hook;check(!uc_hook_add(u,&hook,UC_HOOK_BLOCK,stop,0,1,0));check(!uc_emu_start(u,pc,0,0,0));
 int expected=!corrupt&&!kernel&&!limit&&!(slide&0x3fff)&&!(point==0&&other_error);check(capture.total==(unsigned)expected);
 unsigned char after[96];check(!uc_mem_read(u,pc,after,96));if(corrupt)after[95]^=1;check(!memcmp(after,service_fixtures[point],96));check(!uc_close(u));
}
int run_service_observer_tests(void){
 for(unsigned p=0;p<sizeof(service_points)/sizeof(service_points[0]);p++)check(service_points[p].pc<=D27_SERVICE_PC_LIMIT-1-0x100000000ULL);
 for(unsigned bit=0;bit<4096;bit++){unsigned expected=0;for(unsigned p=0;p<sizeof(service_points)/sizeof(service_points[0]);p++)expected|=((service_points[p].pc&0x3fff)/4==bit);check(!!(d27_service_offsets[bit/64]&(1ULL<<(bit%64)))==!!expected);}
 memset(&capture,0,sizeof capture);check(!observe_service_entry(NULL,0xfffffe0008000000ULL,&capture)&&capture.total==0);

 for(unsigned p=0;p<sizeof(service_points)/sizeof(service_points[0]);p++){trial(p,0,0,0,0,0);trial(p,0x10210000,0,0,0,0);trial(p,0,1,0,0,0);trial(p,0,0,1,0,0);trial(p,0,0,0,1,0);trial(p,0x1000,0,0,0,0);}
 trial(0,0,0,0,0,1);bridge_output("PASS service observer fixture: 61 translated exact-signature/ASLR/EL/corruption/rate/error-filter cases; GPR/SP/PC/NZCV and code preserved\n");return 1;
}
