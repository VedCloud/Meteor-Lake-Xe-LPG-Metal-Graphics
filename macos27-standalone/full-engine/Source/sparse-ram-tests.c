/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "sparse-ram.h"
void bridge_output(const char*);
static void expect(int v){if(!v){bridge_output("FAIL sparse physical RAM\n");abort();}}
int run_sparse_ram_tests(void){
 uc_engine*u=NULL;expect(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==UC_ERR_OK);expect(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==UC_ERR_OK);
 SparseRam r;uint64_t base=0x800000000ULL,size=16ULL*1024*1024*1024;expect(sparse_ram_init(&r,u,base,size,2*RAM_BLOCK)==UC_ERR_OK);
 expect(sparse_ram_back(&r,base-1,1)==UC_ERR_ARG);expect(sparse_ram_back(&r,UINT64_MAX,8)==UC_ERR_ARG);expect(sparse_ram_back(&r,base+size-1,2)==UC_ERR_ARG);expect(r.committed==0);
 expect(sparse_ram_back(&r,base+size-RAM_BLOCK,8)==UC_ERR_OK);expect(r.committed==RAM_BLOCK);
 uint64_t value=1;expect(uc_mem_read(u,base+size-8,&value,8)==UC_ERR_OK && value==0);value=0x123456789abcdef0ULL;expect(uc_mem_write(u,base+size-8,&value,8)==UC_ERR_OK);
 expect(sparse_ram_back(&r,base+size-8,8)==UC_ERR_OK && r.committed==RAM_BLOCK);value=0;expect(uc_mem_read(u,base+size-8,&value,8)==UC_ERR_OK && value==0x123456789abcdef0ULL);
 expect(sparse_ram_back(&r,base,RAM_BLOCK+1)==UC_ERR_NOMEM && r.committed==RAM_BLOCK);
 expect(sparse_ram_back(&r,base,1)==UC_ERR_OK && r.committed==2*RAM_BLOCK);
 expect(sparse_ram_back(&r,base+RAM_BLOCK,1)==UC_ERR_NOMEM && r.committed==2*RAM_BLOCK);
 expect(!sparse_ram_fault(&r,UC_MEM_READ_PROT,base,8));expect(uc_close(u)==UC_ERR_OK);
 bridge_output("PASS sparse physical RAM: 16 GiB logical range, zeroing, persistence, overflow/MMIO bounds and commit budget\n");return 1;
}
