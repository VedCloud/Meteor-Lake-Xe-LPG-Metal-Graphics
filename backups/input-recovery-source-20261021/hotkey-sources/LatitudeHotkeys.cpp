#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include <libkern/c++/OSData.h>
#include <libkern/c++/OSNumber.h>
#include "latitude_wmi_protocol.h"
class LatitudeHotkeys:public IOService {
 OSDeclareDefaultStructors(LatitudeHotkeys)
 IOACPIPlatformDevice *device=nullptr;IOWorkLoop *loop=nullptr;IOCommandGate *gate=nullptr;IONotifier *notice=nullptr;
 bool enabled=false,owned=false,gateAdded=false;
 uint64_t notifications=0,reads=0,errors=0,delivered=0,missing=0,malformed=0,ignored=0,up=0,down=0;
 static bool matches(IOACPIPlatformDevice *d){
  if(!d)return false;OSObject *raw=nullptr;IOReturn rc=d->evaluateObject("_WDG",&raw);auto data=OSDynamicCast(OSData,raw);
  bool ok=rc==kIOReturnSuccess&&data&&latitudeWmiGuid((const uint8_t*)data->getBytesNoCopy(),data->getLength());if(raw)raw->release();return ok;
 }
 IOReturn number(const char *method,uint64_t *value){OSObject *raw=nullptr;IOReturn rc=device->evaluateObject(method,&raw);auto n=OSDynamicCast(OSNumber,raw);if(!rc&&!n)rc=kIOReturnBadArgument;if(!rc)*value=n->unsigned64BitValue();if(raw)raw->release();return rc;}
 IOReturn set(const char *method,unsigned value){OSNumber *n=OSNumber::withNumber(value,32);if(!n)return kIOReturnNoMemory;OSObject *args[]={n};IOReturn rc=device->evaluateObject(method,nullptr,args,1);n->release();return rc;}
 IOReturn ownership(unsigned value){
  OSNumber *n=OSNumber::withNumber(value,32);if(!n)return kIOReturnNoMemory;OSObject *args[]={n},*raw=nullptr;
  IOReturn rc=device->evaluateObject("LBRG",&raw,args,1);n->release();auto returned=OSDynamicCast(OSNumber,raw);
  if(!rc&&(!returned||returned->unsigned64BitValue()!=value))rc=kIOReturnBadArgument;if(raw)raw->release();return rc;
 }
 void counters(){
  setProperty("Notifications",notifications,64);setProperty("BufferReads",reads,64);setProperty("ReadErrors",errors,64);setProperty("Delivered",delivered,64);setProperty("MissingKeyboard",missing,64);setProperty("Malformed",malformed,64);setProperty("IgnoredRecords",ignored,64);setProperty("BrightnessUp",up,64);setProperty("BrightnessDown",down,64);
 }
 void send(int direction){
  auto matching=IOService::serviceMatching("ApplePS2Keyboard");auto it=matching?IOService::getMatchingServices(matching):nullptr;if(matching)matching->release();
  auto keyboard=it?OSDynamicCast(IOService,it->getNextObject()):nullptr;bool unique=keyboard&&it->getNextObject()==nullptr;
  if(unique){UInt32 code=direction>0?0x0366:0x0365;IOReturn rc=keyboard->message(kIOACPIMessageDeviceNotification,this,&code);if(!rc)delivered++;else missing++;}
  else missing++;if(it)it->release();
 }
 IOReturn read(){
  if(!enabled)return kIOReturnNotReady;notifications++;OSObject *raw=nullptr;OSNumber *n=OSNumber::withNumber(0xd0,32);if(!n)return kIOReturnNoMemory;
  OSObject *args[]={n};IOReturn rc=device->evaluateObject("_WED",&raw,args,1);n->release();auto data=OSDynamicCast(OSData,raw);
  if(rc||!data){errors++;setProperty("LastReadResult",(uint64_t)(uint32_t)(rc?rc:kIOReturnBadArgument),32);}
  else {reads++;const uint8_t *bytes=(const uint8_t*)data->getBytesNoCopy();size_t size=data->getLength(),offset=0;
   while(offset<size){LatitudeWmiRecord r={};int result=latitudeWmiRecord(bytes,size,offset,&r);if(result<0){malformed++;break;}if(!result)break;
    setProperty("LastType",r.type,32);setProperty("LastCode",r.code,32);
    if(r.direction){if(r.direction>0)up++;else down++;send(r.direction);}else ignored++;offset=r.next;
   }
  }
  if(raw)raw->release();counters();return rc;
 }
 static IOReturn action(OSObject *owner,void *a0,void*,void*,void*){
  auto self=OSDynamicCast(LatitudeHotkeys,owner);if(!self)return kIOReturnBadArgument;
  if(a0){IOReturn rc=kIOReturnSuccess;if(self->enabled)rc=self->set("WED0",0);self->enabled=false;if(self->owned){self->ownership(0);self->owned=false;}return rc;}
  return self->read();
 }
 static IOReturn notification(void *target,void*,UInt32 kind,IOService *provider,void *arg,vm_size_t size){
  auto self=(LatitudeHotkeys*)target;if(!self||provider!=self->device||kind!=kIOACPIMessageDeviceNotification||!arg||(size&&size<sizeof(UInt32)))return kIOReturnSuccess;
  if(*(UInt32*)arg!=0xd0)return kIOReturnSuccess;return self->gate?self->gate->runAction(action):kIOReturnNotReady;
 }
 void teardown(){
  if(notice){notice->remove();notice=nullptr;}
  if(gate){if(gateAdded){gate->runAction(action,(void*)1);loop->removeEventSource(gate);gateAdded=false;}gate->release();gate=nullptr;}
  if(loop){loop->release();loop=nullptr;}device=nullptr;
 }
public:
 IOService *probe(IOService *provider,SInt32 *score) override {return matches(OSDynamicCast(IOACPIPlatformDevice,provider))?this:nullptr;}
 bool start(IOService *provider) override {
  if(!IOService::start(provider))return false;device=OSDynamicCast(IOACPIPlatformDevice,provider);if(!matches(device))return false;
  uint64_t active=0;if(number("ECD0",&active)||active){IOLog("LatitudeHotkeys0.1.0: event enable state unavailable or already owned; refused\n");return false;}
  if(device->validateObject("LBRG")!=kIOReturnSuccess){IOLog("LatitudeHotkeys0.1.0: BRTKEYS revision5 ownership method absent; refused\n");return false;}
  loop=IOWorkLoop::workLoop();gate=IOCommandGate::commandGate(this,action);if(!loop||!gate||loop->addEventSource(gate)!=kIOReturnSuccess){teardown();return false;}
  gateAdded=true;
  notice=device->registerInterest(gIOGeneralInterest,notification,this);if(!notice){teardown();return false;}
  owned=true;IOReturn rc=ownership(1);if(rc){IOLog("LatitudeHotkeys0.1.0: ownership guard failed %x\n",rc);teardown();return false;}
  enabled=true;rc=set("WED0",1);setProperty("EnableResult",(uint64_t)(uint32_t)rc,32);if(rc){IOLog("LatitudeHotkeys0.1.0: WED0 enable failed %x\n",rc);teardown();return false;}
  setProperty("Protocol","Dell WMI D0; brightness only; first key per record");setProperty("RouteActive",true);counters();registerService();IOLog("LatitudeHotkeys0.1.0: Dell WMI D0 brightness bridge active\n");return true;
 }
 void stop(IOService *provider) override {setProperty("RouteActive",false);teardown();IOService::stop(provider);}
 void free() override {teardown();IOService::free();}
};
OSDefineMetaClassAndStructors(LatitudeHotkeys,IOService)
