#pragma once
#include <stdint.h>
// The same framebuffer bytes and row layout need no copy. Distinct mappings
// count as aliases only for a framebuffer-owned resource with a known physical address.
static inline bool latitudeScanoutCopyNoop(const volatile void *source,const volatile void *target,
                                          bool scanout,uint64_t physical,uint64_t targetPhysical,
                                          uint32_t pitch,uint32_t targetPitch) {
 if(!source||!target||!pitch||pitch!=targetPitch)return false;
 return source==target || (scanout && physical && physical==targetPhysical);
}
