#ifdef LATITUDE_MTL_PANEL
IOReturn NMXVGAFB::setProperties(OSObject *properties)
{
    OSDictionary *outer = OSDynamicCast(OSDictionary, properties);
    if(outer && outer->getObject("LatitudeReadHotkeyCounters")){
        if(IOUserClient::clientHasPrivilege(current_task(),kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)return kIOReturnNotPrivileged;
        if(outer->getObject("LatitudeReadHotkeyCounters")!=kOSBooleanTrue || !fPanelMemoryValidated || fStopping)return kIOReturnBadArgument;
        setProperty("LatitudeHotkeySnapshotStage","locate LCD in ACPI plane");
        IORegistryEntry *entry=IORegistryEntry::fromPath("IOACPIPlane:/_SB/PC00/GFX0@20000/LCD@400");
        auto lcd=OSDynamicCast(IOACPIPlatformDevice,entry);IOReturn result=kIOReturnNotFound;
        if(lcd){
            setProperty("LatitudeHotkeySnapshotStage","validate local method");
            result=lcd->validateObject("LDBG");
            if(result==kIOReturnSuccess){
                setProperty("LatitudeHotkeySnapshotStage","evaluate local method");
                OSObject *raw=nullptr;result=lcd->evaluateObject("LDBG",&raw);
                if(result==kIOReturnSuccess){
                    setProperty("LatitudeHotkeySnapshotStage","decode package");
                    auto values=OSDynamicCast(OSArray,raw);
                    if(!values || values->getCount()!=5)result=kIOReturnBadArgument;
                    else {
                        auto snapshot=OSDictionary::withCapacity(5);
                        const char *names[]={"Query66Entries","SMEEEntries","LastSMEEFlags","BRT6Entries","LastBRT6Argument"};
                        if(!snapshot)result=kIOReturnNoMemory;
                        else {
                            for(unsigned i=0;i<5;i++){
                                setProperty("LatitudeHotkeySnapshotCounter",names[i]);
                                auto value=OSDynamicCast(OSNumber,values->getObject(i));
                                if(!value){result=kIOReturnBadArgument;break;}
                                snapshot->setObject(names[i],value);
                            }
                            if(result==kIOReturnSuccess){
                                setProperty("LatitudeHotkeySnapshotStage","publish snapshot");
                                if(!setProperty("LatitudeHotkeyCounters",snapshot))result=kIOReturnNoMemory;
                                else setProperty("LatitudeHotkeySnapshotStage","complete");
                            }
                            snapshot->release();
                        }
                    }
                }
                if(raw)raw->release();
            }
        }
        if(entry)entry->release();setProperty("LatitudeHotkeySnapshotResult",(uint64_t)(uint32_t)result,32);return result;
    }

    OSObject *payload = outer ? outer->getObject("LatitudeGPUIdentity") : nullptr;
    if (!payload) return IOFramebuffer::setProperties(properties);
    if (IOUserClient::clientHasPrivilege(current_task(), kIOClientPrivilegeAdministrator) != kIOReturnSuccess)
        return kIOReturnNotPrivileged;
    OSDictionary *report = OSDynamicCast(OSDictionary, payload);
    if (!report) return kIOReturnBadArgument;
    auto number = [report](const char *key) { return OSDynamicCast(OSNumber, report->getObject(key)); };
    OSNumber *ver=number("version"), *vendor=number("vendor"), *device=number("device"),
             *heap=number("sharedHeapBytes"), *ram=number("physicalRAMBytes");
    OSString *name=OSDynamicCast(OSString, report->getObject("name"));
    if (!ver || !vendor || !device || !heap || !ram || !name ||
        ver->unsigned64BitValue()!=1 || vendor->unsigned64BitValue()!=0x8086 || device->unsigned64BitValue()!=0x7d45 ||
        !latitudeGPUIdentityValid(1,0x8086,0x7d45,name->getCStringNoCopy(),name->getLength(),
                                 heap->unsigned64BitValue(),ram->unsigned64BitValue())) return kIOReturnBadArgument;
    IOService *claim=getProvider();
    IOPCIDevice *pci=claim ? OSDynamicCast(IOPCIDevice,claim->getProvider()) : nullptr;
    if (!fPanelMemoryValidated || !pci || pci->configRead16(0)!=0x8086 || pci->configRead16(2)!=0x7d45 ||
        pci->getProperty("nmxgpu-claimed")!=kOSBooleanTrue) return kIOReturnNotReady;
    OSData *model=OSData::withBytes(name->getCStringNoCopy(),name->getLength()+1);
    OSNumber *mb=OSNumber::withNumber(heap->unsigned64BitValue() >> 20,32);
    if (!model || !mb) { OSSafeReleaseNULL(model); OSSafeReleaseNULL(mb); return kIOReturnNoMemory; }
    bool ok=true;
    /* PCI is System Information's device record. Mirror onto our services for diagnostics. */
    IORegistryEntry *entries[3]={pci,this,fAccel};
    for (auto entry:entries) if (entry) {
        ok=entry->setProperty("model",model) && ok;
        ok=entry->setProperty("VRAM,totalMB",mb) && ok;
        ok=entry->setProperty("NMXSharedMemoryBytes",heap) && ok;
        ok=entry->setProperty("NMXGPUIdentity",report) && ok;
        ok=entry->setProperty("NMXMemoryIsShared",kOSBooleanTrue) && ok;
    }
    model->release();mb->release();
    VFBLOG("query-based identity published: %s shared heap %llu bytes (%llu MiB), success=%d",
           name->getCStringNoCopy(),heap->unsigned64BitValue(),heap->unsigned64BitValue()>>20,ok);
    return ok ? kIOReturnSuccess : kIOReturnNoMemory;
}
#endif
