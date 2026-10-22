/* SPDX-License-Identifier: MIT */
#include "../a64_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t code[16384],data[4096];
static int load(const char *path,uint8_t *out,size_t capacity){FILE*f=fopen(path,"rb");if(!f)return -1;size_t n=fread(out,1,capacity,f);int extra=fgetc(f);fclose(f);return extra==EOF?(int)n:-1;}
int main(int argc,char **argv){
 if(argc!=4)return 2;int n=load(argv[1],code,sizeof(code)),m=load(argv[2],data,sizeof(data));if(n<0||m<0)return 2;
 D27Cpu cpu;memset(&cpu,0,sizeof(cpu));D27Region regions[2]={{0,code,(size_t)n,D27_READ|D27_EXEC},{0x41000000,data,sizeof(data),D27_READ|D27_WRITE}};
 enum D27Stop stop=d27_a64_run(&cpu,regions,2,10000);
 if(stop!=D27_BREAKPOINT||cpu.syndrome!=((0x3cu<<26)|(1u<<25)|0x27)){fprintf(stderr,"stop=%u pc=%llx insn=%x fault=%llx\n",stop,(unsigned long long)cpu.pc,cpu.instruction,(unsigned long long)cpu.fault_address);return 1;}
 FILE*f=fopen(argv[3],"wb");if(!f)return 2;if(fwrite(data,1,80,f)!=80)return 2;fclose(f);
 printf("PASS interpreted ARM payload: %llu committed instructions, BRK at 0x%llx\n",(unsigned long long)cpu.retired,(unsigned long long)cpu.pc);return 0;
}
