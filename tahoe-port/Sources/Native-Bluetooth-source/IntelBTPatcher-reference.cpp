//
//  IntelBTPatcher.cpp
//  IntelBTPatcher
//
//  Created by zxystd <zxystd@foxmail.com> on 2021/2/8.
//
#define __ACIDANTHERA_MAC_SDK 2600
#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/plugin_start.hpp>

#include "IntelBTPatcher.hpp"

static CIntelBTPatcher ibtPatcher;
static CIntelBTPatcher *callbackIBTPatcher = nullptr;

static const char *bootargOff[] {
    "-ibtcompatoff"
};

static const char *bootargDebug[] {
    "-ibtcompatdbg"
};

static const char *bootargBeta[] {
    "-ibtcompatbeta"
};

PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery | LiluAPI::AllowSafeMode,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::MountainLion,
    KernelVersion::Tahoe,
    []() {
        ibtPatcher.init();
    }
};

static const char *IntelBTPatcher_IOBluetoothFamily[] { "/System/Library/Extensions/IOBluetoothFamily.kext/Contents/MacOS/IOBluetoothFamily" };

static KernelPatcher::KextInfo IntelBTPatcher_IOBluetoothInfo {
    "com.apple.iokit.IOBluetoothFamily",
    IntelBTPatcher_IOBluetoothFamily,
    1,
    {true, true},
    {},
    KernelPatcher::KextInfo::Unloaded
};

static const char *IntelBTPatcher_IOUSBHostFamily[] {
    "/System/Library/Extensions/IOUSBHostFamily.kext/Contents/MacOS/IOUSBHostFamily" };

static KernelPatcher::KextInfo IntelBTPatcher_IOUsbHostInfo {
    "com.apple.iokit.IOUSBHostFamily",
    IntelBTPatcher_IOUSBHostFamily,
    1,
    {true, true},
    {},
    KernelPatcher::KextInfo::Unloaded
};

void *CIntelBTPatcher::_hookPipeInstance = nullptr;
void *CIntelBTPatcher::_hookIntelDevice = nullptr;
bool CIntelBTPatcher::_iosSourceCompletionRouted = false;
bool CIntelBTPatcher::_randomAddressInit = false;

bool CIntelBTPatcher::init()
{
    DBGLOG(DRV_NAME, "%s", __PRETTY_FUNCTION__);
    callbackIBTPatcher = this;
    if (getKernelVersion() < KernelVersion::Monterey) {
        lilu.onKextLoadForce(&IntelBTPatcher_IOBluetoothInfo, 1,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            callbackIBTPatcher->processKext(patcher, index, address, size);
        }, this);
    } else {
        lilu.onKextLoadForce(&IntelBTPatcher_IOUsbHostInfo, 1,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            callbackIBTPatcher->processKext(patcher, index, address, size);
        }, this);
    }
    return true;
}

void CIntelBTPatcher::free()
{
    DBGLOG(DRV_NAME, "%s", __PRETTY_FUNCTION__);
}

