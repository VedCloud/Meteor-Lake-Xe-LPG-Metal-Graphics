/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "vmapple-bdif.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL VMAPPLE utility storage\n");abort();}}
static unsigned char storage[1024];static unsigned transfers;static int fail_backend;
static int transfer(void*o,uint64_t at,void*data,unsigned n,bool write){(void)o;transfers++;if(fail_backend||at>sizeof(storage)||n>sizeof(storage)-at)return 0;if(write)memcpy(storage+at,data,n);else memcpy(data,storage+at,n);return 1;}
typedef struct {uint64_t addr;uint32_t len,flags;} Desc;
int run_bdif_tests(void){
 uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);SparseRam ram;check(sparse_ram_init(&ram,u,0x800000000ULL,4*RAM_BLOCK,4*RAM_BLOCK)==0);check(sparse_ram_back(&ram,ram.base,4096)==0);
 VmappleBdif d;check(vmapple_bdif_map(u,&d,&ram,sizeof(storage),transfer,NULL)==0);
 /* Actual ARM guest stores the chain address to BDIF_CMD and polls ready. */
 const uint32_t code[]={0xf9020401,0xb9401002};
 check(uc_mem_write(u,ram.base,code,sizeof(code))==0);uint64_t mmio=0x30010000,chain=ram.base+0x100;
 check(uc_reg_write(u,UC_ARM64_REG_X0,&mmio)==0);check(uc_reg_write(u,UC_ARM64_REG_X1,&chain)==0);
 Desc req[3]={{ram.base+0x180,16,0x10001},{ram.base+0x200,512,0x10001},{ram.base+0x500,1,0x20001}};
 uint32_t sector[4]={1,0,1,0};unsigned char pattern[512],actual[512],status;
 for(unsigned i=0;i<512;i++)pattern[i]=(i*73)^0xa5;
 check(uc_mem_write(u,chain,req,sizeof(req))==0);check(uc_mem_write(u,ram.base+0x180,sector,sizeof(sector))==0);
 check(uc_mem_write(u,ram.base+0x200,pattern,512)==0);
 check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);
 check(uc_mem_read(u,ram.base+0x500,&status,1)==0&&status==0);check(d.writes==1&&!memcmp(storage+512,pattern,512));
 req[1].flags=0x30001;sector[0]=0;check(uc_mem_write(u,chain,req,sizeof(req))==0);check(uc_mem_write(u,ram.base+0x180,sector,sizeof(sector))==0);
 memset(actual,0,512);check(uc_mem_write(u,ram.base+0x200,actual,512)==0);check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);
 check(uc_mem_read(u,ram.base+0x200,actual,512)==0&&!memcmp(actual,pattern,512));check(d.reads==1);
 unsigned before=transfers;req[1].addr=UINT64_MAX-100;check(uc_mem_write(u,chain,req,sizeof(req))==0);check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);check(transfers==before);check(uc_mem_read(u,ram.base+0x500,&status,1)==0&&status==1);
 req[1].addr=ram.base+0x200;sector[2]=2;check(uc_mem_write(u,chain,req,sizeof(req))==0);check(uc_mem_write(u,ram.base+0x180,sector,sizeof(sector))==0);check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);check(transfers==before);
 sector[2]=0;fail_backend=1;check(uc_mem_write(u,ram.base+0x180,sector,sizeof(sector))==0);check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);check(uc_mem_read(u,ram.base+0x500,&status,1)==0&&status==1);check(d.failed==3&&!d.bad_access);
 before=transfers;req[2].addr=UINT64_MAX;fail_backend=0;check(uc_mem_write(u,chain,req,sizeof(req))==0);check(uc_emu_start(u,ram.base,ram.base+8,0,20)==0);check(transfers==before&&d.failed==4);
 check(uc_close(u)==0);bridge_output("PASS BDIF: ARM guest utility reads/writes, DMA and disk bounds, backend failure completion\n");return 1;
}
