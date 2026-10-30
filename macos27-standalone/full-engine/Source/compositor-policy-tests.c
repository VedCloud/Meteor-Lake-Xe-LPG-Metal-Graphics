/* SPDX-License-Identifier: MIT — isolated control-flow guard unit, not a GUI test. */
#include "glue.h"
#include <unicorn/unicorn.h>
#include <unicorn/arm64.h>
#include "../Standalone/boot_input.h"
void bridge_output(const char*);
static int guest_read(uc_engine*u,uint64_t va,void*out,unsigned n){unsigned long long pa=0;return !uc_arm64_monitor_translate(u,va,0,&pa)&&!uc_mem_read(u,pa,out,n);}
#include "no-metal-compat.h"
#include "compositor-fixture.h"
static void check(int ok){if(!ok){bridge_output("FAIL compositor compatibility guard\n");abort();}}
static unsigned redirects,blocks;
static int enabled;
static uint64_t fixture_slide;
static void observe(uc_engine*u,uint64_t pc,uint32_t n,void*p){(void)n;(void)p;if(++blocks>64){bridge_output("FAIL compositor compatibility loop\n");uc_emu_stop(u);return;}if(enabled&&diagnostic_no_metal_cleanup(u,pc))redirects++;}
static void stop(uc_engine*u,uint64_t pc,uint32_t n,void*p){(void)pc;(void)n;(void)p;uc_emu_stop(u);}
static void trial(int on,int timeout,int corrupt,int kernel,int already_metal){
 uc_engine*u=0;check(!uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&u));check(!uc_ctl_set_cpu_model(u,UC_CPU_ARM64_VMAPPLE_PAC));check(!uc_mem_map(u,(0x187088000ULL+fixture_slide),0x8000,UC_PROT_ALL));check(!uc_mem_map(u,(0x1c39d4000ULL+fixture_slide),0x4000,UC_PROT_ALL));check(!uc_mem_write(u,(0x18708b508ULL+fixture_slide),supplied_compositor_code,sizeof(supplied_compositor_code)));
 /* Fixture-only endpoints outside the pinned signature. They identify the
  * executed branch and do not model rendering or a Metal device. */
 uint32_t cleanup[]={0xd28000e0,0x14000015},abort_path[]={0xd2800540,0x14000006};check(!uc_mem_write(u,(0x18708b628ULL+fixture_slide),cleanup,sizeof(cleanup)));check(!uc_mem_write(u,(0x18708b664ULL+fixture_slide),abort_path,sizeof(abort_path)));
 if(corrupt){uint32_t changed=0xd503201f;check(!uc_mem_write(u,(0x18708b508ULL+fixture_slide),&changed,4));}
 uint8_t metal=already_metal;check(!uc_mem_write(u,(0x1c39d44a8ULL+fixture_slide),&metal,1));uint32_t mode=kernel?5:0;check(!uc_reg_write(u,UC_ARM64_REG_PSTATE,&mode));uint64_t result=timeout;check(!uc_reg_write(u,UC_ARM64_REG_X21,&result));
 uc_hook a,b;enabled=on;redirects=blocks=0;check(!uc_hook_add(u,&a,UC_HOOK_BLOCK,observe,0,1,0));check(!uc_hook_add(u,&b,UC_HOOK_CODE,stop,0,(0x18708b680ULL+fixture_slide),(0x18708b680ULL+fixture_slide)));check(!uc_emu_start(u,(0x18708b618ULL+fixture_slide),0,0,0));uint64_t x0=0;check(!uc_reg_read(u,UC_ARM64_REG_X0,&x0));check(!uc_mem_read(u,(0x1c39d44a8ULL+fixture_slide),&metal,1));
 int should=on&&timeout&&!corrupt&&!kernel&&!already_metal&&!(fixture_slide&0x3fff);check(redirects==(unsigned)should);check(x0==((should||!timeout)?7:42));check(metal==(!timeout||already_metal));check(!uc_close(u));
}
int run_compositor_policy_tests(void){trial(0,1,0,0,0);trial(1,1,0,0,0);trial(1,0,0,0,0);trial(1,1,1,0,0);trial(1,1,0,1,0);trial(1,1,0,0,1);fixture_slide=0x4000000;trial(1,1,0,0,0);fixture_slide=0x1000;trial(1,1,0,0,0);bridge_output("PASS compositor compatibility guard: translated branch/redirection, disabled path, real success unchanged, wrong signature/EL/nonzero capability refused; no renderer tested\n");return 1;}
