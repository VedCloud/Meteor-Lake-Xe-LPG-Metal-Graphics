/* SPDX-License-Identifier: MIT */
#include "../a64_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { checks++; if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} } while(0)
static uint8_t code[32],data[32];
static D27Cpu cpu;
static D27Region map[3];
static void instruction(uint32_t word){for(unsigned i=0;i<4;i++)code[i]=(uint8_t)(word>>(8*i));}
static void setup(uint32_t word){memset(&cpu,0,sizeof(cpu));memset(code,0,sizeof(code));memset(data,0xa5,sizeof(data));instruction(word);map[0]=(D27Region){0,code,sizeof(code),D27_READ|D27_EXEC};map[1]=(D27Region){0x1000,data,sizeof(data),D27_READ|D27_WRITE};}
static enum D27Stop once(void){return d27_a64_run(&cpu,map,2,1);}
int main(void){
 setup(0xab010002);cpu.x[0]=UINT64_MAX;cpu.x[1]=1;CHECK(once()==D27_BUDGET);CHECK(cpu.x[2]==0&&cpu.nzcv==0x60000000&&cpu.retired==1);
 setup(0x2b010002);cpu.x[0]=0x7fffffff;cpu.x[1]=1;CHECK(once()==D27_BUDGET);CHECK(cpu.x[2]==0x80000000&&cpu.nzcv==0x90000000);
 setup(0xeb010002);cpu.x[0]=0;cpu.x[1]=1;CHECK(once()==D27_BUDGET);CHECK(cpu.x[2]==UINT64_MAX&&cpu.nzcv==0x80000000);
 setup(0x910043ff);cpu.sp=0x1000;CHECK(once()==D27_BUDGET);CHECK(cpu.sp==0x1010);
 setup(0xaa1f03e0);cpu.x[0]=UINT64_MAX;CHECK(once()==D27_BUDGET);CHECK(cpu.x[0]==0); /* ORR X0,XZR,XZR */
 setup(0x52800020);cpu.x[0]=UINT64_MAX;CHECK(once()==D27_BUDGET);CHECK(cpu.x[0]==1); /* MOVZ W0 */
 setup(0xd63f03c0);cpu.x[30]=16;CHECK(once()==D27_BUDGET);CHECK(cpu.pc==16&&cpu.x[30]==4); /* BLR X30 reads old LR */
 setup(0x17ffffff);CHECK(once()==D27_BUDGET);CHECK(cpu.pc==UINT64_MAX-3); /* signed B -4 */
 setup(0x54000040);cpu.nzcv=0x40000000;CHECK(once()==D27_BUDGET);CHECK(cpu.pc==8); /* B.EQ */
 setup(0xb4000040);CHECK(once()==D27_BUDGET);CHECK(cpu.pc==8); /* CBZ X0 */
 setup(0xb5000040);cpu.x[0]=1;CHECK(once()==D27_BUDGET);CHECK(cpu.pc==8);
 setup(0xd51b4200);cpu.x[0]=UINT64_MAX;CHECK(once()==D27_BUDGET);CHECK(cpu.nzcv==0xf0000000);
 setup(0xd53b4200);cpu.nzcv=0xa0000000;CHECK(once()==D27_BUDGET);CHECK(cpu.x[0]==0xa0000000);
 const uint32_t unsupported[]={0,0xd503233f,0xd5381000,0xd69f03e0,0x4e201c00,0x0bc10000,0x52c00000,0x39c00000};
 for(unsigned i=0;i<sizeof(unsupported)/sizeof(unsupported[0]);i++){setup(unsupported[i]);cpu.x[0]=123;CHECK(once()==D27_UNSUPPORTED);CHECK(cpu.retired==0&&cpu.pc==0&&cpu.x[0]==123);}
 setup(0xd503201f);map[0].permissions=D27_READ;CHECK(once()==D27_FETCH_FAULT);CHECK(cpu.retired==0);
 setup(0xd503201f);map[0].length=3;CHECK(once()==D27_FETCH_FAULT);CHECK(cpu.fault_address==3);
 setup(0xd503201f);cpu.pc=2;CHECK(once()==D27_PC_ALIGNMENT);
 setup(0xf9400020);cpu.x[1]=0x2000;cpu.x[0]=123;CHECK(once()==D27_DATA_FAULT);CHECK(cpu.x[0]==123&&cpu.retired==0&&cpu.pc==0);
 setup(0xf9000020);cpu.x[1]=0x1000;map[1].permissions=D27_READ;CHECK(once()==D27_DATA_FAULT);CHECK(data[0]==0xa5);
 setup(0xf9000020);cpu.x[1]=0x101c;CHECK(once()==D27_DATA_FAULT);CHECK(data[28]==0xa5&&data[31]==0xa5); /* no partial store */
 setup(0xf9000020);cpu.x[1]=UINT64_MAX-3;CHECK(once()==D27_DATA_FAULT);
 setup(0xf94003e0);cpu.sp=0x1001;CHECK(once()==D27_SP_ALIGNMENT);
 setup(0xf9000020);cpu.x[1]=0x1000;cpu.x[0]=0x0807060504030201ULL;map[1].length=4;map[2]=(D27Region){0x1004,data+4,4,D27_READ|D27_WRITE};CHECK(d27_a64_run(&cpu,map,3,1)==D27_BUDGET);CHECK(data[0]==1&&data[7]==8);
 setup(0xf9000020);cpu.x[1]=0x1000;map[1].length=4;map[2]=(D27Region){0x1004,data+4,4,D27_READ};CHECK(d27_a64_run(&cpu,map,3,1)==D27_DATA_FAULT);CHECK(data[0]==0xa5&&data[7]==0xa5);
 setup(0xd503201f);map[1].base=0;CHECK(once()==D27_BAD_MAP);
 setup(0xd503201f);map[1].base=UINT64_MAX;CHECK(once()==D27_BAD_MAP);
 setup(0x14000000);CHECK(d27_a64_run(&cpu,map,2,123)==D27_BUDGET);CHECK(cpu.retired==123&&cpu.pc==0);
 setup(0xd42004e0);CHECK(once()==D27_BREAKPOINT);CHECK(cpu.syndrome==((0x3cu<<26)|(1u<<25)|0x27)&&cpu.pc==0&&cpu.retired==0);
 CHECK(d27_a64_run(&cpu,map,2,100)==D27_BREAKPOINT); /* terminal stop cannot silently restart */
 printf("PASS %u execution/fault checks\n",checks);return 0;
}
