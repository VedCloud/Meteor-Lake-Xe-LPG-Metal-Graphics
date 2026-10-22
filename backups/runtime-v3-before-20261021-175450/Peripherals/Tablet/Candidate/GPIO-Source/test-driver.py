from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
s=(r/'gpio-sources/LatitudeGPIO.cpp').read_text();s='\n'.join(l for l in s.splitlines() if not l.startswith('#include'));s=s.replace('class LatitudeGPIO:public VoodooGPIO {','struct LatitudeGPIO:public VoodooGPIO {')
pre=r'''
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include "latitude_gpio_map.h"
using IOReturn=int;using IOOptionBits=unsigned;
constexpr int kIOReturnSuccess=0,kIOReturnUnsupported=1,kIOReturnNotPermitted=2,kIOReturnExclusiveAccess=3,kIOReturnError=4,kIOReturnNotReady=5,kIOReturnNoInterrupt=6,kIOInterruptTypeLevel=1,IRQ_TYPE_LEVEL_LOW=8;
#define OSDeclareDefaultStructors(x)
#define OSDefineMetaClassAndStructors(x,y)
#define OSDynamicCast(T,x) dynamic_cast<T*>(x)
struct OSObject{int refs=1;virtual ~OSObject()=default;void retain(){refs++;}virtual void release(){assert(refs>0);refs--;}};
struct IOService;using IOInterruptAction=void(*)(OSObject*,void*,IOService*,int);
struct IOMemoryMap:OSObject{std::vector<uint32_t> v=std::vector<uint32_t>(16384);uintptr_t getVirtualAddress(){return (uintptr_t)v.data();}size_t getLength(){return v.size()*4;}};
struct IOService:OSObject{virtual bool start(IOService*){return true;}virtual void stop(IOService*){}virtual void free(){}virtual unsigned getDeviceMemoryCount(){return 5;}virtual IOMemoryMap*mapDeviceMemoryWithIndex(unsigned){return nullptr;}
 virtual IOReturn registerInterrupt(int,OSObject*,IOInterruptAction,void*){return 0;}virtual IOReturn unregisterInterrupt(int){return 0;}virtual IOReturn enableInterrupt(int){return 0;}virtual IOReturn disableInterrupt(int){return 0;}virtual IOReturn getInterruptType(int,int*){return 0;}
 std::map<std::string,uint64_t> properties;void setProperty(const char*k,uint64_t v,unsigned){properties[k]=v;}void setProperty(const char*k,bool v){properties[k]=v;}void setName(const char*){}void registerService(){}
};
struct IOACPIPlatformDevice:IOService{IOMemoryMap*m=nullptr;unsigned count=5;unsigned getDeviceMemoryCount()override{return count;}IOMemoryMap*mapDeviceMemoryWithIndex(unsigned i)override{assert(i==4);return m;}};
struct IOWorkLoop;
struct Event:OSObject{IOWorkLoop*loop=nullptr;IOWorkLoop*getWorkLoop(){return loop;}void release()override{OSObject::release();if(!refs)delete this;}};
static unsigned fail=0;
struct IOWorkLoop:OSObject{static IOWorkLoop*workLoop(){return fail==1?nullptr:new IOWorkLoop;}int addEventSource(Event*e){if(fail==2)return 1;e->loop=this;return 0;}void removeEventSource(Event*e){e->loop=nullptr;}void release()override{OSObject::release();if(!refs)delete this;}};
struct IOCommandGate:Event{OSObject*owner;using Action=IOReturn(*)(OSObject*,void*,void*,void*,void*);static IOCommandGate*commandGate(OSObject*o){auto g=new IOCommandGate;g->owner=o;return g;}IOReturn runAction(Action a){return a(owner,nullptr,nullptr,nullptr,nullptr);}};
struct IOInterruptEventSource:Event{bool on=false;void enable(){on=true;}void disable(){on=false;}};
struct IOFilterInterruptEventSource:IOInterruptEventSource{using A=void(*)(OSObject*,IOInterruptEventSource*,int);using F=bool(*)(OSObject*,IOFilterInterruptEventSource*);static IOFilterInterruptEventSource*filterInterruptEventSource(OSObject*,A,F,IOService*,int){return fail==3?nullptr:new IOFilterInterruptEventSource;}};
struct VoodooGPIO:IOService{IOWorkLoop*workLoop=nullptr;IOCommandGate*command_gate=nullptr;virtual IOReturn setInterruptTypeForPin(int,int)=0;};
static unsigned delivered=0;static void clientAction(OSObject*,void*,IOService*,int pin){assert(pin==387);delivered++;}
'''
post=r'''
int main(){for(unsigned mode=0;mode<10;mode++){
 IOMemoryMap m;m.v[0]=0x940000;m.v[3]=0x700;IOACPIPlatformDevice p;p.m=&m;OSObject client;LatitudeGPIO d;fail=mode<=3?mode:0;
 if(mode==4)p.count=4;if(mode==5)m.v[3]=0xffffff00;if(mode==6)m.v[0]=0xffffffff;
 bool ok=d.start(&p);if(mode>=1&&mode<=6){assert(!ok);d.free();continue;}
 assert(ok);assert(d.properties["LatitudeGPIOStage"]==1);assert(d.setInterruptTypeForPin(1,8)!=0);assert(d.properties["LatitudeGPIOStage"]==10);assert(d.setInterruptTypeForPin(387,4)!=0);
 if(mode==7)m.v[LAT_OWN/4]=1u<<12;if(mode==8)m.v[LAT_LOCK/4]=8;if(mode==9)m.v[LAT_IE/4]=8;
 auto original=m.v;int rc=d.setInterruptTypeForPin(387,8);
 if(mode>=7){assert(rc!=0);assert(d.properties["LatitudeGPIOStage"]==(mode==9?13:12));assert(d.properties["LatitudeGPIOLastResult"]==(unsigned)rc);assert(d.properties["LatitudeGPIOOriginalEnable"]==original[LAT_IE/4]);assert(m.v==original);d.stop(&p);d.free();continue;}
 assert(!rc);assert(d.properties["LatitudeGPIOStage"]==15);assert(!d.registerInterrupt(387,&client,clientAction,nullptr));assert(client.refs==2);
 assert(!d.enableInterrupt(387));assert(m.v[LAT_IE/4]&8);m.v[LAT_IS/4]=0;assert(!LatitudeGPIO::filter(&d,nullptr));m.v[LAT_IS/4]=8;assert(LatitudeGPIO::filter(&d,nullptr));
 LatitudeGPIO::dispatch(&d,nullptr,1);assert(delivered==1);assert(d.events==1);assert(!d.unregisterInterrupt(387));assert(client.refs==1);
 d.stop(&p);d.free();assert(m.refs==0);assert(!(m.v[LAT_IE/4]&8));assert(!(m.v[LAT_HOST/4]&8));
 for(unsigned i=0;i<m.v.size();i++)if(i!=LAT_IS/4)assert(m.v[i]==original[i]);
 }
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(pre+s+post)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-I'+str(r/'gpio-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS actual GPIO driver mocked MMIO/IOKit: pin dispatch, target-bit-only writes, restore, foreign owner/locked/enabled refusal and start failure cleanup.')
