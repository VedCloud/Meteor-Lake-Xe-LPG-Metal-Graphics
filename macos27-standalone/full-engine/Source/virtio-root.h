/* SPDX-License-Identifier: MIT */
#ifndef DELL27_VIRTIO_ROOT_H
#define DELL27_VIRTIO_ROOT_H
#include "sparse-ram.h"
typedef int (*VirtioBackingTransfer)(void*,uint64_t,void*,unsigned,bool);
typedef struct VirtioRoot {
 SparseRam*ram; uint64_t image,bytes,bar,desc,avail,used,features;
 uint32_t feature_sel,driver_sel; uint16_t size,last_avail,used_idx,vector,config_vector;
 uint8_t pending;uint8_t status,isr,enabled,config[256],msix[32];
 uint64_t reads,failed,notifications;unsigned invalid;
 struct VirtioRoot*next;VirtioBackingTransfer transfer;void*opaque;
 uint64_t immutable_base,immutable_bytes,writes;uint32_t apple_type,irq;
} VirtioRoot;
uc_err virtio_root_map(uc_engine*,VirtioRoot*,SparseRam*,uint64_t,uint64_t);
uc_err virtio_aux_attach(VirtioRoot*,VirtioRoot*,uint64_t,VirtioBackingTransfer,void*);
uint64_t virtio_root_config_read(VirtioRoot*,unsigned,unsigned);
void virtio_root_config_write(VirtioRoot*,unsigned,unsigned,uint64_t);
#endif
