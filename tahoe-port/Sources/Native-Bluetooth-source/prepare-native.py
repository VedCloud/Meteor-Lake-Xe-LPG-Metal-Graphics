from pathlib import Path
b=Path(__file__).resolve().parent;f=b/'IntelBTPatcher.cpp';s=(b/'IntelBTPatcher-reference.cpp').read_text();s=s.replace('#include "IntelBTPatcher.hpp"','#include "IntelBTPatcher.hpp"\n#include "NativeLEHandshake.hpp"',1)
insert='''
static IOSimpleLock* nativeLock = nullptr;
static NativeLEHandshake nativeLE;
static IOMemoryDescriptor* nativeInflight[64] = {};
static UInt64 nativeNow(){UInt64 t=0,n=0;clock_get_uptime(&t);absolutetime_to_nanoseconds(t,&n);return n;}
static bool nativePending(){if(!nativeLock)return false;auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);bool p=nativeLE.pending(nativeNow());IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);return p;}
static bool nativeBegin(unsigned h){if(!nativeLock)return false;auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);bool p=nativeLE.begin(h,nativeNow());IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);return p;}
static void nativeReset(){if(!nativeLock)return;auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);nativeLE.reset();IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);}
static bool nativeClaim(IOMemoryDescriptor* p){
 if(!nativeLock||!p)return false;
 auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);bool result=false;
 for(auto f:nativeInflight)if(f==p){IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);return false;}
 for(auto& f:nativeInflight)if(!f){f=p;result=true;break;}
 IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);return result;
}
static void nativeRelease(IOMemoryDescriptor* p){if(!nativeLock)return;auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);for(auto& f:nativeInflight)if(f==p){f=nullptr;break;}IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);}
'''
s=s.replace('static CIntelBTPatcher ibtPatcher;',insert+'\nstatic CIntelBTPatcher ibtPatcher;',1)
s=s.replace('    callbackIBTPatcher = this;','    callbackIBTPatcher = this;\n    nativeLock = IOSimpleLockAlloc();\n    if(!nativeLock) SYSLOG(DRV_NAME,"native LE workaround disabled: no state lock");',1)
s=s.replace('int remoteReadSCount = 0;','')
s=s.replace('static int leFeaturesEventStage = 0;','')
a=s.index('            remoteReadSCount += 2;');z=s.index('\n        }',a);s=s[:a]+'''            if(hdrLen >= 5 && hdr->len == 2 && that == _hookIntelDevice &&
               nativeBegin(unsigned(hdr->data[0]) | (unsigned(hdr->data[1]) << 8))) {
                SYSLOG(DRV_NAME,"native LE handshake armed");
                FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, request, nullptr, descriptor, length, nullptr, timeout);
                FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that, provider, request, nullptr, descriptor, length, nullptr, timeout);
            }'''+s[z:]
