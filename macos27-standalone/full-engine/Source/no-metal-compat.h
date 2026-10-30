/* SPDX-License-Identifier: MIT
 * OPT-IN diagnostic candidate; real rendering remains unverified. Requires guest_read() and boot_input.h.
 * This handles only the exact supplied SkyLight no-device timeout. It changes
 * emulated control flow, not code bytes, and does not supply GPU acceleration.
 * Never include in a normal/default build. Runtime qualification is pending.
 */
static int diagnostic_no_metal_cleanup(uc_engine *u, uint64_t pc) {
 const uint64_t branch=0x18708b618ULL;
 if(pc<branch || pc-branch>0x100000000ULL || ((pc-branch)&0x3fff))return 0;
 uint32_t state=0;
 if(uc_reg_read(u,UC_ARM64_REG_PSTATE,&state) || (state&15))return 0;
 uint64_t result=0;
 if(uc_reg_read(u,UC_ARM64_REG_X21,&result) || !result)return 0;
 const uint8_t expected[32]={0x05,0x59,0xac,0x04,0x1a,0x11,0xc1,0x61,0xd2,0xe1,0x81,0xf4,0x1b,0xa4,0x45,0x9f,0x47,0x1c,0xec,0x87,0xc6,0x06,0x81,0x61,0x76,0x34,0x4e,0x8c,0xe1,0x8b,0x5b,0x8d};
 uint8_t code[0x120],actual[32],metal_lovin=255;
 if(!guest_read(u,pc-0x110,code,sizeof(code)))return 0;
 D27Sha256 hash;d27_sha_init(&hash);d27_sha_update(&hash,code,sizeof(code));d27_sha_final(&hash,actual);
 if(memcmp(actual,expected,32))return 0;
 if(!guest_read(u,0x1c39d44a8ULL+(pc-branch),&metal_lovin,1) || metal_lovin)return 0;
 /* dispatch_semaphore_wait actually failed and dispatch_release has already
  * run. Skip only the abort branch and true-result store. Original observer
  * removal, array release, stack guard, and authenticated epilogue still run. */
 uint64_t cleanup=pc+0x10;
 if(uc_reg_write(u,UC_ARM64_REG_PC,&cleanup))return 0;
 bridge_output("DIAGNOSTIC: genuine no-Metal timeout; continuing native cleanup with software capability false (no GPU acceleration)\n");
 return 1;
}
