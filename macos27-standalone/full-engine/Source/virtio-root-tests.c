/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "virtio-root.h"
#include <unicorn/arm64.h>
void bridge_output(const char*);
static void check(int ok){if(!ok){bridge_output("FAIL read-only VirtIO root\n");abort();}}
static unsigned register_offset=0,features_low=0x60;
static unsigned char auxiliary_backing[8192];static unsigned auxiliary_flushes;static int auxiliary_fail;
static int auxiliary_io(void*p,uint64_t at,void*data,unsigned n,bool write){(void)p;if(auxiliary_fail)return 0;if(!n){if(write){auxiliary_flushes++;return 1;}return 0;}if(at>sizeof(auxiliary_backing)||n>sizeof(auxiliary_backing)-at)return 0;if(write)memcpy(auxiliary_backing+at,data,n);else memcpy(data,auxiliary_backing+at,n);return 1;}
static void write_reg(uc_engine*u,unsigned off,unsigned n,uint64_t value){uint64_t base=0x50000000+register_offset+off;uint32_t op=n==8?0xf9000001:n==4?0xb9000001:n==2?0x79000001:0x39000001;check(!uc_mem_write(u,0x800000000,&op,4));check(!uc_reg_write(u,UC_ARM64_REG_X0,&base));check(!uc_reg_write(u,UC_ARM64_REG_X1,&value));check(!uc_emu_start(u,0x800000000,0x800000004,0,1));}
static void setup(uc_engine*u){write_reg(u,20,1,0);write_reg(u,8,4,1);write_reg(u,12,4,1);write_reg(u,8,4,0);write_reg(u,12,4,features_low);write_reg(u,20,1,15);write_reg(u,24,2,8);write_reg(u,32,8,0x800002000);write_reg(u,40,8,0x800003000);write_reg(u,48,8,0x800004000);write_reg(u,28,2,1);}
int run_virtio_root_tests(void){
 uc_engine*u=NULL;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));SparseRam ram;check(!sparse_ram_init(&ram,u,0x800000000,0x400000000,64*1024*1024));check(!sparse_ram_back(&ram,0x800000000,0x20000));
 static VirtioRoot d;check(!virtio_root_map(u,&d,&ram,0x800010000,4096));virtio_root_config_write(&d,0x10,4,UINT32_MAX);check(virtio_root_config_read(&d,0x10,4)==0xffffc000);virtio_root_config_write(&d,0x10,4,0x50000000);check(virtio_root_config_read(&d,0,4)==0x1a00106b);
 /* Strictly ascending capability list and immutable identity. */
 unsigned last=0,cap=virtio_root_config_read(&d,0x34,1);while(cap){check(cap>last);last=cap;cap=virtio_root_config_read(&d,cap+1,1);}check(last==0xc0);
 /* Native AppleVirtIOStorageDevice::probe reads its type at config +0x3c. */
 uint32_t load=0xb9400001;uint64_t addr=0x5000203c,type=0;check(!uc_mem_write(u,0x800000000,&load,4));check(!uc_reg_write(u,UC_ARM64_REG_X0,&addr));check(!uc_emu_start(u,0x800000000,0x800000004,0,1));check(!uc_reg_read(u,UC_ARM64_REG_X1,&type)&&type==1);
 write_reg(u,0x2014,1,0);write_reg(u,0x2015,1,2);check(!d.invalid);
 setup(u);check(d.enabled&&d.status==15&&d.features==0x100000060ULL);
 struct {uint64_t a;uint32_t len;uint16_t flags,next;} q[]={{0x800005000,16,1,1},{0x800006000,512,3,2},{0x800007000,1,2,0}};
 uint32_t h[4]={0};uint16_t av[3]={1,1,0};uint8_t input[512],output[512],status=255;for(unsigned i=0;i<512;i++)input[i]=i^0xa5;
 check(!uc_mem_write(u,0x800010000,input,512));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800007000,&status,1));
 write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800006000,output,512));check(!memcmp(input,output,512));check(!uc_mem_read(u,0x800007000,&status,1)&&status==0);uint16_t used;check(!uc_mem_read(u,0x800004002,&used,2)&&used==1);check(d.reads==1&&!d.failed);
 /* Write request must never change backing image. */
 h[0]=1;av[1]=2;q[1].flags=1;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==1);check(!uc_mem_read(u,0x800010000,output,512)&&!memcmp(input,output,512));
 /* Truncated media read and cyclic chain are rejected. */
 h[0]=0;h[2]=8;q[1].flags=3;av[1]=3;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==1);
 /* Real MSI-X completion routes into the monitor GIC pending latch. */
 check(!uc_arm64_gic_init(u));h[2]=0;av[0]=0;av[1]=4;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));
 write_reg(u,26,2,1);write_reg(u,0x3010,8,0x1fff0040);write_reg(u,0x3018,4,160);virtio_root_config_write(&d,0xc2,2,0x8001);
 /* Vector initially masked: pending must survive until guest unmasks it. */
 write_reg(u,0x800,2,0);check(d.pending==2);write_reg(u,0x301c,4,0);unsigned long long pending=0;check(!uc_arm64_gic_mmio(u,0,0x214,4,0,&pending)&&pending&1);check(!d.pending);
 /* Re-arming can kick an empty queue; this is not a completion. */
 pending=1;check(!uc_arm64_gic_mmio(u,0,0x294,4,1,&pending));
 for(unsigned kick=0;kick<64;kick++)write_reg(u,0x800,2,0);
 pending=0;check(!uc_arm64_gic_mmio(u,0,0x214,4,0,&pending));
 if((pending&1)||d.pending||d.isr||d.used_idx!=4||d.reads!=2){bridge_output("FAIL empty VirtIO queue kick generated a completion interrupt without used-ring progress\n");abort();}
 bridge_output("PASS VirtIO empty kicks: 64 re-arms produce no duplicate completion or MSI-X interrupt\n");
 q[1].next=1;av[1]=5;check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(d.status&64);check(d.used_idx==4&&d.reads==2);
 write_reg(u,20,1,0);check(!d.enabled&&!d.status&&!d.desc&&!d.used&&!d.avail&&!d.last_avail);check(!d.invalid);
 static VirtioRoot aux;check(!virtio_aux_attach(&d,&aux,sizeof(auxiliary_backing),auxiliary_io,NULL));
 check(virtio_aux_attach(&d,&aux,sizeof(auxiliary_backing),auxiliary_io,NULL)==UC_ERR_ARG);
 virtio_root_config_write(&aux,0x10,4,0x50004000);register_offset=0x4000;features_low=0x240;
 setup(u);check(aux.enabled&&aux.status==15&&aux.features==0x100000240ULL);
 addr=0x5000603c;type=0;check(!uc_mem_write(u,0x800000000,&load,4));check(!uc_reg_write(u,UC_ARM64_REG_X0,&addr));check(!uc_emu_start(u,0x800000000,0x800000004,0,1));check(!uc_reg_read(u,UC_ARM64_REG_X1,&type)&&type==2);
 q[1].next=2;q[1].flags=1;h[0]=1;h[2]=3;av[0]=1;av[1]=1;
 check(!uc_mem_write(u,0x800006000,input,512));check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));
 write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==0&&aux.writes==1&&!memcmp(auxiliary_backing+1536,input,512));
 h[0]=0;q[1].flags=3;av[1]=2;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);
 check(!uc_mem_read(u,0x800007000,&status,1)&&status==0&&aux.reads==1);check(!uc_mem_read(u,0x800006000,output,512)&&!memcmp(output,input,512));
 /* Backend failure and out-of-range writes cannot report success. */
 auxiliary_fail=1;h[0]=1;q[1].flags=1;av[1]=3;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==1&&aux.writes==1);auxiliary_fail=0;
 h[2]=16;av[1]=4;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==1&&aux.writes==1);
 /* Actual flush callback, and error propagation, including Apple barrier. */
 q[0].next=2;h[0]=4;h[2]=0;av[1]=5;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800002000,q,sizeof(q)));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==0&&auxiliary_flushes==1);
 h[0]=0x10000;av[1]=6;check(!uc_mem_write(u,0x800005000,h,16));check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==0&&auxiliary_flushes==2);
 auxiliary_fail=1;av[1]=7;check(!uc_mem_write(u,0x800003000,av,sizeof(av)));write_reg(u,0x800,2,0);check(!uc_mem_read(u,0x800007000,&status,1)&&status==1&&auxiliary_flushes==2);auxiliary_fail=0;
 check(!d.enabled&&!d.used_idx&&d.reads==2);check(!uc_mem_read(u,0x800010000,output,512)&&!memcmp(input,output,512));
 bridge_output("PASS VirtIO AUX: independent role2 queue, real read/write roundtrip, immutable ROOT preserved, backend and bounds errors, durable flush/barrier errors propagated\n");
 check(!uc_close(u));
 bridge_output("PASS VirtIO root: ARM modern transport, immutable media read, write rejection, truncated read, cyclic queue rejection, masked MSI-X delivery and reset\n");return 1;
}
