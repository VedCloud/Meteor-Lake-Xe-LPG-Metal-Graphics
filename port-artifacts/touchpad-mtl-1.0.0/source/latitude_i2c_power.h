#pragma once
#include <stdint.h>
static inline bool latitude_i2c_pci_identity(uint16_t vendor,uint16_t device,
                                            uint16_t subvendor,uint16_t subsystem)
{
    return vendor==0x8086 && device==0x7e7b && subvendor==0x1028 && subsystem==0x0cb6;
}
template<class PCI,class Sleep>
static bool latitude_i2c_d0(PCI *pci,Sleep pause)
{
    if(!pci || !latitude_i2c_pci_identity(pci->configRead16(0),pci->configRead16(2),
                                         pci->configRead16(0x2c),pci->configRead16(0x2e)))return false;
    uint8_t cap=0;
    if(!pci->findPCICapability(1,&cap) || cap<0x40 || cap>0xf8 || (cap&3))return false;
    uint16_t old=pci->configRead16(cap+4);
    if(old==0xffff)return false;
    pci->enablePCIPowerManagement(0);
    if(old&3)pci->configWrite16(cap+4,(uint16_t)(old&~0x8003u));
    for(unsigned i=0;i<10;i++) {
        if((pci->configRead16(cap+4)&3)==0) {
            pci->setBusMasterEnable(true);pci->setMemoryEnable(true);return true;
        }
        pause();
    }
    return false;
}
