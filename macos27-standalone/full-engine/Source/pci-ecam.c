/* SPDX-License-Identifier: MIT
 * GPEX-compatible virtual PCI host function and ECAM, without endpoints.
 * Configuration semantics cross-checked against QEMU hw/pci-host/gpex.c,
 * hw/pci/pcie_host.c and hw/pci/pci.c. No host PCI device is passed through.
 */
#include "glue.h"
#include "pci-ecam.h"
void bridge_output(const char*);
static int valid(uc_engine*u,PciEcam*d,uint64_t off,unsigned size){if((size!=1&&size!=2&&size!=4)||off%size||off>=0x10000000||size>0x10000000-off){d->invalid++;bridge_output("Unsupported PCI ECAM access\n");uc_emu_stop(u);return 0;}return 1;}
static uint64_t read_config(uc_engine*u,uint64_t off,unsigned size,void*opaque){PciEcam*d=opaque;d->reads++;if(!valid(u,d,off,size))return 0;if(d->aux&&off>=0x10000&&off<0x11000)return virtio_root_config_read(d->aux,off-0x10000,size);if(d->root&&off>=0x8000&&off<0x9000)return virtio_root_config_read(d->root,off-0x8000,size);uint64_t v=0;for(unsigned i=0;i<size;i++)v|=(uint64_t)(off+i<256?d->config[off+i]:0xff)<<(i*8);return v;}
static void write_config(uc_engine*u,uint64_t off,unsigned size,uint64_t value,void*opaque){PciEcam*d=opaque;d->writes++;if(!valid(u,d,off,size))return;if(d->aux&&off>=0x10000&&off<0x11000){virtio_root_config_write(d->aux,off-0x10000,size,value);return;}if(d->root&&off>=0x8000&&off<0x9000){virtio_root_config_write(d->root,off-0x8000,size,value);return;}/* Absent functions ignore config writes. */if(off>=256)return;for(unsigned i=0;i<size;i++){unsigned at=off+i;uint8_t mask=at==4?7:at==5?5:at==0xc||at==0x3c||at>=0x40?0xff:0;d->config[at]=(d->config[at]&~mask)|((value>>(i*8))&mask);}}
uc_err pci_ecam_map(uc_engine*u,PciEcam*d){if(!u||!d)return UC_ERR_ARG;memset(d,0,sizeof(*d));/* GPEX vendor/device, conventional header, host bridge class. */d->config[0]=0x36;d->config[1]=0x1b;d->config[2]=8;d->config[0xb]=6;return uc_mmio_map(u,0x40000000,0x10000000,read_config,d,write_config,d);}