void CIntelBTPatcher::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size)
{
    DBGLOG(DRV_NAME, "%s", __PRETTY_FUNCTION__);
    if (getKernelVersion() < KernelVersion::Monterey) {
        if (IntelBTPatcher_IOBluetoothInfo.loadIndex == index) {
            DBGLOG(DRV_NAME, "%s", IntelBTPatcher_IOBluetoothInfo.id);
            
            KernelPatcher::RouteRequest findQueueRequestRequest {
                "__ZN25IOBluetoothHostController17FindQueuedRequestEtP22BluetoothDeviceAddresstbPP21IOBluetoothHCIRequest",
                newFindQueueRequest,
                oldFindQueueRequest
            };
            patcher.routeMultiple(index, &findQueueRequestRequest, 1, address, size);
            if (patcher.getError() == KernelPatcher::Error::NoError) {
                DBGLOG(DRV_NAME, "routed %s", findQueueRequestRequest.symbol);
            } else {
                SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", findQueueRequestRequest.symbol, patcher.getError());
                patcher.clearError();
            }
            
        }
    } else {
        if (IntelBTPatcher_IOUsbHostInfo.loadIndex == index) {
            SYSLOG(DRV_NAME, "%s", IntelBTPatcher_IOUsbHostInfo.id);
            
            KernelPatcher::RouteRequest hostDeviceRequest {
            "__ZN15IOUSBHostDevice13deviceRequestEP9IOServiceRN11StandardUSB13DeviceRequestEPvP18IOMemoryDescriptorRjP19IOUSBHostCompletionj",
                newHostDeviceRequest,
                oldHostDeviceRequest
            };
            patcher.routeMultiple(index, &hostDeviceRequest, 1, address, size);
            if (patcher.getError() == KernelPatcher::Error::NoError) {
                SYSLOG(DRV_NAME, "routed %s", hostDeviceRequest.symbol);
            } else {
                SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", hostDeviceRequest.symbol, patcher.getError());
                patcher.clearError();
            }

            KernelPatcher::RouteRequest asyncIORequest {
            "__ZN13IOUSBHostPipe2ioEP18IOMemoryDescriptorjP19IOUSBHostCompletionj",
                newAsyncIO,
                oldAsyncIO
            };
            patcher.routeMultiple(index, &asyncIORequest, 1, address, size);
            if (patcher.getError() == KernelPatcher::Error::NoError) {
                SYSLOG(DRV_NAME, "routed %s", asyncIORequest.symbol);
            } else {
                SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", asyncIORequest.symbol, patcher.getError());
                patcher.clearError();
            }

            // Tahoe: pipe/stream io tail-jumps into one of three IOSource::io
            // overloads (Completion, Rjj, Bundled), so cover all of them.
            if (getKernelVersion() >= KernelVersion::Tahoe) {
                KernelPatcher::RouteRequest iosourceIOCompletionRequest {
                "__ZN17IOUSBHostIOSource2ioEP18IOMemoryDescriptorjP19IOUSBHostCompletionj",
                    newIOSourceIOCompletion,
                    oldIOSourceIOCompletion
                };
                patcher.routeMultiple(index, &iosourceIOCompletionRequest, 1, address, size);
                if (patcher.getError() == KernelPatcher::Error::NoError) {
                    SYSLOG(DRV_NAME, "routed %s", iosourceIOCompletionRequest.symbol);
                    _iosSourceCompletionRouted = true;
                } else {
                    SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", iosourceIOCompletionRequest.symbol, patcher.getError());
                    patcher.clearError();
                }

                KernelPatcher::RouteRequest iosourceIORjjRequest {
                "__ZN17IOUSBHostIOSource2ioEP18IOMemoryDescriptorjRjj",
                    newIOSourceIORjj,
                    oldIOSourceIORjj
                };
                patcher.routeMultiple(index, &iosourceIORjjRequest, 1, address, size);
                if (patcher.getError() == KernelPatcher::Error::NoError) {
                    SYSLOG(DRV_NAME, "routed %s", iosourceIORjjRequest.symbol);
                } else {
                    SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", iosourceIORjjRequest.symbol, patcher.getError());
                    patcher.clearError();
                }

                KernelPatcher::RouteRequest iosourceIOBundledRequest {
                "__ZN17IOUSBHostIOSource2ioEP18IOMemoryDescriptorjP26IOUSBHostBundledCompletionj",
                    newIOSourceIOBundled,
                    oldIOSourceIOBundled
                };
                patcher.routeMultiple(index, &iosourceIOBundledRequest, 1, address, size);
                if (patcher.getError() == KernelPatcher::Error::NoError) {
                    SYSLOG(DRV_NAME, "routed %s", iosourceIOBundledRequest.symbol);
                } else {
                    SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", iosourceIOBundledRequest.symbol, patcher.getError());
                    patcher.clearError();
                }
            }

            // macOS 26.5+: third parameter became ConfigurationDescriptor (was SuperSpeedEndpointCompanionDescriptor)
            KernelPatcher::RouteRequest initPipeConfigRequest {
            "__ZN13IOUSBHostPipe28initWithDescriptorsAndOwnersEPKN11StandardUSB18EndpointDescriptorEPKNS0_23ConfigurationDescriptorEP22AppleUSBHostControllerP15IOUSBHostDeviceP18IOUSBHostInterfaceht",
                newInitPipeConfig,
                oldInitPipeConfig
            };
            patcher.routeMultiple(index, &initPipeConfigRequest, 1, address, size);
            if (patcher.getError() == KernelPatcher::Error::NoError) {
                SYSLOG(DRV_NAME, "routed %s", initPipeConfigRequest.symbol);
            } else {
                SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", initPipeConfigRequest.symbol, patcher.getError());
                patcher.clearError();
            }

            KernelPatcher::RouteRequest initPipeRequest {
            "__ZN13IOUSBHostPipe28initWithDescriptorsAndOwnersEPKN11StandardUSB18EndpointDescriptorEPKNS0_37SuperSpeedEndpointCompanionDescriptorEP22AppleUSBHostControllerP15IOUSBHostDeviceP18IOUSBHostInterfaceht",
                newInitPipe,
                oldInitPipe
            };
            patcher.routeMultiple(index, &initPipeRequest, 1, address, size);
            if (patcher.getError() == KernelPatcher::Error::NoError) {
                SYSLOG(DRV_NAME, "routed %s", initPipeRequest.symbol);
            } else {
                SYSLOG(DRV_NAME, "failed to resolve %s, error = %d", initPipeRequest.symbol, patcher.getError());
                patcher.clearError();
            }
        }
    }
}

