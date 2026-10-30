/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef DELL27_IOSFC_H
#define DELL27_IOSFC_H
#include "sparse-ram.h"
typedef struct {uint64_t descriptor;uint64_t*pages;uint32_t page_count,bytes;uint8_t metadata[512];} IosfcSurface;
typedef struct {SparseRam*ram;uint64_t ring_base,ring_size,table,wr,rd,maps,unmaps,commands;IosfcSurface*slot[8192];unsigned invalid,ring_mapped;uint8_t scratch[16384];} IosfcDevice;
uc_err iosfc_map(uc_engine*,IosfcDevice*,SparseRam*);
void iosfc_close(IosfcDevice*);
int iosfc_copy(IosfcDevice*,unsigned,uint64_t,void*,unsigned,int);
#endif
