#include <IOKit/IOService.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOMessage.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include <libkern/c++/OSNumber.h>
#include <libkern/c++/OSArray.h>
#include <libkern/c++/OSString.h>
#include <kern/clock.h>
#include "LatitudeModeQualification.h"
class LatitudeTabletState:public IOService {
 OSDeclareDefaultStructors(LatitudeTabletState)
 IOACPIPlatformDevice *device=nullptr;IOWorkLoop *loop=nullptr;IOCommandGate *gate=nullptr;IOTimerEventSource *timer=nullptr;
 IONotifier *notice=nullptr,*power=nullptr;bool gateAdded=false,timerAdded=false,active=false,asleep=false;UInt64 reads=0,errors=0,events=0;
 LatitudeTablet::ModeQualification state;
 static UInt64 nowNs(){UInt64 ticks=0,ns=0;clock_get_uptime(&ticks);absolutetime_to_nanoseconds(ticks,&ns);return ns;}
 static bool matches(IOService *p){auto d=OSDynamicCast(IOACPIPlatformDevice,p);if(!d)return false;OSString *name=OSString::withCString("HIDD");bool ok=name&&d->compareName(name,nullptr);if(name)name->release();return ok;}
 void invalidate(){state.reset();setProperty("LatitudeTabletModeQualified",false);setProperty("LatitudeTabletModeCurrent",false);setProperty("LatitudeTabletModeHeartbeatUptimeNs",(UInt64)0,64);}
 IOReturn read(){
  if(!active||asleep)return kIOReturnNotReady;
  OSObject*raw=nullptr;IOReturn rc=device->evaluateObject("LTSM",&raw);reads++;
  auto list=OSDynamicCast(OSArray,raw);auto magic=list&&list->getCount()==3?OSDynamicCast(OSNumber,list->getObject(0)):nullptr;
  auto valid=magic?OSDynamicCast(OSNumber,list->getObject(1)):nullptr;auto value=valid?OSDynamicCast(OSNumber,list->getObject(2)):nullptr;
  bool ok=!rc&&magic&&magic->unsigned64BitValue()==0x4c545331&&valid&&valid->unsigned64BitValue()==1&&value&&value->unsigned64BitValue()<255;
  UInt64 ns=nowNs();unsigned byte=ok?(unsigned)value->unsigned64BitValue():255;
  bool qualified=state.update(ns/1000000,ok,byte);
  setProperty("LatitudeTabletRawEC39",(UInt64)byte,32);setProperty("LatitudeTabletModeQualified",qualified);setProperty("LatitudeTabletModeCurrent",state.isTablet());
  setProperty("LatitudeTabletModeHeartbeatUptimeNs",qualified?ns:0,64);
  if(!ok)errors++;setProperty("LatitudeTabletReads",reads,64);setProperty("LatitudeTabletReadErrors",errors,64);setProperty("LatitudeTabletLastReadResult",(UInt64)(UInt32)rc,32);
  if(raw)raw->release();return ok?kIOReturnSuccess:(rc?rc:kIOReturnBadArgument);
 }
 static void tick(OSObject *owner,IOTimerEventSource*){auto s=OSDynamicCast(LatitudeTabletState,owner);if(s&&s->active&&!s->asleep){s->read();s->timer->setTimeoutMS(1000);}}
 static IOReturn action(OSObject *owner,void *arg,void*,void*,void*){
  auto s=OSDynamicCast(LatitudeTabletState,owner);if(!s||!s->active||!arg)return kIOReturnNotReady;UInt32 event=*(UInt32*)arg;
  if(event==kIOMessageSystemWillSleep){s->asleep=true;s->timer->cancelTimeout();s->invalidate();return kIOReturnSuccess;}
  if(event==kIOMessageSystemHasPoweredOn){s->asleep=false;s->invalidate();s->timer->setTimeoutMS(500);return kIOReturnSuccess;}
  if(event!=0xcc&&event!=0xcd)return kIOReturnSuccess;
  s->events++;s->state.event(event,nowNs()/1000000);s->setProperty("LatitudeTabletLastEvent",event,32);s->setProperty("LatitudeTabletEventCount",s->events,64);
  // A contradictory unfold event must hide the keyboard immediately, even if
  // the EC still returns the old folded bit during its transition.
  if(event==0xcd){s->setProperty("LatitudeTabletModeQualified",false);s->setProperty("LatitudeTabletModeCurrent",false);}
  s->timer->setTimeoutMS(100);return kIOReturnSuccess;
 }
 static IOReturn notify(void *target,void*,UInt32 kind,IOService *provider,void *arg,vm_size_t size){auto s=(LatitudeTabletState*)target;if(!s||provider!=s->device||kind!=kIOACPIMessageDeviceNotification||!arg||(size&&size<4))return kIOReturnSuccess;UInt32 event=*(UInt32*)arg;if(event!=0xcc&&event!=0xcd)return kIOReturnSuccess;return s->gate?s->gate->runAction(action,&event):kIOReturnNotReady;}
 static IOReturn powerNotify(void *target,void*,UInt32 kind,IOService*,void*,vm_size_t){auto s=(LatitudeTabletState*)target;if(kind!=kIOMessageSystemWillSleep&&kind!=kIOMessageSystemHasPoweredOn)return kIOReturnSuccess;return s&&s->gate?s->gate->runAction(action,&kind):kIOReturnNotReady;}
 static IOReturn halt(OSObject *owner,void*,void*,void*,void*){auto s=(LatitudeTabletState*)owner;s->active=false;if(s->timer)s->timer->cancelTimeout();s->invalidate();return kIOReturnSuccess;}
 void cleanup(){
  if(power){power->remove();power=nullptr;}if(notice){notice->remove();notice=nullptr;}
  if(gate&&gateAdded)gate->runAction(halt);else active=false;
  if(timer){if(timerAdded){loop->removeEventSource(timer);timerAdded=false;}timer->release();timer=nullptr;}
  if(gate){if(gateAdded){loop->removeEventSource(gate);gateAdded=false;}gate->release();gate=nullptr;}
  if(loop){loop->release();loop=nullptr;}device=nullptr;
 }
public:
 IOService *probe(IOService *p,SInt32*)override{return matches(p)?this:nullptr;}
 bool start(IOService *p)override{
  if(!IOService::start(p)||!matches(p))return false;device=OSDynamicCast(IOACPIPlatformDevice,p);
  UInt64 version=0;if(device->evaluateInteger("LTSV",&version)||version!=2)return false;
  loop=IOWorkLoop::workLoop();gate=IOCommandGate::commandGate(this,action);timer=IOTimerEventSource::timerEventSource(this,tick);
  if(!loop||!gate||!timer||loop->addEventSource(gate)){cleanup();return false;}gateAdded=true;
  if(loop->addEventSource(timer)){cleanup();return false;}timerAdded=true;
  active=true;invalidate();setProperty("LatitudeTabletStateVersion","0.2.1");setProperty("LatitudeTabletInputSuppressionEnabled",true);setProperty("LatitudeTabletOrientationStreamReady",false);
  notice=device->registerInterest(gIOGeneralInterest,notify,this);auto root=getPMRootDomain();power=root?root->registerInterest(gIOGeneralInterest,powerNotify,this):nullptr;
  if(!notice||!power){cleanup();return false;}
  registerService();timer->setTimeoutMS(100);return true;
 }
 void stop(IOService *p)override{cleanup();IOService::stop(p);}void free()override{cleanup();IOService::free();}
};
OSDefineMetaClassAndStructors(LatitudeTabletState,IOService)