#pragma mark - For Bigsur-, patch unhandled 0x2019 opcode

#define HCI_OP_LE_START_ENCRYPTION 0x2019

IOReturn CIntelBTPatcher::newFindQueueRequest(void *that, unsigned short arg1, void *addr, unsigned short arg2, bool arg3, void **hciRequestPtr)
{
    IOReturn ret = FunctionCast(newFindQueueRequest, callbackIBTPatcher->oldFindQueueRequest)(that, arg1, addr, arg2, arg3, hciRequestPtr);
    if (ret != 0 && arg1 == HCI_OP_LE_START_ENCRYPTION) {
        ret = FunctionCast(newFindQueueRequest, callbackIBTPatcher->oldFindQueueRequest)(that, arg1, addr, 0xffff, arg3, hciRequestPtr);
        DBGLOG(DRV_NAME, "%s ret: %d arg1: 0x%04x arg2: 0x%04x arg3: %d ptr: %p", __FUNCTION__, ret, arg1, arg2, arg3, *hciRequestPtr);
    }
    return ret;
}

#pragma mark - For Monterey+ patch for intercept HCI REQ and RESP

StandardUSB::DeviceRequest randomAddressRequest;
// Hardcoded Random Address HCI Command
const uint8_t randomAddressHci[9] = {0x05, 0x20, 0x06, 0x94, 0x50, 0x64, 0xD0, 0x78, 0x6B}; 
IOBufferMemoryDescriptor *writeHCIDescriptor = nullptr;

#define MAX_HCI_BUF_LEN                 255
#define HCI_OP_RESET                    0x0c03
#define HCI_OP_LE_SET_SCAN_PARAM        0x200B
#define HCI_OP_LE_SET_SCAN_ENABLE       0x200C
#define HCI_OP_LE_READ_REMOTE_FEATURES  0x2016

