/* SPDX-License-Identifier: MIT. Exercise guest MMU translations, not host reads. */
#include "glue.h"
#include <unicorn/unicorn.h>
void bridge_output(const char*);
static void ok(uc_err e){if(e){bridge_output("FAIL SPRR engine API\n");abort();}}
static void cp(uc_engine*u,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t v){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=v};ok(uc_reg_write(u,UC_ARM64_REG_CP_REG,&r));}
static uint64_t readcp(uc_engine*u,unsigned crm,unsigned op2){uc_arm64_cp_reg r={.op0=3,.op1=6,.crn=15,.crm=crm,.op2=op2};ok(uc_reg_read(u,UC_ARM64_REG_CP_REG,&r));return r.val;}
static int assert_sprr(int yes){if(!yes){bridge_output("FAIL SPRR permission/bank assertion\n");return 0;}return 1;}
int run_sprr_tests(void){
 uc_engine*u=NULL;ok(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));ok(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_APPLE_SPRR));ok(uc_mem_map(u,0,0x20000,UC_PROT_ALL));
 uint64_t desc=0x11003;ok(uc_mem_write(u,0x10008,&desc,8));desc=0x12003;ok(uc_mem_write(u,0x11000,&desc,8));
 cp(u,6,1,1,0,0x501);uint32_t p=0x3c9;ok(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p));
 cp(u,6,15,15,7,0x123);cp(u,6,15,1,7,0x456);
 if(!assert_sprr(readcp(u,1,6)==0x123 && readcp(u,1,7)==0x456))return 0;
 cp(u,4,1,1,0,(1ULL<<34)|(1ULL<<31)); /* HCR_EL2.E2H */
 if(!assert_sprr(readcp(u,1,6)==0x456 && readcp(u,15,7)==0x123))return 0;
 cp(u,6,15,1,6,0x789);if(!assert_sprr(readcp(u,1,7)==0x789 && readcp(u,15,7)==0x123))return 0;
 cp(u,4,1,1,0,1ULL<<31);p=0x3c5;ok(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p));
 cp(u,0,2,0,0,0x10000);cp(u,0,2,0,2,25);cp(u,0,1,0,0,1);
 cp(u,6,15,1,0,1);
 const uint32_t code[]={0xf9400001,0xf9000001,0xd503201f}; /* ldr;str;nop */
 ok(uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t nop=0xd503201f;ok(uc_mem_write(u,0x2000,&nop,8));
 const unsigned rights[16]={0,5,1,3,0,5,1,0,0,4,1,3,0,5,1,3};unsigned checks=0;
 for(unsigned index=0;index<16;index++){
  unsigned ci=index==0?12:0;
  desc=0x1000|0x403|((uint64_t)(ci&3)<<53)|((uint64_t)(ci>>2)<<6);ok(uc_mem_write(u,0x12008,&desc,8));
  desc=0x2000|0x403|((uint64_t)(index&3)<<53)|((uint64_t)(index>>2)<<6);ok(uc_mem_write(u,0x12010,&desc,8));
  for(unsigned n=0;n<16;n++)for(unsigned operation=0;operation<3;operation++){
   cp(u,6,15,1,6,((uint64_t)1<<(4*ci))|((uint64_t)n<<(4*index)));
   uint64_t x0=0x40002000,marker=0x0102030405060708ULL;ok(uc_reg_write(u,UC_ARM64_REG_X0,&x0));if(operation==1)ok(uc_reg_write(u,UC_ARM64_REG_X1,&marker));
   uint64_t begin=operation==2?0x40002000:0x40001000+operation*4;
   uc_err e=uc_emu_start(u,begin,begin+4,0,4);
   int permitted=(rights[n]&(1<<operation))!=0;
   /* Every operation must execute or fault according to the table. */
   if(!assert_sprr(permitted?e==UC_ERR_OK:e==UC_ERR_EXCEPTION)){
    char s[140];snprintf(s,sizeof(s),"SPRR mismatch index=%u nibble=%u op=%u error=%u\n",index,n,operation,e);bridge_output(s);return 0;
   }
   if(permitted && operation==0){uint64_t value=0;ok(uc_reg_read(u,UC_ARM64_REG_X1,&value));if(!assert_sprr(value==nop))return 0;}
   if(permitted && operation==1){uint64_t value=0;ok(uc_mem_read(u,0x2000,&value,8));if(!assert_sprr(value==marker))return 0;}
   /* Restore the instruction overwritten by a successful store. */
   ok(uc_mem_write(u,0x2000,&nop,8));checks++;
  }
 }
 /* A guest range TLBI must observe changed PTEs, rather than returning
  * successfully while leaving stale physical mappings in the TLB. */
 desc=0x2000|0x403|(3ULL<<53)|(3ULL<<6);ok(uc_mem_write(u,0x12010,&desc,8));cp(u,6,15,1,6,1ULL|(3ULL<<60));
 uint64_t expected_new=0x9876543210abcdefULL;ok(uc_mem_write(u,0x3000,&expected_new,8));
 if(!assert_sprr(uc_emu_start(u,0x40001000,0x40001004,0,4)==UC_ERR_OK))return 0;
 desc=0x3000|0x403|(3ULL<<53)|(3ULL<<6);ok(uc_mem_write(u,0x12010,&desc,8));
 const uint32_t tlbi_code[]={0xd50882a8,0xf9400001};ok(uc_mem_write(u,0x1100,tlbi_code,sizeof(tlbi_code)));uint64_t range=0x40002000>>12;ok(uc_reg_write(u,UC_ARM64_REG_X8,&range));
 if(!assert_sprr(uc_emu_start(u,0x40001100,0x40001108,0,8)==UC_ERR_OK))return 0;
 uint64_t observed=0;ok(uc_reg_read(u,UC_ARM64_REG_X1,&observed));if(!assert_sprr(observed==expected_new))return 0;
 bridge_output("PASS TLBI range: guest invalidation observes changed physical mapping\n");
 /* UPERM must control EL0 independently of permissive EL1 permissions. */
 cp(u,6,15,1,6,1ULL|(3ULL<<60));cp(u,6,15,1,5,1);
 p=0x3c0;ok(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p));
 if(!assert_sprr(uc_emu_start(u,0x40001000,0x40001004,0,4)==UC_ERR_EXCEPTION))return 0;
 cp(u,6,15,1,5,1ULL|(3ULL<<60));
 if(!assert_sprr(uc_emu_start(u,0x40001000,0x40001004,0,4)==UC_ERR_OK))return 0;
 p=0x3c5;ok(uc_reg_write(u,UC_ARM64_REG_PSTATE,&p));
 /* A valid guest PTE cannot authorize access outside monitor-owned RAM. */
 desc=0x30000|0x403|(3ULL<<53)|(3ULL<<6);ok(uc_mem_write(u,0x12010,&desc,8));cp(u,6,15,1,6,1ULL|(3ULL<<60));
 if(!assert_sprr(uc_emu_start(u,0x40001000,0x40001004,0,4)==UC_ERR_READ_UNMAPPED))return 0;
 bridge_output("PASS system MMU: physical read/write contents, EL0 separation and unmapped backing rejection\n");
 ok(uc_close(u));char s[128];snprintf(s,sizeof(s),"PASS SPRR: %u nonidentity guest MMU permission checks and VHE/EL12 banking\n",checks);bridge_output(s);return 1;
}
