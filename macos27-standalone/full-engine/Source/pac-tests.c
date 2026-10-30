/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <unicorn/unicorn.h>
#include "pac-probe.h"
void bridge_output(const char*);
static unsigned checks;
static void checked(uc_err e,unsigned line){if(e){char s[100];snprintf(s,sizeof(s),"FAIL PAC engine API line=%u error=%u\n",line,e);bridge_output(s);abort();}}
#define ok(e) checked((e),__LINE__)
static void verify(int yes){checks++;if(!yes){char s[80];snprintf(s,sizeof(s),"FAIL PAC assertion %u\n",checks);bridge_output(s);abort();}}
static void cp(uc_engine*u,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t v){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=v};ok(uc_reg_write(u,UC_ARM64_REG_CP_REG,&r));}
static uint64_t reg(uc_engine*u,int id){uint64_t v=0;ok(uc_reg_read(u,id,&v));return v;}
static void set(uc_engine*u,int id,uint64_t v){ok(uc_reg_write(u,id,&v));}
static uint64_t hvc(uc_engine*u,unsigned call,uint64_t x1,uint64_t x2){set(u,UC_ARM64_REG_X0,call);set(u,UC_ARM64_REG_X1,x1);set(u,UC_ARM64_REG_X2,x2);ok(uc_emu_start(u,0,4,0,10));return reg(u,UC_ARM64_REG_X0);}
static uint64_t pac(uc_engine*u,unsigned instruction,uint64_t p){set(u,UC_ARM64_REG_X9,p);set(u,UC_ARM64_REG_X10,0x9988);uc_err e=uc_emu_start(u,0x100+instruction*4,0x104+instruction*4,0,10);if(e){char s[110];snprintf(s,sizeof(s),"PAC instruction=%u checks=%u PSTATE=%llx\n",instruction,checks,(unsigned long long)reg(u,UC_ARM64_REG_PSTATE));bridge_output(s);}ok(e);return reg(u,UC_ARM64_REG_X9);}
static void el(uc_engine*u,unsigned n){uint32_t p=n?0x3c5:0x3c0;ok(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p));}
int run_pac_tests(EFI_SYSTEM_TABLE*t){
 uc_engine*u=NULL;ok(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));ok(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));ok(uc_mem_map(u,0,4096,UC_PROT_ALL));ok(uc_mem_write(u,0,pac_code,sizeof(pac_code)));
 verify(uc_arm64_monitor_counter(u,0)==UC_ERR_ARG);ok(uc_arm64_monitor_counter(u,1000000));
 uc_arm64_cp_reg frequency={.op0=3,.op1=3,.crn=14,.crm=0,.op2=0};ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&frequency));verify(frequency.val==1000000);
 uc_arm64_cp_reg counter={.op0=3,.op1=3,.crn=14,.crm=0,.op2=1};ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&counter));uint64_t before=counter.val;
 t->BootServices->Stall(10000);ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&counter));verify(counter.val>before && counter.val-before>=5000 && counter.val-before<50000);
 cp(u,6,1,1,0,0x30501);cp(u,4,1,1,0,(1ULL<<31)|(3ULL<<40));
 cp(u,0,2,0,2,16|(16ULL<<16));cp(u,0,1,0,0,(1ULL<<31)|(1ULL<<30)|(1ULL<<27)|(1ULL<<13));el(u,1);
 unsigned pac_checks=checks;
 verify(hvc(u,0x84000000,0,0)==0x10001);
 verify(hvc(u,0x8400000a,0x84000000,0)==0);
 verify(hvc(u,0x8400000a,0x84000008,0)==UINT64_MAX);
 verify(hvc(u,0xc4000004,0,0)==0);
 verify(hvc(u,0xc4000004,1,0)==UINT64_MAX-1);
 verify(hvc(u,0xc4000004,0,4)==UINT64_MAX-1);
 verify(hvc(u,0xc4000003,0,0)==UINT64_MAX-3);
 verify(hvc(u,0xc4000003,1,0)==UINT64_MAX-1);
 verify(hvc(u,0x84000008,0,0)==UINT64_MAX);
 bridge_output("PASS PSCI: version/features, CPU affinity, already-on and unsupported power requests\n");
 checks=pac_checks;
 verify(hvc(u,0xc1000000,0,0)==UINT64_MAX); /* no entropy: explicit failure */
 unsigned char secret[16];for(unsigned i=0;i<16;i++)secret[i]=i;
 const unsigned long long defaults[4]={0x1234,0x2345,0x3456,0x4567};
 ok(uc_arm64_vmapple_configure(u,secret,defaults));verify(uc_arm64_vmapple_configure(u,secret,defaults)==UC_ERR_ARG);
 verify(hvc(u,0xc1000002,7,0)==UINT64_MAX); /* no initial state */
 verify(hvc(u,0xc1000000,0,0)==0);verify(hvc(u,0xc1000001,0,0)==0);
 for(unsigned i=0;i<4;i++)verify(reg(u,UC_ARM64_REG_X1+i)==defaults[i]);
 uc_arm64_cp_reg keylo={.op0=3,.op1=0,.crn=2,.crm=1,.op2=0};ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&keylo));
 uc_arm64_cp_reg keyhi=keylo;keyhi.op2=1;ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&keyhi));
 verify(keylo.val==0xceb3f698f6926cb3ULL && keyhi.val==0xb2565d57936d9417ULL);
 uint64_t original=0x12345678, signed_values[4];
 for(unsigned role=0;role<4;role++){
  signed_values[role]=pac(u,role*2,original);verify(signed_values[role]!=original);
  verify(pac(u,role*2+1,signed_values[role])==original);
 }
 verify(signed_values[0]!=signed_values[1] && signed_values[0]!=signed_values[2] && signed_values[2]!=signed_values[3]);
 verify(hvc(u,0xc1000002,0x9876,0)==0);verify(pac(u,1,signed_values[0])!=original);verify(pac(u,5,signed_values[2])!=original);
 verify(pac(u,3,signed_values[1])==original);verify(pac(u,7,signed_values[3])==original);
 verify(hvc(u,0xc1000002,defaults[0],0)==0);verify(pac(u,0,original)==signed_values[0]);
 verify(hvc(u,0xc1000003,0xabcd,0)==0);verify(pac(u,3,signed_values[1])!=original);verify(pac(u,7,signed_values[3])!=original);verify(pac(u,1,signed_values[0])==original);
 uint64_t g=pac(u,8,original);verify(hvc(u,0xc1000006,0xdead,0)==0);verify(pac(u,8,original)!=g);verify(hvc(u,0xc1000006,defaults[3],0)==0);verify(pac(u,8,original)==g);
 verify(hvc(u,0xc1000003,defaults[1],0)==0);
 verify(hvc(u,0xc1000004,0xabcde,0)==0);verify(pac(u,0,original)==signed_values[0]);
 el(u,0);uint64_t user[4];for(unsigned i=0;i<4;i++){user[i]=pac(u,i*2,original);verify(user[i]!=signed_values[i]);verify(pac(u,i*2+1,user[i])==original);}
 verify(pac(u,8,original)==g);el(u,1);
 verify(pac(u,0,original)==signed_values[0]);verify(pac(u,1,user[0])!=original);
 verify(hvc(u,0xc1000005,1,0xabcde)==0);
 for(unsigned i=0;i<4;i++){verify(pac(u,i*2,original)==user[i]);verify(pac(u,i*2+1,user[i])==original);}
 verify(hvc(u,0xc1000005,2,0)==UINT64_MAX);verify(pac(u,0,original)==user[0]);
 verify(hvc(u,0xc1000005,0,0xabcde)==0);verify(pac(u,0,original)==signed_values[0]);
 verify(hvc(u,0xc1000000,0,0)==0);verify(pac(u,0,original)==signed_values[0]);
 verify(hvc(u,0xc10000f0,0,0)==0);verify(pac(u,0,original)==signed_values[0]);
 verify(hvc(u,0x8300ff01,0,0)==UINT64_MAX);verify(hvc(u,0x8300ff03,0,0)==UINT64_MAX);verify(hvc(u,0x8300feff,0,0)==UINT64_MAX);
 set(u,UC_ARM64_REG_X0,0xc100007f);verify(uc_emu_start(u,0,4,0,10)==UC_ERR_EXCEPTION);
 ok(uc_close(u));char s[120];snprintf(s,sizeof(s),"PASS VMAPPLE PAC: %u HVC, key, signature rejection and EL0/EL1 diversification checks\n",checks);bridge_output(s);return 1;
}

