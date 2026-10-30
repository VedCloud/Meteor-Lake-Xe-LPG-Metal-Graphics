/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_AES_H
#define DELL27_AES_H
#include "sparse-ram.h"
typedef struct {unsigned bytes,mode,encrypt;unsigned char key[32];} GuestAesKey;
typedef struct {SparseRam*ram;uint32_t fifo[128],count,running,status,irq_status,irq_enable,watermark,flag,key_disable;GuestAesKey key[2];unsigned char iv[4][16],buffer[65536];unsigned bad_access,irq_asserted;uint64_t operations,rejected;} VmappleAes;
uc_err vmapple_aes_map(uc_engine*,VmappleAes*,SparseRam*);
#endif
