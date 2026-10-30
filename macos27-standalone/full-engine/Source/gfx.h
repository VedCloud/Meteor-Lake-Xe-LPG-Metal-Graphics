/* SPDX-License-Identifier: MIT */
#ifndef DELL27_GFX_H
#define DELL27_GFX_H
#include <unicorn/unicorn.h>
#include "sparse-ram.h"
typedef struct {SparseRam* ram; uc_engine* uc; uint64_t reads,writes; unsigned invalid;} GfxDevice;
uc_err gfx_map(uc_engine*,GfxDevice*,SparseRam*);
void gfx_close(GfxDevice*);
#endif