int remoteReadSCount = 0;
// 0 = pass first LE Meta 0x04, 1 = rewrite second as PHY, 2 = rewrite third as Data Length Change
static int leFeaturesEventStage = 0;
IOReturn CIntelBTPatcher::newHostDeviceRequest(void *that, IOService *provider, StandardUSB::DeviceRequest &request, void *data, IOMemoryDescriptor *descriptor, unsigned int &length, IOUSBHostCompletion *completion, unsigned int timeout)
{
    HciCommandHdr *hdr = nullptr;
    uint32_t hdrLen = 0;
    char hciBuf[MAX_HCI_BUF_LEN] = {0};
    
    if (data == nullptr) {
        if (descriptor != nullptr &&
            (getKernelVersion() < KernelVersion::Sequoia || !descriptor->prepare(kIODirectionOut))) {
            if (descriptor->getLength() > 0) {
                descriptor->readBytes(0, hciBuf, min(descriptor->getLength(), MAX_HCI_BUF_LEN));
                hdrLen = (uint32_t)min(descriptor->getLength(), MAX_HCI_BUF_LEN);
            }
            if (getKernelVersion() >= KernelVersion::Sequoia)
                descriptor->complete(kIODirectionOut);
        }
        hdr = (HciCommandHdr *)hciBuf;
        if (hdr->opcode == HCI_OP_LE_SET_SCAN_PARAM && hdr->data[5] == 0x1) {
            if (!_randomAddressInit) {
                randomAddressRequest.bmRequestType = makeDeviceRequestbmRequestType(kRequestDirectionOut, kRequestTypeClass, kRequestRecipientInterface);
                randomAddressRequest.bRequest = 0xE0;
                randomAddressRequest.wIndex = 0;
                randomAddressRequest.wValue = 0;
                randomAddressRequest.wLength = 9;
                length = 9;
                if (writeHCIDescriptor == nullptr)
                    writeHCIDescriptor = IOBufferMemoryDescriptor::withBytes(randomAddressHci, 9, kIODirectionInOut);
                writeHCIDescriptor->prepare(kIODirectionInOut);
                IOReturn ret = FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, randomAddressRequest, nullptr, writeHCIDescriptor, length, nullptr, timeout);
                writeHCIDescriptor->complete(kIODirectionInOut);
                const char *randAddressDump = _hexDumpHCIData((uint8_t *)randomAddressHci, 9);
                if (randAddressDump) {
                    SYSLOG(DRV_NAME, "[PATCH] Sending Random Address HCI %d %s", ret, randAddressDump);
                    IOFree((void *)randAddressDump, 9 * 3 + 1);
                }
                _randomAddressInit = true;
                SYSLOG(DRV_NAME, "[PATCH] Resend LE SCAN PARAM HCI %d", ret);
            }
        } else if (hdr->opcode == HCI_OP_LE_READ_REMOTE_FEATURES) {
            remoteReadSCount += 2;
            leFeaturesEventStage = 0;
            FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, request, nullptr, descriptor, length, nullptr, timeout);
            FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, request, nullptr, descriptor, length, nullptr, timeout);
        }
    } else {
        hdr = (HciCommandHdr *)data;
        hdrLen = request.wLength - 3;
    }
    if (hdr) {
        // HCI reset, we need to send Random address again
        if (hdr->opcode == HCI_OP_RESET) {
            _randomAddressInit = false;
            leFeaturesEventStage = 0;
        }
#if DEBUG
        DBGLOG(DRV_NAME, "[%s] bRequest: 0x%x direction: %s type: %s recipient: %s wValue: 0x%02x wIndex: 0x%02x opcode: 0x%04x len: %d length: %d async: %d", provider->getName(), request.bRequest, requestDirectionNames[(request.bmRequestType & kDeviceRequestDirectionMask) >> kDeviceRequestDirectionPhase], requestRecipientNames[(request.bmRequestType & kDeviceRequestRecipientMask) >> kDeviceRequestRecipientPhase], requestTypeNames[(request.bmRequestType & kDeviceRequestTypeMask) >> kDeviceRequestTypePhase], request.wValue, request.wIndex, hdr->opcode, hdr->len, request.wLength, completion != nullptr);
        if (hdrLen) {
            const char *dump = _hexDumpHCIData((uint8_t *)hdr, hdrLen);
            if (dump) {
                DBGLOG(DRV_NAME, "[Request]: %s", dump);
                IOFree((void *)dump, hdrLen * 3 + 1);
            }
        }
#endif
    }
    return FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, request, data, descriptor, length, completion, timeout);
}

#define HCI_EVT_LE_META                               0x3E
#define HCI_EVT_LE_META_READ_REMOTE_FEATURES_COMPLETE 0x04

uint8_t fakePhyUpdateCompleteEvent[8] = {0x3E, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x02, 0x02};
uint8_t fakeDataLengthChangeEvent[13] = {0x3E, 0x0B, 0x07, 0x00, 0x00, 0x1B, 0x00, 0x48, 0x01, 0x1B, 0x00, 0x48, 0x01};

