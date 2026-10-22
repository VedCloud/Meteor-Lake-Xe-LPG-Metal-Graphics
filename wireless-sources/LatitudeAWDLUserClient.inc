// Private root-only transport; included by AirportItlwmV2.cpp after its header.
#include <IOKit/IOUserClient.h>
#include <libkern/OSAtomic.h>
struct LatitudeAWDLBridgeCall {UInt32 selector;void *bytes;size_t length;uint64_t result=0;};
class LatitudeAWDLUserClient:public IOUserClient {
 OSDeclareDefaultStructors(LatitudeAWDLUserClient)
 AirportItlwm *owner=nullptr;volatile UInt32 closed=0;
public:
 bool start(IOService *provider)override{
  auto radio=OSDynamicCast(AirportItlwm,provider);if(!radio||!IOUserClient::start(provider))return false;
  owner=radio;owner->retain();return true;
 }
 IOReturn clientClose()override{OSCompareAndSwap(0,1,&closed);terminate();return kIOReturnSuccess;}
 void stop(IOService *provider)override{OSCompareAndSwap(0,1,&closed);IOUserClient::stop(provider);}
 void free()override{if(owner){owner->release();owner=nullptr;}IOUserClient::free();}
 IOReturn externalMethod(uint32_t selector,IOExternalMethodArguments *args,IOExternalMethodDispatch *,OSObject *,void *)override{
  if(!args||!owner||OSAddAtomic(0,(volatile SInt32*)&closed))return kIOReturnNotOpen;
  if(args->scalarInputCount||args->structureInputDescriptor||args->structureOutputDescriptor)return kIOReturnBadArgument;
  using namespace LatitudeAWDLRadio;
  if(selector==Query||selector==ReceiveAction){
   size_t need=selector==Query?sizeof(Status):sizeof(Received);
   if(args->structureInputSize||args->scalarOutputCount||args->structureOutputSize!=need||!args->structureOutput)return kIOReturnBadArgument;
   bzero(args->structureOutput,need);LatitudeAWDLBridgeCall call{selector,args->structureOutput,need};
   IOReturn rc=owner->latitudeAWDLBridgeRun(&call);if(rc!=kIOReturnSuccess)args->structureOutputSize=0;return rc;
  }
  if(selector!=TransmitAction)return kIOReturnUnsupported;
  if(args->structureOutputSize||args->scalarOutputCount!=1||!args->scalarOutput||!args->structureInput||args->structureInputSize<8||args->structureInputSize>4096)return kIOReturnBadArgument;
  // Inline IOKit input is kernel-owned for this synchronous call. The HAL
  // creates its own mbuf; no user pointer or buffer survives this operation.
  LatitudeAWDLBridgeCall call{selector,(void*)args->structureInput,args->structureInputSize};
  IOReturn rc=owner->latitudeAWDLBridgeRun(&call);args->scalarOutput[0]=call.result;return rc;
 }
};
OSDefineMetaClassAndStructors(LatitudeAWDLUserClient,IOUserClient)

IOReturn AirportItlwm::newUserClient(task_t task,void *securityID,UInt32 type,OSDictionary *properties,IOUserClient **handler)
{
 if(type!=LatitudeAWDLRadio::Connection)return super::newUserClient(task,securityID,type,properties,handler);
 if(!handler)return kIOReturnBadArgument;*handler=nullptr;
 if(IOUserClient::clientHasPrivilege(task,kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)return kIOReturnNotPrivileged;
 auto client=OSTypeAlloc(LatitudeAWDLUserClient);if(!client)return kIOReturnNoMemory;
 if(!client->initWithTask(task,securityID,type)){client->release();return kIOReturnError;}
 if(!client->attach(this)){client->release();return kIOReturnNotAttached;}
 if(!client->start(this)){client->detach(this);client->release();return kIOReturnNotReady;}
 *handler=client;return kIOReturnSuccess;
}
bool AirportItlwm::latitudeAWDLBridgeStart()
{
 if(!fAWDLBridgeLock||!_fCommandGate||!_fWorkloop)return false;
 IOLockLock(fAWDLBridgeLock);
 fAWDLBridgeGate=_fCommandGate;fAWDLBridgeLoop=_fWorkloop;
 fAWDLBridgeGate->retain();fAWDLBridgeLoop->retain();fAWDLBridgeReady=true;
 IOLockUnlock(fAWDLBridgeLock);
 setProperty("LatitudeAWDLRadioProtocol",(uint64_t)LatitudeAWDLRadio::Version,32);
 return true;
}
void AirportItlwm::latitudeAWDLBridgeStop()
{
 if(!fAWDLBridgeLock)return;
 IOLockLock(fAWDLBridgeLock);fAWDLBridgeReady=false;
 auto gate=fAWDLBridgeGate;auto loop=fAWDLBridgeLoop;
 fAWDLBridgeGate=nullptr;fAWDLBridgeLoop=nullptr;IOLockUnlock(fAWDLBridgeLock);
 // Mark closed before draining the controller gate. A client that took a
 // retained snapshot before stop is refused by the gated ready check.
 if(gate){gate->runAction(latitudeAWDLBridgeAction);gate->release();}
 if(loop)loop->release();
}
IOReturn AirportItlwm::latitudeAWDLBridgeRun(void *request)
{
 if(!fAWDLBridgeLock)return kIOReturnNotReady;
 IOLockLock(fAWDLBridgeLock);
 auto gate=fAWDLBridgeReady?fAWDLBridgeGate:nullptr;auto loop=fAWDLBridgeReady?fAWDLBridgeLoop:nullptr;
 if(gate)gate->retain();if(loop)loop->retain();IOLockUnlock(fAWDLBridgeLock);
 if(!gate||!loop){if(gate)gate->release();if(loop)loop->release();return kIOReturnNotReady;}
 IOReturn rc=gate->runAction(latitudeAWDLBridgeAction,request);
 gate->release();loop->release();return rc;
}
IOReturn AirportItlwm::latitudeAWDLBridgeAction(OSObject *object,void *request,void*,void*,void*)
{
 auto self=OSDynamicCast(AirportItlwm,object);if(!self)return kIOReturnBadArgument;
 if(!request)return kIOReturnSuccess; // teardown barrier, no HAL access
 IOLockLock(self->fAWDLBridgeLock);bool ready=self->fAWDLBridgeReady;IOLockUnlock(self->fAWDLBridgeLock);
 if(!ready||self->isInactive())return kIOReturnNotReady;
 auto radio=OSDynamicCast(ItlIwx,self->fHalService);if(!radio)return kIOReturnUnsupported;
 auto call=(LatitudeAWDLBridgeCall*)request;using namespace LatitudeAWDLRadio;
 if(call->selector==Query){if(call->length!=sizeof(Status))return kIOReturnBadArgument;radio->latitudeAWDLStatus(*(Status*)call->bytes);return kIOReturnSuccess;}
 if(call->selector==ReceiveAction){if(call->length!=sizeof(Received))return kIOReturnBadArgument;return radio->latitudeAWDLReceiveQueue.pop(*(Received*)call->bytes)?kIOReturnSuccess:kIOReturnNotFound;}
 if(call->selector!=TransmitAction)return kIOReturnUnsupported;
 int armed=0;if(!PE_parse_boot_argn("latitude-awdl-current-channel",&armed,sizeof(armed))||armed!=1)return kIOReturnNotPermitted;
 call->result=(uint32_t)radio->latitudeAWDLCurrentChannel(call->bytes,call->length);
 self->setProperty("LatitudeAWDLRawLastSubmitErrno",call->result,32);return kIOReturnSuccess;
}
