/* SPDX-License-Identifier: MIT
 * Read-only exact-build diagnostic. Exports names, status codes and selected
 * return addresses only, never key references, key bytes or process payloads.
 * No guest writes, exception suppression or fabricated service results.
 */
typedef struct {const char*name;uint64_t pc;unsigned result;unsigned char hash[32];} D27ServicePoint;
#include "service-points.h"
#include "service-offset-filter.h"
typedef struct {unsigned seen[sizeof(service_points)/sizeof(service_points[0])];unsigned total;} D27ServiceObserver;
static int observe_service_entry(uc_engine*u,uint64_t pc,D27ServiceObserver*s){
 if(pc>=D27_SERVICE_PC_LIMIT || (pc&3) || !(d27_service_offsets[(pc&0x3fff)>>8] & (1ULL<<((pc>>2)&63))))return 0;
 for(unsigned n=0;n<sizeof(service_points)/sizeof(service_points[0]);n++){
  const D27ServicePoint*p=&service_points[n];
  if(s->seen[n]>=8 || pc<p->pc || pc-p->pc>0x100000000ULL || ((pc-p->pc)&0x3fff))continue;
  uint32_t mode=0;if(uc_reg_read(u,UC_ARM64_REG_PSTATE,&mode)||(mode&15))continue;
  unsigned char b[96],hash[32];if(!guest_read(u,pc,b,sizeof(b)))continue;
  D27Sha256 h;d27_sha_init(&h);d27_sha_update(&h,b,sizeof(b));d27_sha_final(&h,hash);
  if(memcmp(hash,p->hash,32))continue;
  uc_arm64_cp_reg tcr={.op0=3,.op1=0,.crn=2,.crm=0,.op2=2};if(uc_reg_read(u,UC_ARM64_REG_CP_REG,&tcr))continue;
  unsigned bits=64-(tcr.val&63);if(bits<32||bits>48)continue;
  uint64_t mask=(1ULL<<bits)-1,lr=0,fp=0,value=0;
  if(uc_reg_read(u,UC_ARM64_REG_X30,&lr)||uc_reg_read(u,UC_ARM64_REG_X29,&fp))continue;
  if(p->result && uc_reg_read(u,UC_ARM64_REG_X0,&value))continue;
  /* CSSM diagnostics retain only the failure under investigation. */
  if(n==0 && (uint32_t)value!=0x80010911U)continue;
  s->seen[n]++;s->total++;char text[230];
  snprintf(text,sizeof(text),"OBSERVED service %s pc=0x%llx slide=0x%llx lr=0x%llx status=0x%x occurrence=%u (read-only)\n",p->name,(unsigned long long)pc,(unsigned long long)(pc-p->pc),(unsigned long long)(lr&mask),p->result?(unsigned)value:0,s->seen[n]);bridge_output(text);
  fp&=mask;
  for(unsigned i=0;i<12 && fp && !(fp&15);i++){
   uint64_t frame[2];if(!guest_read(u,fp,frame,sizeof(frame)))break;
   snprintf(text,sizeof(text),"  service return[%u]=0x%llx\n",i,(unsigned long long)(frame[1]&mask));bridge_output(text);
   uint64_t next=frame[0]&mask;if(next<=fp||next-fp>0x40000)break;fp=next;
  }
  return 1;
 }
 return 0;
}