#include "auth-load-probe.h"
static uint64_t load_signed(uc_engine*u,unsigned key,uint64_t pointer){
 set(u,UC_ARM64_REG_X9,pointer);set(u,UC_ARM64_REG_X10,0);
 ok(uc_emu_start(u,key?8:4,key?12:8,0,2));return reg(u,UC_ARM64_REG_X9);
}
int run_pac_load_tests(void){
 checks=0;uc_engine*u=NULL;ok(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));ok(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));
 ok(uc_mem_map(u,0,4096,UC_PROT_ALL));ok(uc_mem_map(u,0x8000,4096,UC_PROT_READ|UC_PROT_WRITE));ok(uc_mem_write(u,0,auth_load_code,sizeof(auth_load_code)));
 cp(u,6,1,1,0,0x30501);cp(u,4,1,1,0,(1ULL<<31)|(3ULL<<40));cp(u,0,1,0,0,(1ULL<<31)|(1ULL<<30)|(1ULL<<27)|(1ULL<<13));el(u,1);
 unsigned char secret[16];for(unsigned i=0;i<16;i++)secret[i]=i;const unsigned long long defaults[4]={0x1234,0x2345,0x3456,0x4567};
 ok(uc_arm64_vmapple_configure(u,secret,defaults));verify(hvc(u,0xc1000000,0,0)==0);
 const uint64_t a=0x1122334455667788ULL,b=0x9988776655443322ULL;ok(uc_mem_write(u,0x8028,&a,8));ok(uc_mem_write(u,0x8078,&b,8));ok(uc_mem_write(u,0x8000,&b,8));
 for(unsigned tbi=0;tbi<2;tbi++){
  el(u,1);cp(u,0,2,0,2,tbi?(17|(17ULL<<16)|(1ULL<<37)):(16|(16ULL<<16)));el(u,0);
  for(unsigned nonzero_sp=0;nonzero_sp<2;nonzero_sp++){
   set(u,UC_ARM64_REG_SP,nonzero_sp?0x5550:0);uint64_t signed_a=load_signed(u,0,0x8000);
   ok(uc_emu_start(u,12,16,0,2));verify(reg(u,UC_ARM64_REG_X11)==a);verify(reg(u,UC_ARM64_REG_X9)==signed_a);
   set(u,UC_ARM64_REG_X9,signed_a);ok(uc_emu_start(u,16,20,0,2));verify(reg(u,UC_ARM64_REG_X11)==a);verify(reg(u,UC_ARM64_REG_X9)==0x8028);
   load_signed(u,1,0x8080);ok(uc_emu_start(u,20,24,0,2));verify(reg(u,UC_ARM64_REG_X11)==b);verify(reg(u,UC_ARM64_REG_X9)==0x8078);
  }
  uint64_t signed_sp=load_signed(u,0,0x8000);set(u,UC_ARM64_REG_SP,signed_sp);ok(uc_emu_start(u,24,28,0,2));verify(reg(u,UC_ARM64_REG_X11)==a);verify(reg(u,UC_ARM64_REG_SP)==signed_sp);
  signed_sp=load_signed(u,1,0x8080);set(u,UC_ARM64_REG_SP,signed_sp);ok(uc_emu_start(u,28,32,0,2));verify(reg(u,UC_ARM64_REG_X11)==b);verify(reg(u,UC_ARM64_REG_SP)==signed_sp);
  set(u,UC_ARM64_REG_SP,0x5550);uint64_t signed_a=load_signed(u,0,0x8000);set(u,UC_ARM64_REG_X9,signed_a^(1ULL<<48));set(u,UC_ARM64_REG_X11,0xdeadbeef);
  verify(uc_emu_start(u,32,36,0,2)==UC_ERR_EXCEPTION);verify(reg(u,UC_ARM64_REG_X11)==0xdeadbeef);
  load_signed(u,1,0x8000);verify(uc_emu_start(u,32,36,0,2)==UC_ERR_EXCEPTION);
  if(tbi){load_signed(u,0,0xab00000000008000ULL);ok(uc_emu_start(u,16,20,0,2));verify(reg(u,UC_ARM64_REG_X11)==a);verify(reg(u,UC_ARM64_REG_X9)==0xab00000000008028ULL);}
 }
 ok(uc_close(u));char line[140];snprintf(line,sizeof(line),"PASS authenticated loads: %u zero-modifier A/B, nonzero SP, writeback, TBI/tag and invalid-PAC checks\n",checks);bridge_output(line);return 1;
}
