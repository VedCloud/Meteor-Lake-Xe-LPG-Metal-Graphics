/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <unicorn/unicorn.h>
#include "vmapple-config.h"
uc_err vmapple_config_map(uc_engine*,uint64_t,unsigned,const char*);
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL VMApple config\n");abort();}}
int run_config_tests(void){
 uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);
 check(vmapple_config_map(u,16ULL*1024*1024*1024,2,"CPU")==UC_ERR_ARG);
 check(vmapple_config_map(u,16ULL*1024*1024*1024,1,"physical CPU brand")==0);
 check(uc_mem_map(u,0x1000,4096,UC_PROT_ALL)==0);
 const uint32_t code[]={0xb9400001,0xb9400402,0xf9401003};uint64_t base=0x400000;
 check(uc_mem_write(u,0x1000,code,sizeof(code))==0);check(uc_reg_write(u,UC_ARM64_REG_X0,&base)==0);check(uc_emu_start(u,0x1000,0x100c,0,20)==0);
 uint64_t v;check(uc_reg_read(u,UC_ARM64_REG_X1,&v)==0&&v==2);check(uc_reg_read(u,UC_ARM64_REG_X2,&v)==0&&v==1);check(uc_reg_read(u,UC_ARM64_REG_X3,&v)==0&&v==16ULL*1024*1024*1024);
 char text[32];check(uc_mem_read(u,base+0x540,text,32)==0&&!strcmp(text,"VirtualMac2,1"));check(uc_mem_read(u,base+0x584,text,32)==0&&!strcmp(text,"physical CPU brand"));
 uint32_t scratch=0x12345678;check(uc_mem_write(u,base+0x300,&scratch,4)==0);scratch=0;check(uc_mem_read(u,base+0x300,&scratch,4)==0&&scratch==0x12345678);
 check(uc_mem_read(u,base+0x6cc,text,32)==0);for(unsigned i=0;i<32;i++)check(text[i]==0);
 /* Verify the guest-visible entropy ABI and all failed-write boundaries.
  * These public fixture bytes are never used by the actual boot RNG path. */
 uint8_t before_seed[80],after_seed[80],seed[64],zeros[64]={0};
 for(unsigned i=0;i<sizeof(seed);i++)seed[i]=(uint8_t)(i+1);
 check(!uc_mem_read(u,base+0x5c0,before_seed,sizeof(before_seed)));
 check(vmapple_config_seed(NULL,seed,64)==UC_ERR_ARG);
 check(vmapple_config_seed(u,NULL,64)==UC_ERR_ARG);
 check(vmapple_config_seed(u,seed,63)==UC_ERR_ARG);
 check(vmapple_config_seed(u,seed,65)==UC_ERR_ARG);
 check(vmapple_config_seed(u,zeros,64)==UC_ERR_ARG);
 check(!uc_mem_read(u,base+0x5c0,after_seed,sizeof(after_seed)));
 check(!memcmp(before_seed,after_seed,sizeof(before_seed)));
 check(!vmapple_config_seed(u,seed,sizeof(seed)));
 check(!uc_mem_read(u,base+0x5c0,after_seed,sizeof(after_seed)));
 check(!memcmp(before_seed,after_seed,8));
 check(!memcmp(after_seed+8,seed,sizeof(seed)));
 check(!memcmp(before_seed+72,after_seed+72,8));
 const uint32_t read_seed=0xf9400001;uint64_t seed_address=base+0x5c8,first_word;
 memcpy(&first_word,seed,sizeof(first_word));
 check(!uc_mem_write(u,0x1100,&read_seed,sizeof(read_seed)));
 check(!uc_reg_write(u,UC_ARM64_REG_X0,&seed_address));
 check(!uc_emu_start(u,0x1100,0x1104,0,20));
 check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==first_word);
 bridge_output("PASS VMApple guest entropy: translated read at 0x5c8, exact 64-byte span, unchanged neighbors, invalid lengths/null/zero rejection\n");
 check(uc_mem_read(u,base+0x10000,text,1)==UC_ERR_READ_UNMAPPED);SparseRam ram;check(sparse_ram_init(&ram,u,0x800000000ULL,2*RAM_BLOCK,2*RAM_BLOCK)==0);check(sparse_ram_back(&ram,ram.base,4096)==0);
 VmappleConfigDma d;check(vmapple_config_dma_map(u,&d,&ram)==0);
 const uint32_t dma_code[]={0xf9000001};uint64_t doorbell=0x4ffff8,request=ram.base+0x100;
 uint8_t q[24]={1,0,1,0,0x11,0x22,0x33,0x44},copy[24];
 check(uc_mem_write(u,ram.base,dma_code,sizeof(dma_code))==0);check(uc_mem_write(u,request,q,sizeof(q))==0);
 check(uc_reg_write(u,UC_ARM64_REG_X0,&doorbell)==0);check(uc_reg_write(u,UC_ARM64_REG_X1,&request)==0);
 check(uc_emu_start(u,ram.base,ram.base+4,0,20)==0);check(d.unavailable==1&&!d.invalid);
 check(uc_mem_read(u,request,copy,sizeof(copy))==0&&!memcmp(q,copy,sizeof(q)));
 request=0x400000;check(uc_reg_write(u,UC_ARM64_REG_X1,&request)==0);check(uc_emu_start(u,ram.base,ram.base+4,0,20)==0);check(d.invalid==1&&d.unavailable==1);
 check(uc_close(u)==0);
 bridge_output("PASS VMApple config: ARM reads, 16 GiB/one CPU, honest identity, writable scratch, explicit unavailable DMA queries and bounds\n");return 1;
}
