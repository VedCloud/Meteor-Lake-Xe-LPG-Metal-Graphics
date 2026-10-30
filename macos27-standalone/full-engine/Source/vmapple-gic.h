/* SPDX-License-Identifier: MIT */
#ifndef DELL27_GIC_BRIDGE_H
#define DELL27_GIC_BRIDGE_H
#include <unicorn/unicorn.h>
typedef struct VmappleGic VmappleGic;
typedef struct {VmappleGic*owner;unsigned region;} GicRegion;
struct VmappleGic {GicRegion context[3];unsigned bad_access;uint64_t accesses;};
uc_err vmapple_gic_map(uc_engine*,VmappleGic*);
#endif
