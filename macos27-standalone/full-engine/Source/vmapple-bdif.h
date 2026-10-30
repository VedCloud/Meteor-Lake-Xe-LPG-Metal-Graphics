/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_BDIF_H
#define DELL27_BDIF_H
#include "sparse-ram.h"
/* QEMU VMApple BDIF ABI. Backend offsets address a private guest utility file,
 * never a physical host disk. No root device is advertised without a backend. */
typedef int (*BdifTransfer)(void*,uint64_t,void*,unsigned,bool);
typedef struct {SparseRam*ram;BdifTransfer transfer;void*opaque;uint64_t bytes,reads,writes,failed;unsigned bad_access;unsigned char buffer[65536];} VmappleBdif;
uc_err vmapple_bdif_map(uc_engine*,VmappleBdif*,SparseRam*,uint64_t,BdifTransfer,void*);
#endif
