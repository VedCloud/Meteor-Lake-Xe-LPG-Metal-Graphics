//
//  IntelBTPatcher.h
//  IntelBTPatcher
//
//  Created by zxystd <zxystd@foxmail.com> on 2021/2/8.
//

#ifndef IntelBTPatcher_h
#define IntelBTPatcher_h

#include <Headers/kern_patcher.hpp>

#include <IOKit/usb/IOUSBHostDevice.h>
#include <IOKit/usb/IOUSBHostIOSource.h>

#define DRV_NAME "ibtp"

class BluetoothDeviceAddress;

typedef struct {
    void *owner;
    IOMemoryDescriptor *dataBuffer;
    IOUSBHostCompletionAction action;
    IOUSBHostBundledCompletionAction bundledAction;
} AsyncOwnerData;

typedef struct __attribute__((packed))
{
    uint16_t    opcode;    /* OCF & OGF */
    uint8_t     len;
    uint8_t     data[];
} HciCommandHdr;

typedef struct __attribute__((packed))
{
    uint8_t     evt;
    uint8_t     len;
    uint8_t     data[];
} HciEventHdr;

const char *requestDirectionNames[] = {
    "OUT",
    "IN"
};

const char *requestTypeNames[] = {
    "Standard",
    "Class",
    "Vendor"
};

const char *requestRecipientNames[] = {
    "Device",
    "Interface",
    "Endpoint",
    "Other"
};

const char* _hexDumpHCIData(uint8_t *buf, size_t len)
{
    ssize_t str_len = len * 3 + 1;
    char *str = (char*)IOMalloc(str_len);
    if (!str)
        return nullptr;
    for (size_t i = 0; i < len; i++)
    snprintf(str + 3 * i, (len - i) * 3, "%02x ", buf[i]);
    str[MAX(str_len - 2, 0)] = 0;
    return str;
}

class CIntelBTPatcher {
public:
    bool init();
    void free();
    
    void processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
    static IOReturn newFindQueueRequest(void *that, unsigned short arg1, void *addr, unsigned short arg2, bool arg3, void **hciRequestPtr);
    
    static IOReturn newHostDeviceRequest(void *that, IOService *provider, StandardUSB::DeviceRequest &request, void *data, IOMemoryDescriptor *descriptor, unsigned int &length,IOUSBHostCompletion *completion, unsigned int timeout);
    static IOReturn newAsyncIO(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, IOUSBHostCompletion* completion, uint32_t completionTimeoutMs);
    static IOReturn newIOSourceIOCompletion(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, IOUSBHostCompletion* completion, uint32_t completionTimeoutMs);
    static IOReturn newIOSourceIORjj(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, uint32_t &bytesTransferred, uint32_t completionTimeoutMs);
    static IOReturn newIOSourceIOBundled(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, IOUSBHostBundledCompletion* completion, uint32_t completionTimeoutMs);
    static int newInitPipe(void *that, StandardUSB::EndpointDescriptor const *descriptor, StandardUSB::SuperSpeedEndpointCompanionDescriptor const *superDescriptor,AppleUSBHostController *controller, IOUSBHostDevice *device, IOUSBHostInterface *interface, unsigned char, unsigned short);
    static int newInitPipeConfig(void *that, StandardUSB::EndpointDescriptor const *descriptor, StandardUSB::ConfigurationDescriptor const *configDescriptor, AppleUSBHostController *controller, IOUSBHostDevice *device, IOUSBHostInterface *interface, unsigned char, unsigned short);

    
    static bool newInitSource(void* that, AppleUSBHostController* controller, IOUSBHostDevice* device);
    static void newFreeSource(void* that);
    mach_vm_address_t oldInitSource {}, oldFreeSource {};
    mach_vm_address_t oldFindQueueRequest {};
    mach_vm_address_t oldHostDeviceRequest {};
    mach_vm_address_t oldAsyncIO {};
    mach_vm_address_t oldIOSourceIOCompletion {};
    mach_vm_address_t oldIOSourceIORjj {};
    mach_vm_address_t oldIOSourceIOBundled {};
    mach_vm_address_t oldInitPipe {};
    mach_vm_address_t oldInitPipeConfig {};
    
private:
    static void captureIntelInterruptPipe(void *that, StandardUSB::EndpointDescriptor const *descriptor, IOUSBHostDevice *device);
    static bool isIntelSource(void *that);
    static void *_hookPipeInstance;
    static void *_hookIntelDevice;
    static bool _iosSourceCompletionRouted;
    static AsyncOwnerData *_interruptPipeAsyncOwner;
    static bool _randomAddressInit;
};

#endif /* IntelBTPatcher_h */
