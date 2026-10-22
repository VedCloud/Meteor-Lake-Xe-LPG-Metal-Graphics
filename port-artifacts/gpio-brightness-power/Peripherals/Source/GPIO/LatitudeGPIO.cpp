// SPDX-License-Identifier: GPL-2.0-or-later
#include "VoodooGPIO.hpp"
#include <IOKit/IOFilterInterruptEventSource.h>
#include "latitude_gpio_map.h"
class LatitudeGPIO:public VoodooGPIO {
 OSDeclareDefaultStructors(LatitudeGPIO)
 IOMemoryMap *map=nullptr;volatile uint8_t *regs=nullptr;IOFilterInterruptEventSource *irq=nullptr;
 OSObject *client=nullptr;IOInterruptAction action=nullptr;void *reference=nullptr;
 uint32_t pad=0,oldPad=0,ourPad=0,oldHost=0,oldEnable=0;bool configured=false,enabled=false,added=false;
 uint64_t events=0;
 uint32_t rd(unsigned at){return *(volatile uint32_t*)(regs+at);}
 void wr(unsigned at,uint32_t value){*(volatile uint32_t*)(regs+at)=value;}
 void bit(unsigned at,bool on){uint32_t v=rd(at);wr(at,on?v|LAT_BIT:v&~LAT_BIT);}
 static bool filter(OSObject *owner,IOFilterInterruptEventSource*){
  auto s=(LatitudeGPIO*)owner;return s->regs&&s->enabled&&((s->rd(LAT_IS)&s->rd(LAT_IE)&LAT_BIT)!=0);
 }
 static void dispatch(OSObject *owner,IOInterruptEventSource*,int){
  auto s=(LatitudeGPIO*)owner;if(!s->enabled||!s->client||!s->action)return;
  s->wr(LAT_IS,LAT_BIT);s->action(s->client,s->reference,s,LAT_GPIO);s->wr(LAT_IS,LAT_BIT);
  s->events++;if((s->events&63)==0)s->setProperty("LatitudeGPIOEvents",s->events,64);
 }
 static IOReturn releaseClient(OSObject *o,void*,void*,void*,void*) {auto s=(LatitudeGPIO*)o;if(s->client){s->client->release();s->client=nullptr;}s->action=nullptr;return kIOReturnSuccess;}
 void releaseState(){
  if(regs&&configured){bit(LAT_IE,false);wr(LAT_IS,LAT_BIT);
   if((rd(pad)&~2u)==(ourPad&~2u))wr(pad,(oldPad&~2u)|(rd(pad)&2u));
   bit(LAT_HOST,(oldHost&LAT_BIT)!=0);bit(LAT_IE,(oldEnable&LAT_BIT)!=0);configured=false;
  }
  enabled=false;if(irq){if(added)workLoop->removeEventSource(irq);irq->release();irq=nullptr;added=false;}
  releaseClient(this,nullptr,nullptr,nullptr,nullptr);
  if(map){map->release();map=nullptr;regs=nullptr;}
 }
 static IOReturn cleanAction(OSObject *o,void*,void*,void*,void*){((LatitudeGPIO*)o)->releaseState();return kIOReturnSuccess;}
 void cleanup(){
  if(irq)irq->disable();
  if(command_gate&&command_gate->getWorkLoop())command_gate->runAction(cleanAction);else releaseState();
  if(command_gate){if(command_gate->getWorkLoop())workLoop->removeEventSource(command_gate);command_gate->release();command_gate=nullptr;}
  if(workLoop){workLoop->release();workLoop=nullptr;}
 }

public:
 bool start(IOService *provider) override {
  if(!IOService::start(provider))return false;
  auto acpi=OSDynamicCast(IOACPIPlatformDevice,provider);if(!acpi||provider->getDeviceMemoryCount()!=5)return false;
  map=provider->mapDeviceMemoryWithIndex(4);if(!map)return false;regs=(volatile uint8_t*)map->getVirtualAddress();
  if(map->getLength()<0x220||rd(0)==0||rd(0)==0xffffffff||((rd(0)>>16)<0x94)||!latitudePadOffset(rd(0xc),map->getLength(),&pad)){cleanup();return false;}
  workLoop=IOWorkLoop::workLoop();if(!workLoop){cleanup();return false;}
  command_gate=IOCommandGate::commandGate(this);if(!command_gate||workLoop->addEventSource(command_gate)!=kIOReturnSuccess){cleanup();return false;}
  irq=IOFilterInterruptEventSource::filterInterruptEventSource(this,dispatch,filter,provider,0);
  if(!irq||workLoop->addEventSource(irq)!=kIOReturnSuccess){cleanup();return false;}added=true;irq->disable();
  setProperty("LatitudeGPIOPin",(uint64_t)LAT_GPIO,32);setProperty("LatitudeGPIOPad",(uint64_t)LAT_PAD,32);setProperty("LatitudeGPIOEvents",0ull,64);
  setProperty("LatitudeGPIOStatusOffset",(uint64_t)LAT_IS,32);setName("VoodooGPIO");registerService();return true;
 }
 IOReturn setInterruptTypeForPin(int pin,int type) override {
  if(!latitudePin(pin)||type!=IRQ_TYPE_LEVEL_LOW||!regs)return kIOReturnUnsupported;
  if(configured)return kIOReturnSuccess;
  oldPad=rd(pad);oldHost=rd(LAT_HOST);oldEnable=rd(LAT_IE);
  if(!latitudePadPlan(oldPad,rd(LAT_OWN),rd(LAT_LOCK),&ourPad))return kIOReturnNotPermitted;
  // Refuse another active owner instead of clearing a live interrupt.
  if(oldEnable&LAT_BIT)return kIOReturnExclusiveAccess;
  configured=true;bit(LAT_HOST,true);wr(pad,ourPad);wr(LAT_IS,LAT_BIT);
  if((rd(pad)&~2u)!=(ourPad&~2u)||(rd(LAT_HOST)&LAT_BIT)==0){wr(pad,oldPad);bit(LAT_HOST,(oldHost&LAT_BIT)!=0);configured=false;return kIOReturnError;}
  setProperty("LatitudeGPIOConfigured",true);return kIOReturnSuccess;
 }
 IOReturn registerInterrupt(int pin,OSObject *target,IOInterruptAction handler,void *refcon) override {
  if(!latitudePin(pin)||!configured||!target||!handler||client)return kIOReturnNotReady;
  client=target;client->retain();action=handler;reference=refcon;return kIOReturnSuccess;
 }
 IOReturn unregisterInterrupt(int pin) override {
  if(!latitudePin(pin))return kIOReturnNoInterrupt;disableInterrupt(pin);
  return command_gate?command_gate->runAction(releaseClient):kIOReturnNotReady;
 }

 IOReturn enableInterrupt(int pin) override {if(!latitudePin(pin)||!client||!configured)return kIOReturnNotReady;enabled=true;wr(LAT_IS,LAT_BIT);bit(LAT_IE,true);irq->enable();return kIOReturnSuccess;}
 IOReturn disableInterrupt(int pin) override {if(!latitudePin(pin))return kIOReturnNoInterrupt;enabled=false;if(irq)irq->disable();if(regs&&configured)bit(LAT_IE,false);return kIOReturnSuccess;}
 IOReturn getInterruptType(int pin,int *type) override {if(!latitudePin(pin)||!type)return kIOReturnNoInterrupt;*type=kIOInterruptTypeLevel;return kIOReturnSuccess;}
 void stop(IOService *provider) override {cleanup();IOService::stop(provider);}
 void free() override {cleanup();IOService::free();}
};
OSDefineMetaClassAndStructors(LatitudeGPIO,VoodooGPIO)