static void rewriteHciEvent(IOMemoryDescriptor *dataBuffer, uint32_t &bytesTransferred)
{
    if (dataBuffer && bytesTransferred) {
        uint32_t originalSize = bytesTransferred;
        void *buffer = IOMalloc(originalSize);
        if (buffer == nullptr)
            return;
        if (dataBuffer->getLength() > 0 && (getKernelVersion() < KernelVersion::Sequoia || !dataBuffer->prepare(kIODirectionInOut))) {
            dataBuffer->readBytes(0, buffer, originalSize);
            HciEventHdr *hdr = (HciEventHdr *)buffer;
            if (hdr->evt == HCI_EVT_LE_META && hdr->data[0] == HCI_EVT_LE_META_READ_REMOTE_FEATURES_COMPLETE) {
                if (leFeaturesEventStage == 0) {
                    leFeaturesEventStage = 1;
                } else if (leFeaturesEventStage == 1) {
                    fakePhyUpdateCompleteEvent[4] = hdr->data[2];
                    fakePhyUpdateCompleteEvent[5] = hdr->data[3];
                    dataBuffer->writeBytes(0, fakePhyUpdateCompleteEvent, sizeof(fakePhyUpdateCompleteEvent));
                    bytesTransferred = sizeof(fakePhyUpdateCompleteEvent);
                    leFeaturesEventStage = 2;
                    remoteReadSCount--;
                } else {
                    // Data Length Change has no status byte: handle starts at [3][4],
                    // unlike PHY Update Complete which has status at [3] and handle at [4][5].
                    fakeDataLengthChangeEvent[3] = hdr->data[2];
                    fakeDataLengthChangeEvent[4] = hdr->data[3];
                    dataBuffer->writeBytes(0, fakeDataLengthChangeEvent, sizeof(fakeDataLengthChangeEvent));
                    bytesTransferred = sizeof(fakeDataLengthChangeEvent);
                    leFeaturesEventStage = 0;
                    remoteReadSCount--;
                }
            }
            if (getKernelVersion() >= KernelVersion::Sequoia) dataBuffer->complete(kIODirectionInOut);
        }
        IOFree(buffer, originalSize);
    }
}

static void asyncIOCompletion(void* owner, void* parameter, IOReturn status, uint32_t bytesTransferred)
{
    AsyncOwnerData *asyncOwner = (AsyncOwnerData *)owner;
    rewriteHciEvent(asyncOwner->dataBuffer, bytesTransferred);
    if (asyncOwner->action)
        asyncOwner->action(asyncOwner->owner, parameter, status, bytesTransferred);
    delete asyncOwner;
}

static void asyncBundledCompletion(void* owner, uint32_t ioCompletionCount, IOMemoryDescriptor** dataBufferArray, void** parameter, IOReturn* statusArray, uint32_t* actualByteCountArray)
{
    AsyncOwnerData *asyncOwner = (AsyncOwnerData *)owner;
    if (asyncOwner->bundledAction && dataBufferArray && actualByteCountArray) {
        for (uint32_t i = 0; i < ioCompletionCount; i++) {
            uint32_t bytesTransferred = actualByteCountArray[i];
            rewriteHciEvent(dataBufferArray[i], bytesTransferred);
            actualByteCountArray[i] = bytesTransferred;
        }
        asyncOwner->bundledAction(asyncOwner->owner, ioCompletionCount, dataBufferArray, parameter, statusArray, actualByteCountArray);
    }
    delete asyncOwner;
}

IOReturn CIntelBTPatcher::
newAsyncIO(void *that, IOMemoryDescriptor* dataBuffer, uint32_t bytesTransferred, IOUSBHostCompletion* completion, uint32_t completionTimeoutMs)
{
    if (!_iosSourceCompletionRouted && remoteReadSCount > 0 && that == _hookPipeInstance && completion) {
        AsyncOwnerData *ownerData = new AsyncOwnerData;
        ownerData->action = completion->action;
        ownerData->owner = completion->owner;
        ownerData->dataBuffer = dataBuffer;
        completion->action = asyncIOCompletion;
        completion->owner = ownerData;
    }
    return FunctionCast(newAsyncIO, callbackIBTPatcher->oldAsyncIO)(that, dataBuffer, bytesTransferred, completion, completionTimeoutMs);
}

IOReturn CIntelBTPatcher::
newIOSourceIOCompletion(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, IOUSBHostCompletion* completion, uint32_t completionTimeoutMs)
{
    // Behavioral bisection (doc 4.4 step 1): no isIntelSource filter.
    if (remoteReadSCount > 0 && completion) {
        AsyncOwnerData *ownerData = new AsyncOwnerData;
        ownerData->action = completion->action;
        ownerData->owner = completion->owner;
        ownerData->dataBuffer = dataBuffer;
        IOUSBHostCompletion wrappedCompletion = *completion;
        wrappedCompletion.owner = ownerData;
        wrappedCompletion.action = asyncIOCompletion;
        IOReturn ret = FunctionCast(newIOSourceIOCompletion, callbackIBTPatcher->oldIOSourceIOCompletion)(that, dataBuffer, dataBufferLength, &wrappedCompletion, completionTimeoutMs);
        if (ret != kIOReturnSuccess)
            delete ownerData;
        return ret;
    }
    return FunctionCast(newIOSourceIOCompletion, callbackIBTPatcher->oldIOSourceIOCompletion)(that, dataBuffer, dataBufferLength, completion, completionTimeoutMs);
}

