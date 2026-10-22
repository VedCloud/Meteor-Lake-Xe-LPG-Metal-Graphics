"""Compile and exercise the actual IOUserClient implementation with isolated IOKit stubs."""
from pathlib import Path
import subprocess,tempfile,os
r=Path(__file__).resolve().parent.parent
prefix=r'''#include <cassert>
#include <cstdint>
#include <cstring>
#include <mutex>
#include "LatitudeAWDLRadio.h"
using UInt32=uint32_t;using UInt64=uint64_t;using SInt32=int32_t;using IOReturn=int;using task_t=void*;
constexpr int kIOReturnSuccess=0,kIOReturnNotOpen=1,kIOReturnBadArgument=2,kIOReturnUnsupported=3,kIOReturnNotPrivileged=4,kIOReturnNoMemory=5,kIOReturnError=6,kIOReturnNotAttached=7,kIOReturnNotReady=8,kIOReturnNotFound=9,kIOReturnNotPermitted=10;
constexpr const char*kIOClientPrivilegeAdministrator="root";
struct OSObject{int refs=1;virtual ~OSObject()=default;virtual void free(){};void retain(){++refs;}void release(){if(!--refs){free();delete this;}}};
struct OSDictionary{};struct IOExternalMethodDispatch{};
struct IOExternalMethodArguments{unsigned scalarInputCount=0,scalarOutputCount=0;void*structureInputDescriptor=nullptr,*structureOutputDescriptor=nullptr;size_t structureInputSize=0,structureOutputSize=0;const void*structureInput=nullptr;void*structureOutput=nullptr;uint64_t*scalarOutput=nullptr;};
struct IOService:OSObject{bool inactive=false;virtual bool start(IOService*){return true;}virtual void stop(IOService*){};bool isInactive(){return inactive;}template<class...T>void setProperty(T...){};virtual IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,class IOUserClient**){return 77;}};
struct IOUserClient:IOService{bool initWithTask(task_t,void*,UInt32){return true;}bool attach(IOService*){return true;}void detach(IOService*){};void terminate(){};virtual IOReturn clientClose(){return 0;}static int clientHasPrivilege(void*t,const char*){return t==(void*)1?0:1;}virtual IOReturn externalMethod(uint32_t,IOExternalMethodArguments*,IOExternalMethodDispatch*,OSObject*,void*){return 0;}};
struct IOCommandGate:OSObject{OSObject*owner=nullptr;using Action=IOReturn(*)(OSObject*,void*,void*,void*,void*);std::recursive_mutex mutex;IOReturn runAction(Action a,void*p=nullptr){std::lock_guard<std::recursive_mutex>g(mutex);return a(owner,p,nullptr,nullptr,nullptr);}};
struct IOWorkLoop:OSObject{};IOCommandGate*_fCommandGate=nullptr;IOWorkLoop*_fWorkloop=nullptr;
using IOLock=std::mutex;void IOLockLock(IOLock*l){l->lock();}void IOLockUnlock(IOLock*l){l->unlock();}
#define OSDeclareDefaultStructors(x)
#define OSDefineMetaClassAndStructors(x,y)
#define OSDynamicCast(T,p) dynamic_cast<T*>(p)
#define OSTypeAlloc(T) new T
#define super IOService
static bool armed=false;static int txCalls=0;
static bool PE_parse_boot_argn(const char*,void*out,size_t){*(int*)out=armed?1:0;return true;}
static bool OSCompareAndSwap(UInt32 old,UInt32 value,volatile UInt32*p){return __sync_bool_compare_and_swap(p,old,value);}
static SInt32 OSAddAtomic(SInt32 v,volatile SInt32*p){return __sync_fetch_and_add(p,v);}
struct ItlIwx:IOService{LatitudeAWDLRadio::Queue latitudeAWDLReceiveQueue;void latitudeAWDLStatus(LatitudeAWDLRadio::Status&s){s={};s.flags=123;}int latitudeAWDLCurrentChannel(void*,size_t){++txCalls;return 42;}};
struct AirportItlwm:IOService{IOLock*fAWDLBridgeLock=nullptr;IOCommandGate*fAWDLBridgeGate=nullptr;IOWorkLoop*fAWDLBridgeLoop=nullptr;bool fAWDLBridgeReady=false;IOService*fHalService=nullptr;
 IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,IOUserClient**)override;
 bool latitudeAWDLBridgeStart();void latitudeAWDLBridgeStop();IOReturn latitudeAWDLBridgeRun(void*);
 static IOReturn latitudeAWDLBridgeAction(OSObject*,void*,void*,void*,void*);
};
'''
main=r'''
int main(){using namespace LatitudeAWDLRadio;AirportItlwm radio;ItlIwx hal;IOCommandGate gate;IOWorkLoop loop;IOLock lock;
 radio.fAWDLBridgeLock=&lock;radio.fHalService=&hal;gate.owner=&radio;_fCommandGate=&gate;_fWorkloop=&loop;
 IOUserClient*client=(IOUserClient*)123;
 assert(radio.newUserClient(nullptr,nullptr,Connection,nullptr,&client)==kIOReturnNotPrivileged&&!client);
 assert(radio.newUserClient((void*)1,nullptr,123,nullptr,&client)==77);
 assert(radio.newUserClient((void*)1,nullptr,Connection,nullptr,&client)==0&&client);
 auto invoke=[&](unsigned selector,IOExternalMethodArguments&args){return client->externalMethod(selector,&args,nullptr,nullptr,nullptr);};
 Status status;IOExternalMethodArguments query;query.structureOutput=&status;query.structureOutputSize=sizeof(status);
 assert(invoke(Query,query)==kIOReturnNotReady&&query.structureOutputSize==0);
 assert(radio.latitudeAWDLBridgeStart());query.structureOutputSize=sizeof(status);assert(invoke(Query,query)==0&&status.flags==123);
 query.structureInputDescriptor=(void*)1;assert(invoke(Query,query)==kIOReturnBadArgument);query.structureInputDescriptor=nullptr;
 query.structureOutputSize--;assert(invoke(Query,query)==kIOReturnBadArgument);
 uint8_t payload[4096]={};uint64_t error=99;IOExternalMethodArguments tx;tx.structureInput=payload;tx.structureInputSize=40;tx.scalarOutput=&error;tx.scalarOutputCount=1;
 assert(invoke(TransmitAction,tx)==kIOReturnNotPermitted&&!txCalls);
 armed=true;assert(invoke(TransmitAction,tx)==0&&txCalls==1&&error==42);
 tx.structureInputSize=4097;assert(invoke(TransmitAction,tx)==kIOReturnBadArgument&&txCalls==1);tx.structureInputSize=7;assert(invoke(TransmitAction,tx)==kIOReturnBadArgument&&txCalls==1);
 Received f;IOExternalMethodArguments rx;rx.structureOutput=&f;rx.structureOutputSize=sizeof(f);assert(invoke(ReceiveAction,rx)==kIOReturnNotFound&&rx.structureOutputSize==0);
 assert(hal.latitudeAWDLReceiveQueue.push(payload,40,5220,-10,123));rx.structureOutputSize=sizeof(f);assert(invoke(ReceiveAction,rx)==0&&f.length==40&&f.uptimeNs==123);
 radio.latitudeAWDLBridgeStop();query.structureOutputSize=sizeof(status);assert(invoke(Query,query)==kIOReturnNotReady);assert(gate.refs==1&&loop.refs==1);
 LatitudeAWDLBridgeCall stale{Query,&status,sizeof(status)};assert(gate.runAction(AirportItlwm::latitudeAWDLBridgeAction,&stale)==kIOReturnNotReady);
 assert(client->clientClose()==0);query.structureOutputSize=sizeof(status);assert(invoke(Query,query)==kIOReturnNotOpen);client->stop(&radio);client->release();assert(radio.refs==1);
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp)
 for name in ['IOKit/IOUserClient.h','libkern/OSAtomic.h']:
  f=p/name;f.parent.mkdir(parents=True,exist_ok=True);f.write_text('// isolated fixture stub\n')
 code=p/'test.cpp';code.write_text(prefix+'\n#include "LatitudeAWDLUserClient.inc"\n'+main)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(p),'-I'+str(r/'wireless-sources'),str(code),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],env=dict(os.environ,UBSAN_OPTIONS='halt_on_error=1'),check=True)
print('PASS actual radio IOUserClient: root authorization, normal-client forwarding, argument bounds, transmit boot gate, errno, receive/no-data, shutdown barrier and closed-client lifetime. Isolated APIs; no host radio access.')
