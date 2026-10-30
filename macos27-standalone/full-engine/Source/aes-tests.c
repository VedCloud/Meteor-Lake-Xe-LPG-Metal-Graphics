/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "vmapple-aes.h"
void bridge_output(const char*);
static void checked(int v,unsigned line){if(!v){char msg[90];snprintf(msg,sizeof(msg),"FAIL guest AES protocol line=%u\n",line);bridge_output(msg);abort();}}
#define check(v) checked((v),__LINE__)
static void set(uc_engine*u,int r,uint64_t v){check(uc_reg_write(u,r,&v)==0);}
static void write_reg(uc_engine*u,uint64_t at,uint32_t v){set(u,UC_ARM64_REG_X0,at);set(u,UC_ARM64_REG_X1,v);check(uc_emu_start(u,0x1004,0x1008,0,20)==0);}
static uint32_t read_reg(uc_engine*u,uint64_t at){uint64_t v;set(u,UC_ARM64_REG_X0,at);check(uc_emu_start(u,0x1000,0x1004,0,20)==0);check(uc_reg_read(u,UC_ARM64_REG_X2,&v)==0);return v;}
static void fifo(uc_engine*u,uint32_t v){write_reg(u,0x30220200,v);}
static void key(uc_engine*u,const unsigned char*k,unsigned bytes,unsigned enc,unsigned cbc){fifo(u,0x10000000|((bytes-16)/8<<22)|(enc<<20)|(cbc<<16));for(unsigned i=0;i<bytes;i+=4){uint32_t v;memcpy(&v,k+i,4);fifo(u,v);}}
static void iv(uc_engine*u,const unsigned char*v){fifo(u,0x20000000);for(unsigned i=0;i<16;i+=4){uint32_t w;memcpy(&w,v+i,4);fifo(u,w);}}
static void crypt(uc_engine*u,uint64_t src,uint64_t dst,unsigned n){fifo(u,0x50000000|n);fifo(u,((src>>32)<<16)|(dst>>32));fifo(u,src);fifo(u,dst);}
static void hex(unsigned char*out,const char*in,unsigned n){for(unsigned j=0;j<2*n;j++){unsigned char c=in[j];unsigned v=c<='9'?c-'0':c-'a'+10;if(!(j&1))out[j/2]=v<<4;else out[j/2]|=v;}}
int run_aes_tests(void){
 uc_engine*u=NULL;check(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u)==0);check(uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC)==0);
 check(uc_mem_map(u,0x1000,4096,UC_PROT_ALL)==0);check(uc_arm64_gic_init(u)==0);const uint32_t code[]={0xb9400002,0xb9000001};check(uc_mem_write(u,0x1000,code,sizeof(code))==0);
 SparseRam ram;uint64_t base=0x800000000ULL;check(sparse_ram_init(&ram,u,base,2*RAM_BLOCK,2*RAM_BLOCK)==0);check(sparse_ram_back(&ram,base,4096)==0);
 static VmappleAes s;check(vmapple_aes_map(u,&s,&ram)==0);check(read_reg(u,0x30230000)==15);check(read_reg(u,0x3022000c)==0x3f80);
 write_reg(u,0x30220020,64);check(read_reg(u,0x30220020)==64);
 check(read_reg(u,0x30220008)==0);write_reg(u,0x30220008,1);check(read_reg(u,0x30220008)==1);
 unsigned char k[32],plain[16],expected[16],result[16];for(unsigned j=0;j<32;j++)k[j]=j;
 hex(plain,"00112233445566778899aabbccddeeff",16);hex(expected,"69c4e0d86a7b0430d8cdb78070b4c55a",16);
 check(uc_mem_write(u,base,plain,16)==0);key(u,k,16,1,0);crypt(u,base,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,expected,16));
 key(u,k,16,0,0);crypt(u,base+0x100,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,plain,16));
 hex(expected,"dda97ca4864cdfe06eaf70a0ec0d7191",16);key(u,k,24,1,0);crypt(u,base,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,expected,16));
 hex(expected,"8ea2b7ca516745bfeafc49904b496089",16);key(u,k,32,1,0);crypt(u,base,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,expected,16));
 unsigned char initial_iv[16];hex(k,"2b7e151628aed2a6abf7158809cf4f3c",16);hex(initial_iv,"000102030405060708090a0b0c0d0e0f",16);hex(plain,"6bc1bee22e409f96e93d7e117393172a",16);hex(expected,"7649abac8119b246cee98e9b12e9197d",16);
 check(uc_mem_write(u,base,plain,16)==0);key(u,k,16,1,1);iv(u,initial_iv);crypt(u,base,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,expected,16));
 fifo(u,0x60000008);fifo(u,(uint32_t)(base+0x200));check(uc_mem_read(u,base+0x200,result,16)==0&&!memcmp(result,expected,16));
 key(u,k,16,0,1);iv(u,initial_iv);crypt(u,base+0x100,base+0x100,16);check(uc_mem_read(u,base+0x100,result,16)==0&&!memcmp(result,plain,16));
 write_reg(u,0x3022001c,36);fifo(u,0x8800005a);check(s.irq_asserted&&read_reg(u,0x30220018)==32&&read_reg(u,0x30220030)==0x5a);write_reg(u,0x30220018,32);check(!s.irq_asserted);
 /* Queue multiple complete commands while stopped, then START must drain
  * both in order. CLEAR_FIFO must remove a partial key without changing keys. */
 write_reg(u,0x30220008,2);fifo(u,0x80000011);fifo(u,0x88000022);
 check(read_reg(u,0x30220024)==0x200&&s.flag==0x5a);
 write_reg(u,0x30220008,1);check(s.flag==0x22&&s.irq_asserted&&read_reg(u,0x30220024)==2);
 write_reg(u,0x30220018,32);write_reg(u,0x30220008,2);fifo(u,0x10000000);fifo(u,0xdeadbeef);
 write_reg(u,0x30220008,4);check(s.count==0&&!s.running);write_reg(u,0x30220008,1);check(s.key[0].bytes==16);
 uint64_t before=s.operations;crypt(u,0x30220000,base,16);check(s.operations==before&&s.rejected==1&&s.irq_asserted);write_reg(u,0x30220018,4);
 fifo(u,0x11000000);check(s.rejected==2&&!s.key[0].bytes);check(read_reg(u,0x30230000)==15);check(!s.bad_access&&s.operations==6);
 for(unsigned bit=0;bit<4;bit++)write_reg(u,0x30230000,1u<<bit);write_reg(u,0x30230000,0);check(read_reg(u,0x30230000)==15&&!s.bad_access);
 check(uc_close(u)==0);bridge_output("PASS AES: NIST 128/192/256 ECB, CBC and IV writeback, in-place decrypt, IRQ/W1C, START/STOP/queued drain/CLEAR_FIFO/watermark, invalid DMA and unavailable UID/GID\n");return 1;
}
