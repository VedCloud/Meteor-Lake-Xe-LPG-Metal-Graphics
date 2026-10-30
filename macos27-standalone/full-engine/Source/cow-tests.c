/* SPDX-License-Identifier: MIT. COW and ASID isolation with real 16 KiB walks. */
#include "glue.h"
#include <unicorn/unicorn.h>
void bridge_output(const char*);
static void checked(int yes,unsigned line){if(!yes){char msg[100];snprintf(msg,sizeof(msg),"FAIL 16K COW/ASID isolation line=%u\n",line);bridge_output(msg);abort();}}
#define check(x) checked((x),__LINE__)
static void cp(uc_engine*u,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t v){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=v};check(uc_reg_write(u,UC_ARM64_REG_CP_REG,&r)==0);}
static void reg(uc_engine*u,int id,uint64_t v){check(uc_reg_write(u,id,&v)==0);}
static void el(uc_engine*u,unsigned n){uint32_t p=n?0x3c5:0x3c0;check(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p)==0);}
static void word(uc_engine*u,uint64_t a,uint64_t v){check(uc_mem_write(u,a,&v,8)==0);}
static uint64_t get(uc_engine*u,uint64_t a){uint64_t v=0;check(uc_mem_read(u,a,&v,8)==0);return v;}
static void tlbi(uc_engine*u){el(u,1);uc_err e=uc_emu_start(u,0x40004008,0x40004010,0,0);if(e){char b[120];uint64_t pc=0;uc_reg_read(u,UC_ARM64_REG_PC,&pc);snprintf(b,sizeof(b),"COW TLBI error=%u pc=0x%llx\n",e,(unsigned long long)pc);bridge_output(b);}check(e==0);el(u,0);}
static uint64_t load(uc_engine*u){uint64_t v=0;check(uc_emu_start(u,0x40004000,0x40004004,0,0)==0);check(uc_reg_read(u,UC_ARM64_REG_X1,&v)==0);return v;}
static void unrelated_code(uc_engine*u,uint64_t p,uint32_t n,void*d){(void)u;(void)p;(void)n;(void)d;}
static void stop_code(uc_engine*u,uint64_t p,uint32_t n,void*d){(void)p;(void)n;(void)d;uc_emu_stop(u);}
static void stop_block(uc_engine*u,uint64_t p,uint32_t n,void*d){(void)p;(void)n;if(++*(unsigned*)d==5)uc_emu_stop(u);}
static void check_hook_restart(void){
 uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);check(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==0);check(uc_mem_map(u,0x1000,4096,UC_PROT_ALL)==0);
 const uint32_t a[]={0x91000463,0x14000007},b[]={0x91000442,0x17fffff7};check(uc_mem_write(u,0x1000,a,sizeof(a))==0);check(uc_mem_write(u,0x1020,b,sizeof(b))==0);
 uc_hook hc,hb;unsigned armed=0;check(uc_hook_add(u,&hc,UC_HOOK_CODE,unrelated_code,NULL,0x1800,0x1800)==0);check(uc_hook_add(u,&hb,UC_HOOK_BLOCK,stop_block,&armed,0x1020,0x1020)==0);
 reg(u,UC_ARM64_REG_X2,0);reg(u,UC_ARM64_REG_X3,0);check(uc_emu_start(u,0x1000,0x1800,0,0)==0);
 uint64_t pc=0,x2=0,x3=0;check(uc_reg_read(u,UC_ARM64_REG_PC,&pc)==0);check(uc_reg_read(u,UC_ARM64_REG_X2,&x2)==0);check(uc_reg_read(u,UC_ARM64_REG_X3,&x3)==0);if(pc!=0x1020||x2!=4||x3!=5){char out[160];snprintf(out,sizeof(out),"Partial code-hook TB stop PC=0x%llx expected=0x1020 effects=%llu,%llu\n",(unsigned long long)pc,(unsigned long long)x2,(unsigned long long)x3);bridge_output(out);}check(pc==0x1020&&x2==4&&x3==5);
 check(uc_hook_del(u,hb)==0);uc_hook hs;check(uc_hook_add(u,&hs,UC_HOOK_CODE,stop_code,NULL,0x1024,0x1024)==0);check(uc_emu_start(u,0x1020,0x1800,0,0)==0);check(uc_reg_read(u,UC_ARM64_REG_PC,&pc)==0&&pc==0x1024);check(uc_reg_read(u,UC_ARM64_REG_X2,&x2)==0&&x2==5);
 check(uc_close(u)==0);bridge_output("PASS partial code hooks: chained TB stop preserves PC and executes prior effects once\n");
}
int run_cow_tests(void){
 uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);check(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==0);check(uc_mem_map(u,0,0x30000,UC_PROT_ALL)==0);
 const uint32_t fetch_code[]={0x91000442,0x94001ff3,0xd503201f};check(uc_mem_write(u,0x4030,fetch_code,sizeof(fetch_code))==0);
 const uint32_t code[]={0xf9400001,0xf9000001,0xd508831f,0xd5033fdf,0x91000442,0xd101c3ff,0xf9000001,0x9101c3ff};check(uc_mem_write(u,0x4000,code,sizeof(code))==0);
 /* Parent and child have separate table trees and initially share one page. */
 for(unsigned n=0;n<2;n++){uint64_t root=0x10000+n*0x10000;word(u,root,root+0x4000+3);word(u,root+0x4000+32*8,root+0x8000+3);word(u,root+0x8000+8,0x4000|0x4c3);word(u,root+0x8000+16,0x8000|0x4c3);}
 word(u,0x18000+16,0x8000|0x443);word(u,0x8000,0x1122334455667788ULL);
 cp(u,6,1,1,0,0x501);cp(u,4,1,1,0,1ULL<<31);el(u,1);cp(u,0,2,0,0,(1ULL<<48)|0x10000);cp(u,0,2,0,2,25|(2ULL<<14)|(2ULL<<32));cp(u,0,1,0,0,1);el(u,0);reg(u,UC_ARM64_REG_X0,0x40008000);
 check(load(u)==0x1122334455667788ULL);reg(u,UC_ARM64_REG_X1,0x8877665544332211ULL);check(uc_emu_start(u,0x40004004,0x40004008,0,0)==0);
 /* Revoke write permission after warming the parent's write TLB. */
 word(u,0x18000+16,0x8000|0x4c3);tlbi(u);reg(u,UC_ARM64_REG_X1,0xfeedface);
 check(uc_emu_start(u,0x40004004,0x40004008,0,0)==UC_ERR_EXCEPTION);check(get(u,0x8000)==0x8877665544332211ULL);
 el(u,1);cp(u,0,2,0,0,(2ULL<<48)|0x20000);el(u,0);check(load(u)==0x8877665544332211ULL);reg(u,UC_ARM64_REG_X1,0xdeadbeef);
 check(uc_emu_start(u,0x40004004,0x40004008,0,0)==UC_ERR_EXCEPTION);check(get(u,0x8000)==0x8877665544332211ULL);
 /* A fault late in a translated block must resume at the store itself,
  * preserving prior effects once. Replaying the prologue would re-sign LR
  * and decrement SP twice, matching the recovery child-process failure. */
 reg(u,UC_ARM64_REG_X2,0);reg(u,UC_ARM64_REG_SP,0x4000f000);
 check(uc_emu_start(u,0x40004010,0x40004020,0,0)==UC_ERR_EXCEPTION);
 uint64_t pc=0,sp=0,x2=0;check(uc_reg_read(u,UC_ARM64_REG_PC,&pc)==0);check(uc_reg_read(u,UC_ARM64_REG_SP,&sp)==0);check(uc_reg_read(u,UC_ARM64_REG_X2,&x2)==0);
 if(pc!=0x40004018){char msg[150];snprintf(msg,sizeof(msg),"COW precise fault PC=0x%llx expected=0x40004018 SP=0x%llx effects=%llu\n",(unsigned long long)pc,(unsigned long long)sp,(unsigned long long)x2);bridge_output(msg);}
 check(pc==0x40004018&&sp==0x4000ef90&&x2==1);
 /* Model the guest COW handler: copy page, update child PTE and invalidate. */
 word(u,0xc000,get(u,0x8000));word(u,0x28000+16,0xc000|0x443);tlbi(u);reg(u,UC_ARM64_REG_X1,0xfedcba9876543210ULL);check(uc_emu_start(u,0x40004004,0x40004008,0,0)==0);check(get(u,0xc000)==0xfedcba9876543210ULL&&get(u,0x8000)==0x8877665544332211ULL);
 el(u,1);cp(u,0,2,0,0,(1ULL<<48)|0x10000);el(u,0);check(load(u)==0x8877665544332211ULL);
 el(u,1);cp(u,0,2,0,0,(2ULL<<48)|0x20000);el(u,0);check(load(u)==0xfedcba9876543210ULL);
 reg(u,UC_ARM64_REG_X2,0);uc_err fetch_error=uc_emu_start(u,0x40004030,0x4000403c,0,0);
 uint64_t fetch_pc=0;check(uc_reg_read(u,UC_ARM64_REG_PC,&fetch_pc)==0);
 if(fetch_error!=UC_ERR_EXCEPTION||fetch_pc!=0x4000c000){char b[180];snprintf(b,sizeof(b),"COW branch-fetch fault error=%u PC=0x%llx expected=0x4000c000\n",fetch_error,(unsigned long long)fetch_pc);bridge_output(b);}
 check(fetch_error==UC_ERR_EXCEPTION&&fetch_pc==0x4000c000);check(uc_close(u)==0);
 check_hook_restart();
 bridge_output("PASS 16K COW: warm-TLB write revocation, EL0 faults, private copy and parent/child ASID isolation\n");return 1;
}
