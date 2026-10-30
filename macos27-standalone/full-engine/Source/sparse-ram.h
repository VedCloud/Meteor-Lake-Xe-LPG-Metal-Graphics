/* SPDX-License-Identifier: MIT */
#ifndef DELL27_SPARSE_RAM_H
#define DELL27_SPARSE_RAM_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unicorn/unicorn.h>
#define RAM_BLOCK (32ULL*1024*1024)
#define RAM_MAX_BLOCKS 512
/* The ARM engine encodes physical section IDs in 10 bits. At most 512
 * RAM regions cover the complete 16 GiB range, reserving IDs for devices.
 * One canonical mapping per physical block; no virtual aliases or MMIO pages. */
typedef struct {uc_engine*uc;uint64_t base,size,limit,committed;uint64_t last_physical;uc_err last_error;uint8_t mapped[RAM_MAX_BLOCKS/8];} SparseRam;
uc_err sparse_ram_init(SparseRam*,uc_engine*,uint64_t,uint64_t,uint64_t);
uc_err sparse_ram_back(SparseRam*,uint64_t,size_t);
bool sparse_ram_fault(SparseRam*,uc_mem_type,uint64_t,int);
#endif
