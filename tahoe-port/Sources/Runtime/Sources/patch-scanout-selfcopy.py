from pathlib import Path
import shutil
b=Path(__file__).resolve().parent;s=b/'Graphics-source/NVRM';f=s/'accel/nvrm-accel.cpp';original=f.read_text();snap=b/'Performance-before-0.3.1';snap.mkdir(exist_ok=True);shutil.copy2(f,snap/'nvrm-accel.cpp');shutil.copytree(b/'Tahoe-Combined-Runtime/Library/Extensions/NMXAccel.kext',snap/'NMXAccel.kext',dirs_exist_ok=True)
h=s/'accel/nmx/latitude_scanout_identity.h';h.write_text('''#pragma once
#include <stdint.h>
// The same framebuffer bytes and row layout need no copy. Distinct mappings
// count as aliases only for a framebuffer-owned resource with a known physical address.
static inline bool latitudeScanoutCopyNoop(const volatile void *source,const volatile void *target,
                                          bool scanout,uint64_t physical,uint64_t targetPhysical,
                                          uint32_t pitch,uint32_t targetPitch) {
 if(!source||!target||!pitch||pitch!=targetPitch)return false;
 return source==target || (scanout && physical && physical==targetPhysical);
}
''')
t=original.replace('#include "nmx/latitude_front_copy.h"','#include "nmx/latitude_front_copy.h"\n#include "nmx/latitude_scanout_identity.h"')
needle='static bool gSysctlInstalled = false;';t=t.replace(needle,'''#ifdef NMX_ACCEL
NVACCEL_CNT(nmxfb_swap_self_skipped, gNmxSwapSelfSkipped, "Legacy swaps that already name the live scanout; copy skipped");
NVACCEL_CNT(nmxfb_swap_copies, gNmxSwapCopies, "Legacy distinct-source framebuffer copies");
NVACCEL_CNT(nmxfb_swap_copy_ns, gNmxSwapCopyNs, "Legacy framebuffer copy time in nanoseconds");
#endif
'''+needle,1)
t=t.replace('    sysctl_register_oid(&sysctl__debug_nmxaccel_headless);','    sysctl_register_oid(&sysctl__debug_nmxaccel_headless);\n    sysctl_register_oid(&sysctl__debug_nmxfb_swap_self_skipped);\n    sysctl_register_oid(&sysctl__debug_nmxfb_swap_copies);\n    sysctl_register_oid(&sysctl__debug_nmxfb_swap_copy_ns);',1)
start=t.index('static bool nvAccelCopyToScanout(IOAccelResource2 *res)\n{');end=t.index('\nstatic bool nvAccelFlipTo(',start);body=t[start:end]
needle='    const unsigned rows = (h < q.height) ? h : q.height;';body=body.replace(needle,'''#ifdef NMX_ACCEL
    // NvGfx7f9e464 found this read-back onto the same WC scanout can cost ~0.8s.
    // Require an identical row layout and identity, rather than only the scanout flag.
    if(latitudeScanoutCopyNoop(vm->fKva,q.kva,vm->fScanout,vm->fPhys,q.phys,pitch,q.pitch)){
        OSAddAtomic64(1,&gNmxSwapSelfSkipped);return true;
    }
    const uint64_t copyStart=mach_absolute_time();
#endif
'''+needle,1)
startloop=body.index('    for (unsigned y = 0; y < rows; y++) {');endloop=body.index('    nvScanoutCopies++;',startloop)
oldloop=body[startloop:endloop];body=body[:startloop]+'''#ifdef NMX_ACCEL
    if(!latitudeFrontCopy(d,s,rows,rowb,q.pitch,pitch))return false;
    uint64_t copyNs=0;absolutetime_to_nanoseconds(mach_absolute_time()-copyStart,&copyNs);
    OSAddAtomic64(1,&gNmxSwapCopies);OSAddAtomic64((SInt64)copyNs,&gNmxSwapCopyNs);
#else
'''+oldloop+'''#endif
'''+body[endloop:]
t=t[:start]+body+t[end:];assert 'com.nullmoth.NMXAccel, "0.3.0"' in t;t=t.replace('com.nullmoth.NMXAccel, "0.3.0"','com.nullmoth.NMXAccel, "0.3.1"');f.write_text(t)
print('Ported the legacy self-copy fix, optimized distinct copies and added counters; fresh command pools unchanged.')
