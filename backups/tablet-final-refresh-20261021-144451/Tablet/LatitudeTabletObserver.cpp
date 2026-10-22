// Passive observer: deliberately no ACPI evaluation, power methods, HID events,
// input suppression, PCI access, sensor feature writes or display transforms.
#include <IOKit/IOService.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include <libkern/c++/OSString.h>
#include <kern/clock.h>
class LatitudeTabletObserver:public IOService {
 OSDeclareDefaultStructors(LatitudeTabletObserver)
 IOACPIPlatformDevice *device=nullptr;
 IOWorkLoop *loop=nullptr; IOCommandGate *gate=nullptr; IONotifier *notice=nullptr;
 bool gateAdded=false,active=false; UInt64 events=0;
 static bool matches(IOService *provider) {
  auto acpi=OSDynamicCast(IOACPIPlatformDevice,provider);
  if(!acpi)return false;
  OSString *name=OSString::withCString("HIDD");
  bool ok=name && acpi->compareName(name,nullptr);if(name)name->release();return ok;
 }
 static IOReturn action(OSObject *owner,void *arg,void*,void*,void*) {
  auto self=OSDynamicCast(LatitudeTabletObserver,owner);
  if(!self || !self->active || !arg)return kIOReturnNotReady;
  UInt32 event=*(UInt32*)arg;
  if(event!=0xcc && event!=0xcd)return kIOReturnSuccess;
  self->events++;
  uint64_t ticks=0,ns=0;clock_get_uptime(&ticks);absolutetime_to_nanoseconds(ticks,&ns);
  self->setProperty("LatitudeTabletEventCount",self->events,64);
  self->setProperty("LatitudeTabletLastEvent",event,32);
  self->setProperty("LatitudeTabletLastEventUptimeNs",ns,64);
  self->setProperty("LatitudeTabletLastEventCandidate",event==0xcc?"tablet":"laptop");
  return kIOReturnSuccess;
 }
 static IOReturn notification(void *target,void*,UInt32 type,IOService *provider,void *arg,vm_size_t size) {
  auto self=(LatitudeTabletObserver*)target;
  if(!self || provider!=self->device || type!=kIOACPIMessageDeviceNotification || !arg || (size && size<sizeof(UInt32)))return kIOReturnSuccess;
  UInt32 event=*(UInt32*)arg;
  if(event!=0xcc && event!=0xcd)return kIOReturnSuccess;
  return self->gate?self->gate->runAction(action,&event):kIOReturnNotReady;
 }
 void teardown() {
  if(notice){notice->remove();notice=nullptr;}
  if(gate){if(gateAdded){loop->removeEventSource(gate);gateAdded=false;}gate->release();gate=nullptr;}
  if(loop){loop->release();loop=nullptr;}active=false;device=nullptr;
 }
public:
 IOService *probe(IOService *provider,SInt32*) override {return matches(provider)?this:nullptr;}
 bool start(IOService *provider) override {
  if(!IOService::start(provider) || !matches(provider))return false;
  device=OSDynamicCast(IOACPIPlatformDevice,provider);
  loop=IOWorkLoop::workLoop();gate=IOCommandGate::commandGate(this,action);
  if(!loop || !gate || loop->addEventSource(gate)!=kIOReturnSuccess){teardown();return false;}
  gateAdded=true;active=true;
  setProperty("LatitudeTabletObserverVersion","0.1.0");
  setProperty("LatitudeTabletActuationEnabled",false);
  setProperty("LatitudeTabletModeQualified",false);
  setProperty("LatitudeTabletLastEventCandidate","unknown");
  setProperty("LatitudeTabletEventCount",(UInt64)0,64);
  setProperty("LatitudeTabletObserverScope","Passive INTC1077 CC/CD events; historical candidates only, not current posture");
  notice=device->registerInterest(gIOGeneralInterest,notification,this);
  if(!notice){teardown();return false;}
  registerService();return true;
 }
 void stop(IOService *provider) override {teardown();IOService::stop(provider);}
 void free() override {teardown();IOService::free();}
};
OSDefineMetaClassAndStructors(LatitudeTabletObserver,IOService)
