#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOPlatformExpert.h>
#include <IOKit/IONVRAM.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOTimerEventSource.h>
#include <pexpert/pexpert.h>
#include <mach/kmod.h>
extern "C" kern_return_t _start(kmod_info_t *,void *) { return KERN_SUCCESS; }
extern "C" kern_return_t _stop(kmod_info_t *,void *) { return KERN_SUCCESS; }
extern "C" { KMOD_EXPLICIT_DECL(com.nullmoth.LatitudePanelClaim,"0.2.3",_start,_stop) }
static bool claimed = false;
class LatitudePanelClaim : public IOService {
 OSDeclareDefaultStructors(LatitudePanelClaim)
 IOWorkLoop *loop = nullptr;
 IOTimerEventSource *timer = nullptr;
 bool holding = false;
 void releaseHold() { if (holding) { holding=false;adjustBusy(-1); } }
 static void timeout(OSObject *owner,IOTimerEventSource *) {
  auto me=OSDynamicCast(LatitudePanelClaim,owner);if(!me)return;
  me->retain();bool hasFB=false;
  if(OSIterator *it=me->getClientIterator()) { while(OSObject *o=it->getNextObject()) {
   IOService *c=OSDynamicCast(IOService,o);if(c&&c->metaCast("IOFramebuffer")&&!c->isInactive())hasFB=true;
  }it->release(); }
  me->releaseHold();
  if(!hasFB) {
   IOLog("LatitudePanelClaim: no framebuffer after 45 seconds; releasing PCI category for firmware fallback\n");
   IOService *pci=me->getProvider();if(pci)pci->retain();
   me->terminate(kIOServiceRequired);
   if(pci){pci->registerService(kIOServiceAsynchronous);pci->release();}
  }
  me->release();
 }
public:
 IOService *probe(IOService *provider,SInt32 *score) override {
  auto pci=OSDynamicCast(IOPCIDevice,provider);
  if(!pci)return nullptr;
  pci->setProperty("LatitudePanelClaimProbe","entered");
  uint16_t vendor=pci->configRead16(kIOPCIConfigVendorID),device=pci->configRead16(kIOPCIConfigDeviceID);
  uint16_t sv=pci->configRead16(kIOPCIConfigSubSystemVendorID),sd=pci->configRead16(kIOPCIConfigSubSystemID);
  IOLog("LatitudePanelClaim: probe %04x:%04x subsystem %04x:%04x claimed=%u\n",vendor,device,sv,sd,claimed);
  if(claimed){pci->setProperty("LatitudePanelClaimProbe","already tried this boot");return nullptr;}
  const bool supportedGpu =
    vendor == 0x8086 &&
    (device == 0x7D45 || device == 0x7D55);

  if (!supportedGpu) {
      pci->setProperty(
          "LatitudePanelClaimProbe",
          "Unsupported PCI GPU"
      );
      return nullptr;
  }
  bool once=false;
  if(IORegistryEntry *e=IORegistryEntry::fromPath("/options",gIODTPlane)) {
   if(auto n=OSDynamicCast(IODTNVRAM,e)) {
    if(OSObject *o=n->copyProperty("nullmoth-panel-once")) {
     o->release();once=true;
     if(const OSSymbol *key=OSSymbol::withCString("nullmoth-panel-once")){n->removeProperty(key);key->release();}n->sync();
    }
   }e->release();
  }
  if(!once){pci->setProperty("LatitudePanelClaimProbe","one-shot unavailable");return nullptr;}
  pci->setProperty("LatitudePanelClaimProbe","one-shot consumed; waiting for Aux framebuffer");
  claimed=true;setProperty("LatitudePanelOptedIn",kOSBooleanTrue);
  IOLog("LatitudePanelClaim: one-shot consumed; reserving firmware panel for Auxiliary KC framebuffer\n");
  return IOService::probe(provider,score);
 }
 bool start(IOService *provider) override {
  if(!IOService::start(provider)){provider->setProperty("LatitudePanelClaimProbe","IOService start failed");return false;}
  loop=IOWorkLoop::workLoop();timer=IOTimerEventSource::timerEventSource(this,timeout);
  if(!loop||!timer||loop->addEventSource(timer)!=kIOReturnSuccess){OSSafeReleaseNULL(timer);OSSafeReleaseNULL(loop);return false;}
  holding=false;timer->setTimeoutMS(45000);provider->setProperty("LatitudePanelClaimProbe","reservation started");registerService();return true;
 }
 IOReturn callPlatformFunction(const OSSymbol *fn,bool wait,void *p1,void *p2,void *p3,void *p4) override {
  if(fn&&fn->isEqualTo("latitudePanelFramebufferReady")){if(timer)timer->cancelTimeout();setProperty("LatitudePanelFramebufferReady",kOSBooleanTrue);releaseHold();return kIOReturnSuccess;}
  return IOService::callPlatformFunction(fn,wait,p1,p2,p3,p4);
 }
 void stop(IOService *provider) override {
  if(timer){timer->cancelTimeout();if(loop)loop->removeEventSource(timer);}OSSafeReleaseNULL(timer);OSSafeReleaseNULL(loop);releaseHold();IOService::stop(provider);
 }
};
OSDefineMetaClassAndStructors(LatitudePanelClaim,IOService)
