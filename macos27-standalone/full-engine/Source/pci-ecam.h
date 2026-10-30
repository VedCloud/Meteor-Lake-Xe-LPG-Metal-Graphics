/* SPDX-License-Identifier: MIT */
#ifndef DELL27_PCI_ECAM_H
#define DELL27_PCI_ECAM_H
#include <unicorn/unicorn.h>
#include <stdint.h>
#include "virtio-root.h"
typedef struct {VirtioRoot*root;VirtioRoot*aux;uint8_t config[256];uint64_t reads,writes;unsigned invalid;} PciEcam;
uc_err pci_ecam_map(uc_engine*,PciEcam*);
#endif
