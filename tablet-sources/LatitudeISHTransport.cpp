// Private Meteor Lake ISH PIO bring-up candidate. Protocol source references in
// latitude_ish_pio.h. No DMA buffer, arbitrary physical mapping or bus mastering.
#include <IOKit/IOService.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/IOFilterInterruptEventSource.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOMemoryDescriptor.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include <IOKit/IOMessage.h>
#include <libkern/c++/OSData.h>
#include <libkern/OSAtomic.h>
#include <kern/clock.h>
#include <pexpert/pexpert.h>
#include "latitude_ish_pio.h"
#include "latitude_ish_hid.h"
struct LatitudeISHMMIO {
 volatile uint8_t*base=nullptr;
 uint32_t read32(unsigned at){if(!base||at>0x3fc||(at&3))return 0xffffffffu;OSSynchronizeIO();return *(volatile uint32_t*)(base+at);}
 void write32(unsigned at,uint32_t value){if(base&&at<=0x3fc&&!(at&3)){*(volatile uint32_t*)(base+at)=value;OSSynchronizeIO();}}
 void flush(){(void)read32(0x34);OSSynchronizeIO();}
};
class LatitudeISHTransport:public IOService {
 OSDeclareDefaultStructors(LatitudeISHTransport)
 IOPCIDevice *pci=nullptr;IOMemoryMap*map=nullptr;IOWorkLoop*loop=nullptr;IOCommandGate*gate=nullptr;IOFilterInterruptEventSource*irq=nullptr;IOTimerEventSource*timer=nullptr;IONotifier*power=nullptr;
 LatitudeISH::HID sensors;bool sensorClientStarted=false;
 LatitudeISHMMIO registers;LatitudeISH::PIO<LatitudeISHMMIO> link{registers};
 bool opened=false,gateAdded=false,irqAdded=false,timerAdded=false,armed=false,maskChanged=false;
 uint32_t originalMask=0;uint64_t clientMessages=0,interrupts=0;
 static uint64_t now(){uint64_t ticks=0,ns=0;clock_get_uptime(&ticks);absolutetime_to_nanoseconds(ticks,&ns);return ns/1000000;}
 void disarm(){
  armed=false;sensors.reset();sensorClientStarted=false;if(timer)timer->cancelTimeout();if(irq)irq->disable();
  if(registers.base){
   link.stop();
   if(maskChanged){uint32_t value=registers.read32(8);if(value!=0xffffffffu){registers.write32(8,(value&~0x101u)|(originalMask&0x100u));registers.flush();}}
  }
  maskChanged=false;setProperty("LatitudeISHTransportArmed",false);setProperty("LatitudeISHOrientationReady",false);
 }
 void failure(const char*reason){setProperty("LatitudeISHError",reason);disarm();}
 void sensorsStep(){
  uint64_t time=now();
  if(link.state()==LatitudeISH::PIO<LatitudeISHMMIO>::Phase::Connected){
   if(!sensorClientStarted){sensors.start(time);sensorClientStarted=true;}
   if(!sensors.tick(time)){setProperty("LatitudeISHHIDError",sensors.error());}
   else if(sensors.hasRequest()){const auto &request=sensors.peek();if(link.sendClient(request.data,request.length,time))sensors.sent();}
  }
  auto sample=sensors.current(time);setProperty("LatitudeISHOrientationReady",sample.fresh);
  setProperty("LatitudeISHHIDPhase",(uint64_t)sensors.state(),32);
  if(sample.fresh){setProperty("LatitudeISHAccelerationX",(uint64_t)(int64_t)sample.x,64);setProperty("LatitudeISHAccelerationY",(uint64_t)(int64_t)sample.y,64);setProperty("LatitudeISHAccelerationZ",(uint64_t)(int64_t)sample.z,64);setProperty("LatitudeISHAccelerationUptimeNs",sample.hostMS*1000000,64);setProperty("LatitudeISHFirmwareTimestamp",sample.firmwareTimestamp,64);}
 }
 void status(){
  setProperty("LatitudeISHPhase",(uint64_t)link.state(),32);setProperty("LatitudeISHReceived",link.received(),64);setProperty("LatitudeISHSent",link.sent(),64);setProperty("LatitudeISHClientMessages",clientMessages,64);setProperty("LatitudeISHInterrupts",interrupts,64);
 }
 static bool filter(OSObject*owner,IOFilterInterruptEventSource*){
  auto s=(LatitudeISHTransport*)owner;if(!s->armed||!s->registers.base)return false;
  uint32_t status=s->registers.read32(0xc);return status!=0xffffffffu&&status!=0;
 }
 void serviceInterrupt(){
  if(!armed)return;interrupts++;
  uint32_t statusBits=registers.read32(0xc);if(statusBits==0xffffffffu){failure("PCI device inaccessible");return;}
  if(statusBits)registers.write32(0xc,statusBits); // only busy-clear bit is RW
  int result=link.interrupt(now());
  if(result<0){failure(link.error());return;}
  if(result==1){
   size_t length=link.size();const uint8_t*bytes=link.data();
   auto packet=bytes&&length&&length<=8192?OSData::withBytes(bytes,(unsigned)length):nullptr;
   if(!packet){failure("complete client-message capture allocation failed");return;}
   setProperty("LatitudeISHLastClientMessage",packet);packet->release();clientMessages++;
   if(sensorClientStarted&&!sensors.input(bytes,length,now()))setProperty("LatitudeISHHIDError",sensors.error());
   if(!link.consumed()){failure("receive-credit acknowledgement refused");return;}
  }
  sensorsStep();status();
 }
 static void interruptAction(OSObject*owner,IOInterruptEventSource*,int){((LatitudeISHTransport*)owner)->serviceInterrupt();}
 static void tick(OSObject*owner,IOTimerEventSource*){
  auto s=(LatitudeISHTransport*)owner;if(!s->armed)return;
  if(!s->link.tick(now())){s->failure(s->link.error());return;}s->sensorsStep();s->status();s->timer->setTimeoutMS(100);
 }
 static IOReturn powerAction(OSObject*owner,void*arg,void*,void*,void*){
  auto s=(LatitudeISHTransport*)owner;if(!arg)return kIOReturnBadArgument;
  uint32_t event=*(uint32_t*)arg;
  if(event==kIOMessageSystemWillSleep){s->disarm();s->setProperty("LatitudeISHNeedsRearmAfterWake",true);}
  return kIOReturnSuccess;
 }
 static IOReturn powerNotice(void*target,void*,UInt32 kind,IOService*,void*,vm_size_t){auto s=(LatitudeISHTransport*)target;if(kind!=kIOMessageSystemWillSleep)return kIOReturnSuccess;return s->gate?s->gate->runAction(powerAction,&kind):kIOReturnNotReady;}
 static IOReturn halt(OSObject*owner,void*,void*,void*,void*){((LatitudeISHTransport*)owner)->disarm();return kIOReturnSuccess;}
 void cleanup(){
  if(power){power->remove();power=nullptr;}if(gate&&gateAdded)gate->runAction(halt);else disarm();
  if(irq){if(irqAdded){loop->removeEventSource(irq);irqAdded=false;}irq->release();irq=nullptr;}
  if(timer){if(timerAdded){loop->removeEventSource(timer);timerAdded=false;}timer->release();timer=nullptr;}
  if(gate){if(gateAdded){loop->removeEventSource(gate);gateAdded=false;}gate->release();gate=nullptr;}
  if(loop){loop->release();loop=nullptr;}
  registers.base=nullptr;if(map){map->release();map=nullptr;}
  if(pci&&opened){pci->close(this);opened=false;}pci=nullptr;
 }
public:
 bool start(IOService*provider)override{
  if(!IOService::start(provider))return false;pci=OSDynamicCast(IOPCIDevice,provider);
  if(!pci||pci->configRead16(0)!=0x8086||pci->configRead16(2)!=0x7e45){pci=nullptr;return false;}
  if(!pci->open(this)){pci=nullptr;return false;}opened=true;
  // Probe an already memory-enabled, D0 device only. Do not silently power or
  // reset another PCI function to make it appear usable.
  if(!(pci->configRead16(4)&2)){cleanup();return false;}
  UInt8 pm=0;if(pci->findPCICapability(kIOPCICapabilityIDPowerManagement,&pm)&&pm&&(pci->configRead16((UInt8)(pm+4))&3)){cleanup();return false;}
  auto memory=pci->getDeviceMemoryWithRegister(0x10);if(!memory||memory->getLength()<0x400||!memory->getPhysicalAddress()){cleanup();return false;}
  map=memory->map(kIOMapInhibitCache);if(!map||map->getLength()<0x400){cleanup();return false;}registers.base=(volatile uint8_t*)map->getVirtualAddress();
  uint32_t fw=registers.read32(0x34);if(fw==0xffffffffu){cleanup();return false;}
  setProperty("LatitudeISHTransportVersion","0.1.1");setProperty("LatitudeISHInitialFWSTS",(uint64_t)fw,32);setProperty("LatitudeISHInitialHostComm",(uint64_t)registers.read32(0x38),32);setProperty("LatitudeISHInitialRMP2",(uint64_t)registers.read32(0x368),32);setProperty("LatitudeISHOrientationReady",false);setProperty("LatitudeISHTransportArmed",false);setProperty("LatitudeISHDMAEnabledByDriver",false);
  int enable=0;PE_parse_boot_argn("latitude-ish-pio",&enable,sizeof(enable));
  if(enable!=1){setProperty("LatitudeISHMode","read-only preflight; latitude-ish-pio=1 required for IPC writes");registerService();return true;}
  if(fw&0xf0000u){setProperty("LatitudeISHError","DMA active; refusing IPC takeover");registerService();return true;}
  loop=IOWorkLoop::workLoop();gate=IOCommandGate::commandGate(this);timer=IOTimerEventSource::timerEventSource(this,tick);irq=IOFilterInterruptEventSource::filterInterruptEventSource(this,interruptAction,filter,pci,0);
  if(!loop||!gate||!timer||!irq||loop->addEventSource(gate)){cleanup();return false;}gateAdded=true;
  if(loop->addEventSource(timer)){cleanup();return false;}timerAdded=true;
  if(loop->addEventSource(irq)){cleanup();return false;}irqAdded=true;
  auto root=getPMRootDomain();power=root?root->registerInterest(gIOGeneralInterest,powerNotice,this):nullptr;if(!power){cleanup();return false;}
  originalMask=registers.read32(8);if(originalMask==0xffffffffu){cleanup();return false;}
  maskChanged=true;registers.write32(8,originalMask&~0x101u);registers.flush();
  if(!link.start(now())){setProperty("LatitudeISHError",link.error());cleanup();return false;}
  armed=true;irq->enable();registers.write32(8,(originalMask|1u)&~0x100u);registers.flush();timer->setTimeoutMS(100);
  setProperty("LatitudeISHTransportArmed",true);setProperty("LatitudeISHMode","PIO + exact descriptor-bound HID polling; no sensor power/feature writes");registerService();return true;
 }
 void stop(IOService*p)override{cleanup();IOService::stop(p);}void free()override{cleanup();IOService::free();}
};
OSDefineMetaClassAndStructors(LatitudeISHTransport,IOService)
