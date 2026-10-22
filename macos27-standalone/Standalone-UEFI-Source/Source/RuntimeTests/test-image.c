/* SPDX-License-Identifier: MIT */
#include "../macho_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static uint8_t file[1024],baseline[1024],storage[128];
static void p32(size_t at,uint32_t v){for(unsigned i=0;i<4;i++)file[at+i]=(uint8_t)(v>>(i*8));}
static void p64(size_t at,uint64_t v){for(unsigned i=0;i<8;i++)file[at+i]=(uint8_t)(v>>(i*8));}
static void original(void){memcpy(file,baseline,sizeof(file));}
static void synthetic(void){
 p32(0,0xfeedfacf);p32(4,0x0100000c);p32(12,2);p32(16,2);p32(20,360);
 p32(32,0x19);p32(36,72);p64(56,0x1000);p64(64,16);p64(72,512);p64(80,4);p32(88,5);p32(92,5);
 p32(104,5);p32(108,288);p32(112,6);p32(116,68);p64(376,0x1000);p32(512,0xd503201f);
 memcpy(baseline,file,sizeof(file));
}
int main(int argc,char**argv){
 synthetic();D27Image plan,untouched;memset(&untouched,0xa5,sizeof(untouched));
 CHECK(d27_image_plan(file,sizeof(file),&plan)==0);CHECK(plan.count==1&&plan.bytes==16&&plan.entry==0x1000);
 for(size_t i=0;i<392;i++){D27Image out=untouched;CHECK(d27_image_plan(file,i,&out)!=0);CHECK(memcmp(&out,&untouched,sizeof(out))==0);}
 const size_t fields[]={0,4,12,16,20,36,64,80,88,92,108,112,116};
 for(size_t i=0;i<sizeof(fields)/sizeof(fields[0]);i++){original();p32(fields[i],UINT32_MAX);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);}
 original();p64(56,UINT64_MAX-2);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);
 original();p64(72,UINT64_MAX);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);
 original();p64(376,0x1004);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0); /* entry in zero-fill */
 original();p64(376,0x1001);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);
 original();p32(92,7);p32(88,7);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0); /* W+X */
 original();D27Region regions[16];uint64_t entry=0;
 memset(storage,0xa5,sizeof(storage));CHECK(d27_image_load(file,sizeof(file),0x8000,storage,sizeof(storage),regions,16,&plan,&entry)==0);
 CHECK(entry==0x8000&&regions[0].base==0x8000&&regions[0].length==16&&regions[0].permissions==5);
 CHECK(storage[0]==0x1f&&storage[3]==0xd5&&storage[4]==0&&storage[15]==0&&storage[16]==0xa5);
 CHECK(d27_image_load(file,sizeof(file),0x8000,storage,15,regions,16,&plan,&entry)!=0);
 CHECK(d27_image_load(file,sizeof(file),0x8000,storage,sizeof(storage),regions,0,&plan,&entry)!=0);
 CHECK(d27_image_load(file,sizeof(file),UINT64_MAX,storage,sizeof(storage),regions,16,&plan,&entry)!=0);
 CHECK(d27_image_load(file,sizeof(file),0x8000,file,sizeof(file),regions,16,&plan,&entry)!=0);
 /* Duplicate overlapping segments, then non-overlapping with an excessive span. */
 original();memmove(file+176,file+104,288);memcpy(file+104,file+32,72);p32(16,3);p32(20,432);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);
 p64(128,0x100000000ULL);CHECK(d27_image_plan(file,sizeof(file),&plan)!=0);
 printf("PASS %u loader boundary checks\n",checks);
 if(argc!=2)return 2;
 FILE*f=fopen(argv[1],"rb");CHECK(f!=NULL);CHECK(fseek(f,0,SEEK_END)==0);long n=ftell(f);CHECK(n>0&&n<16*1024*1024);rewind(f);
 uint8_t*real=malloc((size_t)n);CHECK(real!=NULL);CHECK(fread(real,1,(size_t)n,f)==(size_t)n);fclose(f);
 CHECK(d27_image_plan(real,(size_t)n,&plan)==0);uint8_t*ram=malloc((size_t)plan.bytes);CHECK(ram!=NULL);
 CHECK(d27_image_load(real,(size_t)n,0x800000000ULL,ram,(size_t)plan.bytes,regions,16,&plan,&entry)==0);
 D27Cpu cpu={0};cpu.pc=entry;cpu.current_el=2;cpu.os_lock=1;cpu.daif=0x3c0;
 enum D27Stop stop=d27_a64_run(&cpu,regions,plan.count,100);
 CHECK(stop==D27_DATA_FAULT&&cpu.retired==5&&cpu.fault_address==0&&cpu.instruction==0xf9400280);
 CHECK(cpu.os_lock==0&&cpu.daif==0x3c0); /* first real setup instructions changed guest state */
 printf("SPTM entry probe: segments=%zu bytes=%llu entry=0x%llx retired=%llu stop=DATA_FAULT instruction=0x%x pc=0x%llx address=0x%llx\n",plan.count,(unsigned long long)plan.bytes,(unsigned long long)entry,(unsigned long long)cpu.retired,cpu.instruction,(unsigned long long)cpu.pc,(unsigned long long)cpu.fault_address);
 free(ram);free(real);return 0;
}
