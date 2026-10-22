#include "VoodooI2CPCIController.hpp"
#include "latitude_i2c_power.h"
#include <mach/kmod.h>
extern "C" kern_return_t _start(kmod_info_t*,void*);
extern "C" kern_return_t _stop(kmod_info_t*,void*);
extern "C" { KMOD_EXPLICIT_DECL(com.ved.LatitudeI2CMeteorLake,"1.0.0",_start,_stop) }
extern "C" kern_return_t _start(kmod_info_t*,void*) {return KERN_SUCCESS;}
extern "C" kern_return_t _stop(kmod_info_t*,void*) {return KERN_SUCCESS;}
class LatitudeI2CMeteorLakeController:public VoodooI2CPCIController {
    OSDeclareDefaultStructors(LatitudeI2CMeteorLakeController)
public:
    VoodooI2CController *probe(IOService *provider,SInt32 *score) override {
        auto pci=OSDynamicCast(IOPCIDevice,provider);
        if(!latitude_i2c_d0(pci,[]{IOSleep(1);}))return nullptr;
        return VoodooI2CPCIController::probe(provider,score);
    }
protected:
    void configurePCI() override {
        bool ok=latitude_i2c_d0(physical_device.pci_device,[]{IOSleep(1);});
        setProperty("LatitudeI2CD0Verified",ok?kOSBooleanTrue:kOSBooleanFalse);
        IOLog("LatitudeI2CMeteorLake: controller 8086:7e7b D0 verified=%d\n",ok);
    }
};
OSDefineMetaClassAndStructors(LatitudeI2CMeteorLakeController,VoodooI2CPCIController)
