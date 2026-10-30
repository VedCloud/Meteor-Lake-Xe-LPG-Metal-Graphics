/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "pci-ecam.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL PCI ECAM\n");abort();}}
int run_pci_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_mem_map(u,0x1000,4096,UC_PROT_ALL));PciEcam d;check(!pci_ecam_map(u,&d));
 const uint32_t code[]={0xb9400001,0xb9000402,0xb9400403,0xb9001002,0xb9401004};check(!uc_mem_write(u,0x1000,code,sizeof(code)));uint64_t base=0x40000000,all=UINT32_MAX,v;
 check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X2,&all));check(!uc_emu_start(u,0x1000,0x1014,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==0x00081b36);check(!uc_reg_read(u,UC_ARM64_REG_X3,&v)&&v==0x507);check(!uc_reg_read(u,UC_ARM64_REG_X4,&v)&&v==0);
 base+=0x1000;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_emu_start(u,0x1000,0x1014,0,20));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==UINT32_MAX);check(!uc_reg_read(u,UC_ARM64_REG_X3,&v)&&v==UINT32_MAX);
 base=0x4ffff000;check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_emu_start(u,0x1000,0x1004,0,10));check(!uc_reg_read(u,UC_ARM64_REG_X1,&v)&&v==UINT32_MAX);check(!d.invalid);check(!uc_close(u));bridge_output("PASS PCI: ARM GPEX host discovery, command masks, unimplemented BARs and absent endpoint functions\n");return 1;
}