IOReturn CIntelBTPatcher::
newIOSourceIORjj(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, uint32_t &bytesTransferred, uint32_t completionTimeoutMs)
{
    IOReturn ret = FunctionCast(newIOSourceIORjj, callbackIBTPatcher->oldIOSourceIORjj)(that, dataBuffer, dataBufferLength, bytesTransferred, completionTimeoutMs);
    if (ret == kIOReturnSuccess && remoteReadSCount > 0)
        rewriteHciEvent(dataBuffer, bytesTransferred);
    return ret;
}

IOReturn CIntelBTPatcher::
newIOSourceIOBundled(void *that, IOMemoryDescriptor* dataBuffer, uint32_t dataBufferLength, IOUSBHostBundledCompletion* completion, uint32_t completionTimeoutMs)
{
    // Behavioral bisection (doc 4.4 step 1): no isIntelSource filter.
    if (remoteReadSCount > 0 && completion) {
        AsyncOwnerData *ownerData = new AsyncOwnerData;
        ownerData->bundledAction = completion->action;
        ownerData->owner = completion->owner;
        ownerData->dataBuffer = dataBuffer;
        IOUSBHostBundledCompletion wrappedCompletion = *completion;
        wrappedCompletion.owner = ownerData;
        wrappedCompletion.action = asyncBundledCompletion;
        IOReturn ret = FunctionCast(newIOSourceIOBundled, callbackIBTPatcher->oldIOSourceIOBundled)(that, dataBuffer, dataBufferLength, &wrappedCompletion, completionTimeoutMs);
        if (ret != kIOReturnSuccess)
            delete ownerData;
        return ret;
    }
    return FunctionCast(newIOSourceIOBundled, callbackIBTPatcher->oldIOSourceIOBundled)(that, dataBuffer, dataBufferLength, completion, completionTimeoutMs);
}

#define VENDOR_USB_INTEL 0x8087

void CIntelBTPatcher::captureIntelInterruptPipe(void *that, StandardUSB::EndpointDescriptor const *descriptor, IOUSBHostDevice *device)
{
    if (!device)
        return;
    const StandardUSB::DeviceDescriptor *deviceDescriptor = device->getDeviceDescriptor();
    if (deviceDescriptor && deviceDescriptor->idVendor == VENDOR_USB_INTEL) {
        CIntelBTPatcher::_hookIntelDevice = device;
        uint8_t epType = StandardUSB::getEndpointType(descriptor);
        if (epType == kIOUSBEndpointTypeInterrupt)
            CIntelBTPatcher::_hookPipeInstance = that;
    }
}

bool CIntelBTPatcher::isIntelSource(void *that)
{
    if (!that)
        return false;
    return (_hookIntelDevice && *(void **)((char *)that + 0x18) == _hookIntelDevice) ||
           (_hookPipeInstance && *(void **)((char *)that + 0x60) == _hookPipeInstance);
}

int CIntelBTPatcher::
newInitPipe(void *that, StandardUSB::EndpointDescriptor const *descriptor, StandardUSB::SuperSpeedEndpointCompanionDescriptor const *superDescriptor, AppleUSBHostController *controller, IOUSBHostDevice *device, IOUSBHostInterface *interface, unsigned char a7, unsigned short a8)
{
    int ret = FunctionCast(newInitPipe, callbackIBTPatcher->oldInitPipe)(that, descriptor, superDescriptor, controller, device, interface, a7, a8);
    captureIntelInterruptPipe(that, descriptor, device);
    return ret;
}

int CIntelBTPatcher::
newInitPipeConfig(void *that, StandardUSB::EndpointDescriptor const *descriptor, StandardUSB::ConfigurationDescriptor const *configDescriptor, AppleUSBHostController *controller, IOUSBHostDevice *device, IOUSBHostInterface *interface, unsigned char a7, unsigned short a8)
{
    int ret = FunctionCast(newInitPipeConfig, callbackIBTPatcher->oldInitPipeConfig)(that, descriptor, configDescriptor, controller, device, interface, a7, a8);
    captureIntelInterruptPipe(that, descriptor, device);
    return ret;
}
