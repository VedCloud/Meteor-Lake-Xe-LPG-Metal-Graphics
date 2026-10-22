/* SPDX-License-Identifier: MIT
 * A64 encoding facts follow Arm DDI 0602. Independently implemented probe core;
 * QEMU is used for differential testing, not copied into this MIT module. */
#include "a64_core.h"
static uint64_t mask(unsigned width) { return width==64?UINT64_MAX:UINT32_MAX; }
static uint64_t sign_extend(uint64_t value,unsigned bits) {
 uint64_t sign=UINT64_C(1)<<(bits-1);return (value^sign)-sign;
}
static uint64_t reg(const D27Cpu *c,unsigned n,int sp) {
 return n==31?(sp?c->sp:0):c->x[n];
}
static void put(D27Cpu *c,unsigned n,uint64_t value,unsigned width,int sp) {
 value&=mask(width);
 if(n!=31)c->x[n]=value;else if(sp)c->sp=value;
}
static enum D27Stop fault(D27Cpu *c,enum D27Stop why,uint64_t address) {
 c->stop=why;c->fault_address=address;return why;
}
static int valid_map(const D27Region *map,size_t count) {
 if(!map||!count||count>32)return 0;
 for(size_t i=0;i<count;i++) {
  if(!map[i].bytes||!map[i].length||map[i].length>UINT64_MAX-map[i].base||
     (map[i].permissions&~7u))return 0;
  uint64_t end=map[i].base+map[i].length;
  for(size_t j=0;j<i;j++)
   if(map[i].base<map[j].base+map[j].length&&map[j].base<end)return 0;
 }
 return 1;
}
static uint8_t *resolve(D27Region *map,size_t count,uint64_t address,unsigned access) {
 for(size_t i=0;i<count;i++)
  if(address>=map[i].base&&address-map[i].base<map[i].length&&
     (map[i].permissions&access)==access)return map[i].bytes+(size_t)(address-map[i].base);
 return NULL;
}
static int memory(D27Cpu *c,D27Region *map,size_t count,uint64_t address,
                  unsigned bytes,unsigned access,uint64_t *value) {
 uint8_t *where[8];
 if(!bytes||bytes>8||address>UINT64_MAX-(bytes-1)) {
  fault(c,access==D27_EXEC?D27_FETCH_FAULT:D27_DATA_FAULT,address);return 0;
 }
 // Validate every byte before a store. A fault must not leave a partial write,
 // including an access that straddles two adjacent regions or a permissions gap.
 for(unsigned i=0;i<bytes;i++) {
  where[i]=resolve(map,count,address+i,access);
  if(!where[i]){fault(c,access==D27_EXEC?D27_FETCH_FAULT:D27_DATA_FAULT,address+i);return 0;}
 }
 if(access==D27_WRITE) {
  for(unsigned i=0;i<bytes;i++)*where[i]=(uint8_t)(*value>>(8*i));
 }else{
  *value=0;for(unsigned i=0;i<bytes;i++)*value|=(uint64_t)*where[i]<<(8*i);
 }
 return 1;
}
static uint64_t shifted(uint64_t value,unsigned type,unsigned amount,unsigned width) {
 uint64_t limit=mask(width);value&=limit;
 if(!amount)return value;
 if(type==0)return (value<<amount)&limit;
 if(type==1)return value>>amount;
 if(type==2) {
  uint64_t out=value>>amount;
  if(value&(UINT64_C(1)<<(width-1)))out|=limit^(limit>>amount);
  return out;
 }
 return ((value>>amount)|(value<<(width-amount)))&limit;
}
static int condition(unsigned cond,uint32_t flags) {
 unsigned n=(flags>>31)&1,z=(flags>>30)&1,c=(flags>>29)&1,v=(flags>>28)&1;
 int result;
 switch(cond>>1) {
  case 0:result=(int)z;break;case 1:result=(int)c;break;
  case 2:result=(int)n;break;case 3:result=(int)v;break;
  case 4:result=c&&!z;break;case 5:result=n==v;break;
  case 6:result=n==v&&!z;break;default:return 1;
 }
 return (cond&1)?!result:result;
}
static uint64_t arithmetic(D27Cpu *c,uint64_t a,uint64_t b,unsigned width,int sub,int flags) {
 uint64_t limit=mask(width);a&=limit;b&=limit;
 uint64_t result=(sub?a-b:a+b)&limit;
 if(flags) {
  unsigned carry=sub?(a>=b):(width==32?(a+b)>UINT32_MAX:result<a);
  unsigned overflow=(unsigned)(((sub?(a^b):~(a^b))&(a^result))>>(width-1))&1;
  c->nzcv=((uint32_t)(result>>(width-1))<<31)|((uint32_t)(result==0)<<30)|
           ((uint32_t)carry<<29)|((uint32_t)overflow<<28);
 }
 return result;
}
static enum D27Stop step(D27Cpu *c,D27Region *map,size_t count) {
 if(c->pc&3)return fault(c,D27_PC_ALIGNMENT,c->pc);
 uint64_t fetched=0;
 if(!memory(c,map,count,c->pc,4,D27_EXEC,&fetched))return c->stop;
 uint32_t insn=(uint32_t)fetched;c->instruction=insn;
 uint64_t next=c->pc+4,result=0;
 unsigned rd=insn&31,rn=(insn>>5)&31,rm=(insn>>16)&31;
 unsigned width=(insn>>31)?64:32;
 if(insn==0xd503201f) { /* NOP only; PAC and other hints are not no-ops here. */ }
 else if((insn&0xffe0001f)==0xd4200000) {
  c->syndrome=(0x3cu<<26)|(1u<<25)|((insn>>5)&0xffff);
  return fault(c,D27_BREAKPOINT,c->pc);
 }
 else if((insn&0x1f800000)==0x12800000) {
  unsigned op=(insn>>29)&3,shift=((insn>>21)&3)*16;
  if(op==1||shift>=width)return fault(c,D27_UNSUPPORTED,c->pc);
  uint64_t immediate=(uint64_t)((insn>>5)&0xffff)<<shift;
  result=op==0?~immediate:op==2?immediate:
         (reg(c,rd,0)&~(UINT64_C(0xffff)<<shift))|immediate;
  put(c,rd,result,width,0);
 }
 else if((insn&0x1f800000)==0x11000000) {
  unsigned setflags=(insn>>29)&1,subtract=(insn>>30)&1;
  uint64_t immediate=(uint64_t)((insn>>10)&0xfff)<<(((insn>>22)&1)?12:0);
  result=arithmetic(c,reg(c,rn,1),immediate,width,subtract,setflags);
  put(c,rd,result,width,!setflags);
 }
 else if((insn&0x1f200000)==0x0b000000) {
  unsigned shift=(insn>>22)&3,amount=(insn>>10)&63;
  if(shift==3||amount>=width)return fault(c,D27_UNSUPPORTED,c->pc);
  result=arithmetic(c,reg(c,rn,0),shifted(reg(c,rm,0),shift,amount,width),width,
                    (insn>>30)&1,(insn>>29)&1);
  put(c,rd,result,width,0);
 }
 else if((insn&0x1f000000)==0x0a000000) {
  unsigned op=(insn>>29)&3,amount=(insn>>10)&63;
  if(amount>=width)return fault(c,D27_UNSUPPORTED,c->pc);
  uint64_t b=shifted(reg(c,rm,0),(insn>>22)&3,amount,width);
  if((insn>>21)&1)b=~b;
  uint64_t a=reg(c,rn,0);
  result=(op==0||op==3)?a&b:op==1?a|b:a^b;
  result&=mask(width);
  if(op==3)c->nzcv=((uint32_t)(result>>(width-1))<<31)|((uint32_t)(result==0)<<30);
  put(c,rd,result,width,0);
 }
 else if((insn&0x1f000000)==0x10000000) {
  uint64_t offset=sign_extend((((uint64_t)insn>>5)&0x7ffff)*4+((insn>>29)&3),21);
  result=(insn>>31)?(c->pc&~UINT64_C(4095))+(offset<<12):c->pc+offset;
  put(c,rd,result,64,0);
 }
 else if((insn&0x7c000000)==0x14000000) {
  next=c->pc+(sign_extend(insn&0x03ffffff,26)<<2);
  if(insn>>31)c->x[30]=c->pc+4;
 }
 else if((insn&0xff000010)==0x54000000) {
  if(condition(insn&15,c->nzcv))next=c->pc+(sign_extend((insn>>5)&0x7ffff,19)<<2);
 }
 else if((insn&0x7e000000)==0x34000000) {
  int zero=(reg(c,rd,0)&mask(width))==0;
  if(zero!=((insn>>24)&1))next=c->pc+(sign_extend((insn>>5)&0x7ffff,19)<<2);
 }
 else if((insn&0xfffffc1f)==0xd61f0000||
         (insn&0xfffffc1f)==0xd63f0000||
         (insn&0xfffffc1f)==0xd65f0000) {
  if(rn==31)return fault(c,D27_UNSUPPORTED,c->pc);
  next=c->x[rn];if((insn&0xfffffc1f)==0xd63f0000)c->x[30]=c->pc+4;
 }
 else if((insn&0x3f000000)==0x39000000) {
  unsigned bytes=1u<<(insn>>30),op=(insn>>22)&3;
  if(op>1)return fault(c,D27_UNSUPPORTED,c->pc);
  if(rn==31&&(c->sp&15))return fault(c,D27_SP_ALIGNMENT,c->sp);
  uint64_t address=reg(c,rn,1)+(uint64_t)((insn>>10)&0xfff)*bytes;
  result=reg(c,rd,0);
  if(!memory(c,map,count,address,bytes,op?D27_READ:D27_WRITE,&result))return c->stop;
  if(op)put(c,rd,result,bytes==8?64:32,0);
 }
 else if(insn==0xd5033fdf) { /* ISB SY: interpreter has no prefetched/JIT state. */ }
 else if((insn&0xfffff0ff)==0xd50340df||(insn&0xfffff0ff)==0xd50340ff) {
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  uint32_t bits=((insn>>8)&15)<<6;
  if((insn&0xff)==0xdf)c->daif|=bits;else c->daif&=~bits;
 }
 else if((insn&0xffffffe0)==0xd5101080) { /* OSLAR_EL1 */
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  c->os_lock=(unsigned)(reg(c,rd,0)&1);
 }
 else if((insn&0xffffffe0)==0xd5384240) { /* CurrentEL */
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  put(c,rd,(uint64_t)c->current_el<<2,64,0);
 }
 else if((insn&0xffffffe0)==0xd53b4220) { /* DAIF */
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  put(c,rd,c->daif&0x3c0,64,0);
 }
 else if((insn&0xffffffe0)==0xd538c000) { /* VBAR_EL1 */
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  put(c,rd,c->vbar_el1,64,0);
 }
 else if((insn&0xffffffe0)==0xd518c000) { /* VBAR_EL1 */
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  c->vbar_el1=reg(c,rd,0)&~UINT64_C(0x7ff);
 }
 else if((insn&0xffffffe0)==0xd53c1100||(insn&0xffffffe0)==0xd51c1100) {
  if(c->current_el!=2)return fault(c,D27_UNSUPPORTED,c->pc);
  /* Only reading reset HCR is supported until its virtualization effects exist. */
  if((insn&0xffffffe0)==0xd51c1100)return fault(c,D27_UNSUPPORTED,c->pc);
  put(c,rd,c->hcr_el2,64,0);
 }
 else if((insn&0xffffffe0)==0xd53b4200)put(c,rd,c->nzcv,64,0); /* MRS NZCV */
 else if((insn&0xffffffe0)==0xd51b4200)c->nzcv=(uint32_t)reg(c,rd,0)&0xf0000000; /* MSR NZCV */
 else return fault(c,D27_UNSUPPORTED,c->pc);
 c->pc=next;c->retired++;return D27_RUNNING;
}
enum D27Stop d27_a64_run(D27Cpu *c,D27Region *map,size_t count,uint64_t budget) {
 if(!c)return D27_BAD_MAP;
 if(c->stop!=D27_RUNNING)return c->stop;
 if(!valid_map(map,count))return fault(c,D27_BAD_MAP,c->pc);
 for(uint64_t i=0;i<budget;i++)if(step(c,map,count)!=D27_RUNNING)return c->stop;
 return fault(c,D27_BUDGET,c->pc);
}
