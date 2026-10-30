/* SPDX-License-Identifier: LGPL-2.1-or-later
 * Firmware differential tests of QEMU's full QARMA computation. Reference
 * includes retain the upstream Linaro notice and LGPL terms.
 */
#include "glue.h"
#include <stdint.h>
#define extract64(v,s,n) (((uint64_t)(v)>>(s))&((UINT64_C(1)<<(n))-1))
#define extract32 extract64
#define MAKE_64BIT_MASK(s,n) (((UINT64_C(1)<<(n))-1)<<(s))
typedef struct {uint64_t lo,hi;} ARMPACKey;
#include "Performance/qarma-reference.inc"
#include "Performance/qarma-optimized.inc"
void bridge_output(const char*);
static uint64_t state=0x87162389baff0103ULL;
static uint64_t rng(void){state^=state<<13;state^=state>>7;state^=state<<17;return state;}
int run_qarma_tests(void){
 for(unsigned j=0;j<4096;j++){uint64_t d=rng(),m=rng();ARMPACKey k={rng(),rng()};if(reference_pauth_computepac(d,m,k)!=optimized_pauth_computepac(d,m,k)){bridge_output("FAIL QARMA differential comparison\n");return 0;}}
 bridge_output("PASS QARMA: 4096 complete cipher comparisons against preserved pre-optimization reference\n");return 1;
}