s=s.replace('            leFeaturesEventStage = 0;','            nativeReset();')
# The reference uses hdrLen as descriptor size, including the header on this path.
a=s.index('uint8_t fakePhyUpdateCompleteEvent[');z=s.index('static void asyncIOCompletion(',a)
s=s[:a]+'''static void rewriteHciEvent(IOMemoryDescriptor *dataBuffer, uint32_t &bytesTransferred)
{
    if(!nativeLock||!dataBuffer||bytesTransferred < 2||bytesTransferred > 64||
       bytesTransferred > dataBuffer->getLength())return;
    if(dataBuffer->prepare(kIODirectionInOut)!=kIOReturnSuccess)return;
    unsigned char packet[64]={};
    if(dataBuffer->readBytes(0,packet,bytesTransferred)==bytesTransferred){
        auto irq=IOSimpleLockLockDisableInterrupt(nativeLock);
        unsigned rewritten=nativeLE.event(packet,bytesTransferred,sizeof(packet),nativeNow());
        IOSimpleLockUnlockEnableInterrupt(nativeLock,irq);
        if(rewritten != bytesTransferred && dataBuffer->writeBytes(0,packet,rewritten)==rewritten){
            bytesTransferred=rewritten;
            SYSLOG(DRV_NAME,"native LE compatibility event delivered, subevent=%u",packet[2]);
        }
    }
    dataBuffer->complete(kIODirectionInOut);
}

'''+s[z:]
s=s.replace('    rewriteHciEvent(asyncOwner->dataBuffer, bytesTransferred);','    if(status == kIOReturnSuccess) rewriteHciEvent(asyncOwner->dataBuffer, bytesTransferred);\n    nativeRelease(asyncOwner->dataBuffer);\n    asyncOwner->dataBuffer->release();',1)
s=s.replace('            rewriteHciEvent(dataBufferArray[i], bytesTransferred);','            if(statusArray && statusArray[i] == kIOReturnSuccess) rewriteHciEvent(dataBufferArray[i], bytesTransferred);',1)
s=s.replace('    delete asyncOwner;\n}\n\nIOReturn CIntelBTPatcher::\nnewAsyncIO', '    nativeRelease(asyncOwner->dataBuffer);\n    asyncOwner->dataBuffer->release();\n    delete asyncOwner;\n}\n\nIOReturn CIntelBTPatcher::\nnewAsyncIO',1)
# Do not combine the old pipe wrapper and new source wrappers.
a=s.index('    if (!_iosSourceCompletionRouted');z=s.index('    return FunctionCast(newAsyncIO',a);s=s[:a]+s[z:]
s=s.replace('    // Behavioral bisection (doc 4.4 step 1): no isIntelSource filter.','    // Restrict interception to the captured Intel Bluetooth interrupt pipe.')
s=s.replace('if (remoteReadSCount > 0 && completion) {','if (isIntelSource(that) && nativePending() && completion && completion->action && nativeClaim(dataBuffer)) {')
s=s.replace('        AsyncOwnerData *ownerData = new AsyncOwnerData;','''        AsyncOwnerData *ownerData = new AsyncOwnerData;
        if(!ownerData){nativeRelease(dataBuffer);return kIOReturnNoMemory;}
        dataBuffer->retain();''')
s=s.replace('        if (ret != kIOReturnSuccess)\n            delete ownerData;','        if (ret != kIOReturnSuccess) {nativeRelease(dataBuffer);dataBuffer->release();delete ownerData;}')
a=s.index('    IOReturn ret = FunctionCast(newIOSourceIORjj');z=s.index('    return ret;',a);s=s[:a]+'''    bool claimed=isIntelSource(that) && nativePending() && nativeClaim(dataBuffer);
    IOReturn ret = FunctionCast(newIOSourceIORjj, callbackIBTPatcher->oldIOSourceIORjj)(that, dataBuffer, dataBufferLength, bytesTransferred, completionTimeoutMs);
    if(claimed){
        if(ret == kIOReturnSuccess)rewriteHciEvent(dataBuffer,bytesTransferred);
        nativeRelease(dataBuffer);
    }
'''+s[z:]
# No guessed IOUSBHostIOSource object offsets. Pointer identity is supplied by
# the real pipe constructor hook, and a nonmatching source is passed untouched.
a=s.index('    return (_hookIntelDevice');z=s.index(';',a)+1;s=s[:a]+'    return _hookPipeInstance && that == _hookPipeInstance;'+s[z:]
s=s.replace('deviceDescriptor->idVendor == VENDOR_USB_INTEL','deviceDescriptor->idVendor == VENDOR_USB_INTEL && deviceDescriptor->idProduct == 0x0033')
s=s.replace('if (epType == kIOUSBEndpointTypeInterrupt)','if (epType == kIOUSBEndpointTypeInterrupt && (descriptor->bEndpointAddress & 0x80))')
# Parsing is always bounded; null/short descriptor command buffers pass through.
s=s.replace('        hdr = (HciCommandHdr *)hciBuf;','        hdr = (HciCommandHdr *)hciBuf;\n        if(hdrLen < 3 || request.wLength > hdrLen || hdr->len + 3U > hdrLen)\n            return FunctionCast(newHostDeviceRequest, callbackIBTPatcher->oldHostDeviceRequest)(that,provider,request,data,descriptor,length,completion,timeout);')
s=s.replace('if (hdr->opcode == HCI_OP_LE_SET_SCAN_PARAM && hdr->data[5] == 0x1)','if (hdr->opcode == HCI_OP_LE_SET_SCAN_PARAM && hdrLen >= 9 && hdr->len >= 6 && hdr->data[5] == 0x1)')
f.write_text(s);print('Bounded native BLE candidate source prepared; no private object offsets or global per-link stage')
