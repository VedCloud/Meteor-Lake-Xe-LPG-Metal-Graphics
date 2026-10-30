/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "glue.h"
#include "iosfc.h"
void bridge_output(const char*);
static void check(int v){if(!v){bridge_output("FAIL IOSFC transport\n");abort();}}
static IosfcDevice device;
static uint32_t entry(uint64_t address){return(uint32_t)((address>>14)<<2)|1;}
static void setx(uc_engine*u,unsigned reg,uint64_t v){check(!uc_reg_write(u,reg,&v));}
static uint64_t getx(uc_engine*u,unsigned reg){uint64_t v;check(!uc_reg_read(u,reg,&v));return v;}
int run_iosfc_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));SparseRam ram;check(!sparse_ram_init(&ram,u,0x800000000ULL,2*RAM_BLOCK,2*RAM_BLOCK));check(!sparse_ram_back(&ram,ram.base,0x20000));check(!uc_arm64_gic_init(u));check(!iosfc_map(u,&device,&ram));
 uint64_t b=ram.base,ring=b+0x4000,table=b+0x8000,desc=b+0xc200,root=b+0x10000,data=b+0x14000;
 const uint32_t code[]={0xf9080001,0xf9080402,0xf9080803,0xf9080c04,0xf9481005,0xf9481406};check(!uc_mem_write(u,b,code,sizeof(code)));setx(u,UC_ARM64_REG_X0,0x30210000);setx(u,UC_ARM64_REG_X1,ring);setx(u,UC_ARM64_REG_X2,1024);setx(u,UC_ARM64_REG_X3,table);setx(u,UC_ARM64_REG_X4,1);
 uint32_t pointer=entry(b+0xc000),pages[]={entry(data),entry(data+0x8000)};check(!uc_mem_write(u,table,&pointer,4));check(!uc_mem_write(u,root,pages,sizeof(pages)));
 uint8_t metadata[512]={0};uint32_t header[]={entry(root),0x42475241,0,0,0x8000,(64<<8)|1,(32<<8)|1,256};memcpy(metadata,header,sizeof(header));metadata[0x20]=4;check(!uc_mem_write(u,desc,metadata,sizeof(metadata)));
 uint32_t commands[][4]={{1,1,0xdeadbeef,0},{3,1,0xdeadbeef,0},{2,1,0xdeadbeef,0},{99,1,0xdeadbeef,0},{1,1,0xdeadbeef,0}};check(!uc_mem_write(u,ring,commands,sizeof(commands)));
 check(!uc_emu_start(u,b,b+24,0,100));check(getx(u,UC_ARM64_REG_X5)==1&&getx(u,UC_ARM64_REG_X6)==0);check(device.maps==1&&device.slot[1]);check(!uc_mem_read(u,desc,&pointer,4)&&(pointer&2));
 uint64_t cross=0xfeedcafedeadbeefULL,actual=0;check(iosfc_copy(&device,1,0x3ffc,&cross,8,1));check(iosfc_copy(&device,1,0x3ffc,&actual,8,0)&&actual==cross);check(!iosfc_copy(&device,1,0x7ffc,&cross,8,1));
 setx(u,UC_ARM64_REG_X4,2);check(!uc_emu_start(u,b+12,b+20,0,50));check(getx(u,UC_ARM64_REG_X5)==2);check(!uc_mem_read(u,data+0x8000,&actual,8)&&actual==0x4000);check(!uc_mem_read(u,data+0xbff8,&actual,8)&&actual==0x7ff8);
 setx(u,UC_ARM64_REG_X4,3);check(!uc_emu_start(u,b+12,b+20,0,50));check(!device.slot[1]&&device.unmaps==1);check(!uc_mem_read(u,desc,&pointer,4)&&!(pointer&2));
 setx(u,UC_ARM64_REG_X4,4);check(!uc_emu_start(u,b+12,b+20,0,50));check(!uc_mem_read(u,ring+3*16+8,&pointer,4)&&pointer==0xe00002c7);
 pages[1]=entry(0x20010000);check(!uc_mem_write(u,root,pages,sizeof(pages)));setx(u,UC_ARM64_REG_X4,5);check(!uc_emu_start(u,b+12,b+20,0,50));check(!uc_mem_read(u,ring+4*16+8,&pointer,4)&&pointer!=0);check(!device.slot[1]&&device.maps==1&&!device.invalid);
 iosfc_close(&device);check(!uc_close(u));bridge_output("PASS IOSFC: ARM ring, real guest-page mappings, cross-page copy, offset pattern, unmap and rejected DMA/commands\n");return 1;
}
