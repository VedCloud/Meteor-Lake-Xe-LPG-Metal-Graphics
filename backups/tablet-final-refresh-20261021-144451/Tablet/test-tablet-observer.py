from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parent.parent
source=(root/'tablet-sources/LatitudeTabletObserver.cpp').read_text();source='\n'.join(s for s in source.splitlines() if not s.startswith('#include'));source=source.replace('class LatitudeTabletObserver:public IOService {','struct LatitudeTabletObserver:public IOService {')
pre=r'''
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
using IOReturn=int;using UInt32=uint32_t;using UInt64=uint64_t;using SInt32=int32_t;using vm_size_t=size_t;
constexpr int kIOReturnSuccess=0,kIOReturnNotReady=3,kIOACPIMessageDeviceNotification=100,gIOGeneralInterest=1;
#define OSDeclareDefaultStructors(x)
#define OSDefineMetaClassAndStructors(x,y)
#define OSDynamicCast(T,x) dynamic_cast<T*>(x)
struct OSObject{virtual ~OSObject(){};virtual void release(){}};
static std::vector<std::unique_ptr<OSObject>> allocations;
template<class T,class...A>T*make(A...args){auto p=new T(args...);allocations.emplace_back(p);return p;}
struct OSString:OSObject{std::string s;OSString(const char*v):s(v){}static OSString*withCString(const char*v){return make<OSString>(v);}};
struct IONotifier:OSObject{bool removed=false;void remove(){removed=true;}};
static bool notifierFail=false,loopFail=false,addFail=false;
void clock_get_uptime(uint64_t*v){*v=1234;}void absolutetime_to_nanoseconds(uint64_t t,uint64_t*v){*v=t*1000;}
struct IOService:OSObject{
 virtual IOService*probe(IOService*,SInt32*){return this;}virtual bool start(IOService*){return true;}virtual void stop(IOService*){}virtual void free(){}
 std::map<std::string,uint64_t>nums;std::map<std::string,std::string>strings;
 void setProperty(const char*k,uint64_t v,unsigned){nums[k]=v;}void setProperty(const char*k,bool v){nums[k]=v;}void setProperty(const char*k,const char*v){strings[k]=v;}void registerService(){}
 using Notice=IOReturn(*)(void*,void*,UInt32,IOService*,void*,vm_size_t);
 IONotifier*registerInterest(int,Notice,void*){return notifierFail?nullptr:make<IONotifier>();}
};
struct IOACPIPlatformDevice:IOService {std::string name="HIDD";bool compareName(OSString*n,OSString**){return n->s==name;}};
struct IOCommandGate;
struct IOWorkLoop:OSObject{static IOWorkLoop*workLoop(){return loopFail?nullptr:make<IOWorkLoop>();}IOReturn addEventSource(IOCommandGate*);void removeEventSource(IOCommandGate*);};
struct IOCommandGate:OSObject{using Action=IOReturn(*)(OSObject*,void*,void*,void*,void*);OSObject*owner;bool added=false;IOCommandGate(OSObject*x):owner(x){}static IOCommandGate*commandGate(OSObject*x,Action){return make<IOCommandGate>(x);}IOReturn runAction(Action a,void*v=nullptr){assert(added);return a(owner,v,nullptr,nullptr,nullptr);}};
IOReturn IOWorkLoop::addEventSource(IOCommandGate*g){if(addFail)return 1;g->added=true;return 0;}void IOWorkLoop::removeEventSource(IOCommandGate*g){assert(g->added);g->added=false;}
'''
post=r'''
int main(){
 for(int failure=0;failure<5;failure++) {
  IOACPIPlatformDevice d;LatitudeTabletObserver observer;loopFail=failure==1;addFail=failure==2;notifierFail=failure==3;if(failure==4)d.name="WMI0";
  assert(observer.start(&d)==(failure==0));
  if(failure){observer.free();assert(!observer.gate&&!observer.loop&&!observer.active);continue;}
  assert(observer.nums["LatitudeTabletActuationEnabled"]==0 && observer.nums["LatitudeTabletModeQualified"]==0);
  UInt32 event=0xcc;
  auto notify=[&](UInt32 type,void*arg,size_t size,IOService*p){return observer.notification(&observer,nullptr,type,p,arg,size);};
  notify(kIOACPIMessageDeviceNotification,&event,4,&d);
  assert(observer.events==1 && observer.strings["LatitudeTabletLastEventCandidate"]=="tablet");
  event=0xcd;notify(kIOACPIMessageDeviceNotification,&event,0,&d);
  assert(observer.events==2 && observer.strings["LatitudeTabletLastEventCandidate"]=="laptop");
  event=0xcc;IOACPIPlatformDevice other;
  notify(999,&event,4,&d);notify(kIOACPIMessageDeviceNotification,nullptr,4,&d);notify(kIOACPIMessageDeviceNotification,&event,1,&d);notify(kIOACPIMessageDeviceNotification,&event,4,&other);
  event=0xcb;notify(kIOACPIMessageDeviceNotification,&event,4,&d);assert(observer.events==2);
  assert(observer.nums["LatitudeTabletLastEventUptimeNs"]==1234000);
  auto savedNotice=observer.notice;observer.stop(&d);assert(savedNotice->removed&&!observer.active&&!observer.gate&&!observer.loop);observer.free();
 }
 puts("PASS actual passive observer under ASan/UBSan: CC/CD decoding, filtering, short/null arguments, coexistence scope, failure cleanup, teardown; no firmware or input APIs provided by fixture.");
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(pre+source+post)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
