/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_CONFIG_H
#define DELL27_CONFIG_H
#include "sparse-ram.h"
typedef struct {SparseRam*ram;uint64_t unavailable,invalid;} VmappleConfigDma;
uc_err vmapple_config_seed(uc_engine*,const unsigned char*,unsigned);
uc_err vmapple_config_dma_map(uc_engine*,VmappleConfigDma*,SparseRam*);
#endif
