#include <IOKit/graphics/IODisplay.h>
#include "latitude_pwm.h"
// The Dell VBT selects native DDI PWM controller0, minimum15/255. Never change
// enable, pin routing, polarity, period, link training or the active pipe.
class LatitudeBacklightHandler : public IODisplayParameterHandler {
 OSDeclareDefaultStructors(LatitudeBacklightHandler)
 IOMemoryMap *mapping=nullptr; IOLock *lock=nullptr; volatile uint8_t *regs=nullptr;
 LatitudePWMState initial={}; uint32_t level=0,lastDuty=0; bool active=false,written=false;
 uint32_t read(uint32_t off) const { return *(volatile uint32_t *)(regs+off); }
 LatitudePWMState state() const {return {read(0xc8250),read(0xc8254),read(0xc8258),read(0x70008),read(0x70180)};}
public:
 static LatitudeBacklightHandler *create(IOMemoryMap *map,unsigned pipe) {
  if(!map || pipe!=0 || map->getLength()<0xc825c)return nullptr;
  auto h=new LatitudeBacklightHandler;
  if(!h || !h->init()){OSSafeReleaseNULL(h);return nullptr;}
  h->lock=IOLockAlloc();if(!h->lock){h->release();return nullptr;}
  h->mapping=map;map->retain();h->regs=(volatile uint8_t *)map->getVirtualAddress();h->initial=h->state();
  if(!latitudePWMValid(h->initial)){IOLog("LatitudeBacklight: disabled: PWM0 or Pipe0 is not active/plausible\n");h->release();return nullptr;}
  h->level=latitudePWMLevel(h->initial.period,h->initial.duty);h->lastDuty=h->initial.duty;h->active=true;
  h->setName("backlight");h->setProperty("LatitudePWMController",0ull,32);h->setProperty("LatitudePWMPeriod",h->initial.period,32);
  h->setProperty("LatitudePWMInitialDuty",h->initial.duty,32);h->setProperty("LatitudePWMReady",kOSBooleanTrue);
  IOLog("LatitudeBacklight: native PWM0 ready: period=%u duty=%u; no initialization writes\n",h->initial.period,h->initial.duty);
  return h;
 }
 bool setDisplay(IODisplay *display) override {
  if(!display || !active)return false;
  OSObject *old=display->copyProperty(gIODisplayParametersKey);
  auto current=OSDynamicCast(OSDictionary,old);
  OSDictionary *params=current?OSDictionary::withDictionary(current):OSDictionary::withCapacity(2);OSSafeReleaseNULL(old);
  if(!params)return false;
  IODisplay::addParameter(params,gIODisplayBrightnessKey,0,65535);
  IODisplay::setParameter(params,gIODisplayBrightnessKey,(SInt32)level);
  IODisplay::addParameter(params,gIODisplayLinearBrightnessKey,0,65535);
  IODisplay::setParameter(params,gIODisplayLinearBrightnessKey,(SInt32)level);
  display->setProperty(gIODisplayParametersKey,params);display->setProperty(kIODisplayHasBacklightKey,kOSBooleanTrue);
  params->release();setProperty("LatitudeBrightnessParametersPublished",kOSBooleanTrue);return true;
 }
 bool doIntegerSet(OSDictionary *,const OSSymbol *key,UInt32 value) override {
  if(key!=gIODisplayBrightnessKey && key!=gIODisplayLinearBrightnessKey)return false;
  if(!lock)return false;
  IOLockLock(lock);uint32_t duty=0;
  bool ok=active && latitudePWMPlan(state(),initial,value,&duty);
  if(ok){*(volatile uint32_t *)(regs+0xc8258)=duty;ok=read(0xc8258)==duty;}
  if(ok){level=value;lastDuty=duty;written=true;setProperty("LatitudePWMCurrentDuty",duty,32);setProperty("LatitudeBrightnessValue",value,32);}
  else setProperty("LatitudeBrightnessWriteRefused",kOSBooleanTrue);
  IOLockUnlock(lock);return ok;
 }
 bool doDataSet(const OSSymbol *,OSData *) override {return false;}
 bool doUpdate() override {return active;}
 void disable() {
  if(!lock)return;IOLockLock(lock);
  if(active && written){auto now=state();if(latitudePWMValid(now) && now.period==initial.period && now.control==initial.control && now.duty==lastDuty){*(volatile uint32_t *)(regs+0xc8258)=initial.duty;(void)read(0xc8258);}}
  active=false;setProperty("LatitudePWMReady",kOSBooleanFalse);IOLockUnlock(lock);
 }
 void free() override {disable();OSSafeReleaseNULL(mapping);regs=nullptr;if(lock){IOLockFree(lock);lock=nullptr;}IODisplayParameterHandler::free();}
};
OSDefineMetaClassAndStructors(LatitudeBacklightHandler,IODisplayParameterHandler)
