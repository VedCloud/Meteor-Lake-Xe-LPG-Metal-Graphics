from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
source=(r/'hotkey-sources/LatitudeHotkeys.cpp').read_text();source='\n'.join(x for x in source.splitlines() if not x.startswith('#include <'));source=source.replace('class LatitudeHotkeys:public IOService {','struct LatitudeHotkeys:public IOService {')
pre=r'''
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <cstdio>
using IOReturn=int;using UInt32=uint32_t;using SInt32=int32_t;using vm_size_t=size_t;
constexpr int kIOReturnSuccess=0,kIOReturnBadArgument=1,kIOReturnNoMemory=2,kIOReturnNotReady=3,kIOACPIMessageDeviceNotification=100,gIOGeneralInterest=1;
#define OSDeclareDefaultStructors(x)
#define OSDefineMetaClassAndStructors(x,y)
#define OSDynamicCast(T,x) dynamic_cast<T*>(x)
void IOLog(const char*,...){}
struct OSObject{virtual ~OSObject(){};virtual void release(){}};
static std::vector<std::unique_ptr<OSObject>> allocations;
template<class T,class...A>T*make(A...args){auto p=new T(args...);allocations.emplace_back(p);return p;}
struct OSNumber:OSObject{uint64_t n;OSNumber(uint64_t v):n(v){}static OSNumber*withNumber(uint64_t n,unsigned){return make<OSNumber>(n);}uint64_t unsigned64BitValue(){return n;}};
struct OSData:OSObject{std::vector<uint8_t>v;OSData(std::vector<uint8_t>x):v(x){}void*getBytesNoCopy(){return v.data();}size_t getLength(){return v.size();}};
struct OSDictionary:OSObject{};
struct IONotifier:OSObject{bool removed=false;void remove(){removed=true;}};
struct IOService;static IOService *keyboard=nullptr;static bool notifierFail=false,loopFail=false,addFail=false;static std::vector<UInt32>sent;
struct OSIterator:OSObject{bool used=false;OSObject*getNextObject();};
struct IOService:OSObject{
 virtual IOService*probe(IOService*,SInt32*){return this;}virtual bool start(IOService*){return true;}virtual void stop(IOService*){}virtual void free(){}
 virtual IOReturn message(UInt32 t,IOService*,void*v){assert(t==kIOACPIMessageDeviceNotification);sent.push_back(*(UInt32*)v);return 0;}
 std::map<std::string,uint64_t>nums;std::map<std::string,std::string>strings;
 void setProperty(const char*k,uint64_t v,unsigned){nums[k]=v;}void setProperty(const char*k,bool v){nums[k]=v;}void setProperty(const char*k,const char*v){strings[k]=v;}void registerService(){}
 static OSDictionary*serviceMatching(const char*n){assert(std::string(n)=="ApplePS2Keyboard");return make<OSDictionary>();}
 static OSIterator*getMatchingServices(OSDictionary*){return make<OSIterator>();}
 using Notice=IOReturn(*)(void*,void*,UInt32,IOService*,void*,vm_size_t);
 IONotifier*registerInterest(int,Notice,void*){return notifierFail?nullptr:make<IONotifier>();}
};
OSObject*OSIterator::getNextObject(){if(used)return nullptr;used=true;return keyboard;}
struct IOACPIPlatformDevice:IOService{
 uint64_t ec=0,guard=0;bool hasGuard=true,guardMismatch=false;int enableError=0,readError=0;OSData *wdg=nullptr,*event=nullptr;
 IOReturn validateObject(const char*n){assert(std::string(n)=="LBRG");return hasGuard?0:1;}
 IOReturn evaluateObject(const char*n,OSObject**out=nullptr,OSObject**args=nullptr,unsigned count=0){assert(std::string(n).size()<=4);std::string m=n;if(out)*out=nullptr;
  if(m=="_WDG"){if(out)*out=wdg;return 0;}if(m=="ECD0"){*out=make<OSNumber>(ec);return 0;}
  if(m=="LBRG"){assert(count==1);guard=dynamic_cast<OSNumber*>(args[0])->n;if(out)*out=make<OSNumber>(guardMismatch&&guard?0:guard);return 0;}
  if(m=="WED0"){assert(count==1);auto v=dynamic_cast<OSNumber*>(args[0])->n;if(v&&enableError)return enableError;ec=v;return 0;}
  if(m=="_WED"){assert(count==1&&dynamic_cast<OSNumber*>(args[0])->n==0xd0);if(out)*out=event;return readError;}assert(false);return 1;
 }
};
struct IOCommandGate;
struct IOWorkLoop:OSObject{static IOWorkLoop*workLoop(){return loopFail?nullptr:make<IOWorkLoop>();}IOReturn addEventSource(IOCommandGate*);void removeEventSource(IOCommandGate*);};
struct IOCommandGate:OSObject{using Action=IOReturn(*)(OSObject*,void*,void*,void*,void*);OSObject*owner;bool added=false;IOCommandGate(OSObject*x):owner(x){}static IOCommandGate*commandGate(OSObject*x,Action){return make<IOCommandGate>(x);}IOReturn runAction(Action a,void*v=nullptr){assert(added);return a(owner,v,nullptr,nullptr,nullptr);}};
IOReturn IOWorkLoop::addEventSource(IOCommandGate*g){if(addFail)return 1;g->added=true;return 0;}void IOWorkLoop::removeEventSource(IOCommandGate*g){assert(g->added);g->added=false;}
'''
post=r'''
static OSData *guid(){return make<OSData>(std::vector<uint8_t>{0x94,0x59,0xbb,0x9d,0x97,0xa9,0xda,0x11,0xb0,0x12,0xb6,0x22,0xa1,0xef,0x54,0x92,0xd0,0,1,8});}
int main(){IOService k;keyboard=&k;
 for(int mode=0;mode<8;mode++){IOACPIPlatformDevice d;d.wdg=guid();LatitudeHotkeys bridge;
  loopFail=mode==1;addFail=mode==2;notifierFail=mode==3;d.hasGuard=mode!=4;d.guardMismatch=mode==7;d.ec=mode==5?1:0;d.enableError=mode==6?17:0;
  bool ok=bridge.start(&d);
  if(mode){assert(!ok);assert(!d.guard);assert(d.ec==(mode==5?1ul:0ul));bridge.free();continue;}
  assert(ok&&d.ec==1&&d.guard==1);
  UInt32 notify=0xd0;
  d.event=make<OSData>(std::vector<uint8_t>{2,0,0,0,5,0xe0});assert(!bridge.notification(&bridge,nullptr,kIOACPIMessageDeviceNotification,&d,&notify,4));assert(sent.back()==0x0365&&bridge.down==1);
  d.event=make<OSData>(std::vector<uint8_t>{3,0,0x10,0,0x58,0,10,0});bridge.read();assert(sent.back()==0x0366&&bridge.up==1);
  size_t count=sent.size();d.event=make<OSData>(std::vector<uint8_t>{2,0,0,0,0x0c,0xe0});bridge.read();assert(sent.size()==count&&bridge.ignored==1);
  d.event=make<OSData>(std::vector<uint8_t>{0xff,0,0,0});bridge.read();assert(bridge.malformed==1);
  keyboard=nullptr;d.event=make<OSData>(std::vector<uint8_t>{2,0,0,0,6,0xe0});bridge.read();assert(bridge.missing==1);keyboard=&k;
  d.readError=9;bridge.read();assert(bridge.errors==1);d.readError=0;
  notify=0xc0;auto before=bridge.notifications;bridge.notification(&bridge,nullptr,kIOACPIMessageDeviceNotification,&d,&notify,4);assert(bridge.notifications==before);
  bridge.stop(&d);assert(d.ec==0&&d.guard==0&&!bridge.enabled&&!bridge.gate);bridge.free();
 }
 puts("PASS actual WMI bridge with mocked IOKit/firmware: brightness translation, ignore policy, malformed data, missing keyboard, read errors, event filtering, ownership refusal, start-failure cleanup and stop restore. No device access.");
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(pre+source+post)
 subprocess.run(['xcrun','clang++','-std=c++17','-g','-fsanitize=address,undefined','-I',str(r/'hotkey-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
