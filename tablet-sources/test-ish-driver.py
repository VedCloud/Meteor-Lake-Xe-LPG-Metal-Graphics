from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parent.parent
source=(root/'tablet-sources/LatitudeISHTransport.cpp').read_text();source='\n'.join(x for x in source.splitlines() if not x.startswith('#include <'));source=source.replace('class LatitudeISHTransport:public IOService {','struct LatitudeISHTransport:public IOService {')
pre=r'''
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
using UInt8=uint8_t;using UInt32=uint32_t;using vm_size_t=size_t;using IOReturn=int;
constexpr int kIOReturnSuccess=0,kIOReturnBadArgument=1,kIOReturnNotReady=2,kIOMessageSystemWillSleep=3,kIOPCICapabilityIDPowerManagement=1,kIOMapInhibitCache=0,gIOGeneralInterest=0;
static uint64_t fakeTime=0;static int arm=0,failAt=0,allocs=0;void clock_get_uptime(uint64_t*p){*p=fakeTime;}void absolutetime_to_nanoseconds(uint64_t n,uint64_t*p){*p=n;}void OSSynchronizeIO(){}void PE_parse_boot_argn(const char*,int*p,unsigned){*p=arm;}
#define OSDeclareDefaultStructors(x)
#define OSDefineMetaClassAndStructors(x,y)
#define OSDynamicCast(t,x) dynamic_cast<t*>(x)
struct OSObject{virtual ~OSObject()=default;int refs=1;void release(){refs--;assert(refs>=0);}};
static std::vector<std::unique_ptr<OSObject>> arena;
template<class T,class...A>T*make(A...a){if(failAt&&++allocs==failAt)return nullptr;auto p=new T(a...);arena.emplace_back(p);return p;}
struct IONotifier:OSObject{bool removed=false;void remove(){removed=true;}};
struct IOService:OSObject{
 std::map<std::string,uint64_t>numbers;std::map<std::string,std::string>strings;bool published=false;
 virtual bool start(IOService*){return true;}virtual void stop(IOService*){}virtual void free(){}
 void setProperty(const char*k,bool v){numbers[k]=v;}void setProperty(const char*k,uint64_t v,unsigned){numbers[k]=v;}void setProperty(const char*k,const char*v){strings[k]=v;}void setProperty(const char*,OSObject*){}
 void registerService(){published=true;}
 template<class F>IONotifier*registerInterest(int,F,void*){return make<IONotifier>();}
};
struct OSData:OSObject{static OSData*withBytes(const void*,unsigned){return make<OSData>();}};
static IOService domain;IOService*getPMRootDomain(){return &domain;}
struct IOMemoryMap:OSObject{uint32_t*memory;explicit IOMemoryMap(uint32_t*p):memory(p){}size_t getLength(){return 0x400;}uintptr_t getVirtualAddress(){return (uintptr_t)memory;}};
struct IODeviceMemory:OSObject{uint32_t*memory;explicit IODeviceMemory(uint32_t*p):memory(p){}size_t getLength(){return 0x400;}uint64_t getPhysicalAddress(){return 0x100000;}IOMemoryMap*map(int){return make<IOMemoryMap>(memory);}};
struct IOPCIDevice:IOService{uint32_t regs[256]={};bool opened=false,busy=false,memoryOn=true,d3=false;uint16_t vendor=0x8086,device=0x7e45;IODeviceMemory descriptor{regs};
 uint16_t configRead16(uint8_t off){if(off==0)return vendor;if(off==2)return device;if(off==4)return memoryOn?2:0;return d3?3:0;}
 bool open(IOService*){if(busy)return false;assert(!opened);opened=true;return true;}void close(IOService*){assert(opened);opened=false;}
 unsigned findPCICapability(unsigned,uint8_t*p){*p=0x40;return 1;}IODeviceMemory*getDeviceMemoryWithRegister(unsigned){return &descriptor;}
};
struct IOWorkLoop:OSObject{static IOWorkLoop*workLoop(){return make<IOWorkLoop>();}int added=0;int addEventSource(OSObject*){added++;return 0;}void removeEventSource(OSObject*){assert(added>0);added--;}};
struct IOCommandGate:OSObject{OSObject*owner;explicit IOCommandGate(OSObject*p):owner(p){}static IOCommandGate*commandGate(OSObject*p){return make<IOCommandGate>(p);}using Action=int(*)(OSObject*,void*,void*,void*,void*);int runAction(Action f,void*a=nullptr){return f(owner,a,nullptr,nullptr,nullptr);}};
struct IOInterruptEventSource:OSObject{bool enabled=false;void enable(){enabled=true;}void disable(){enabled=false;}};
struct IOFilterInterruptEventSource:IOInterruptEventSource{using Action=void(*)(OSObject*,IOInterruptEventSource*,int);using Filter=bool(*)(OSObject*,IOFilterInterruptEventSource*);static IOFilterInterruptEventSource*filterInterruptEventSource(OSObject*,Action,Filter,IOService*,unsigned){return make<IOFilterInterruptEventSource>();}};
struct IOTimerEventSource:OSObject{bool pending=false;using Tick=void(*)(OSObject*,IOTimerEventSource*);static IOTimerEventSource*timerEventSource(OSObject*,Tick){return make<IOTimerEventSource>();}void cancelTimeout(){pending=false;}void setTimeoutMS(unsigned){pending=true;}};
'''
post=r'''
int main(){
 // The default diagnostic path must not alter even one MMIO register.
 {IOPCIDevice p;p.regs[0x34/4]=3;p.regs[0x38/4]=0x20;p.regs[8/4]=0x101;uint32_t saved[256];memcpy(saved,p.regs,sizeof saved);LatitudeISHTransport s;arm=0;assert(s.start(&p)&&s.published&&!s.armed);assert(!memcmp(saved,p.regs,sizeof saved));s.stop(&p);assert(!p.opened&&!memcmp(saved,p.regs,sizeof saved));s.free();}
 // Armed startup/teardown quiesces only owned IPC bits and never enables DMA.
 {IOPCIDevice p;p.regs[0x34/4]=3;p.regs[0x38/4]=0x20;p.regs[8/4]=0x500;LatitudeISHTransport s;arm=1;assert(s.start(&p)&&s.armed&&s.timer->pending&&s.irq->enabled);assert(p.regs[0x38/4]&0x80);assert(p.regs[8/4]==0x401);assert(p.regs[0x368/4]==0);s.stop(&p);assert(!p.opened&&!s.registers.base&&!s.map&&!s.loop);assert(p.regs[0x38/4]==0x20&&p.regs[8/4]==0x500);s.free();}
 // Every event-source/notifier allocation failure must release provider/mapping.
 for(int failure=1;failure<=6;failure++){IOPCIDevice p;p.regs[0x34/4]=3;LatitudeISHTransport s;allocs=0;failAt=failure;arm=1;bool ok=s.start(&p);if(ok)s.stop(&p);else assert(!p.opened&&!s.map&&!s.loop&&!s.irq&&!s.timer);s.free();}failAt=0;
 {IOPCIDevice p;p.device=0;LatitudeISHTransport s;assert(!s.start(&p)&&!p.opened);s.free();}
 {IOPCIDevice p;p.memoryOn=false;LatitudeISHTransport s;assert(!s.start(&p)&&!p.opened);s.free();}
 {IOPCIDevice p;p.d3=true;LatitudeISHTransport s;assert(!s.start(&p)&&!p.opened);s.free();}
 {IOPCIDevice p;p.regs[0x34/4]=0x10003;LatitudeISHTransport s;arm=1;assert(s.start(&p)&&!s.armed&&p.regs[0x38/4]==0&&p.regs[8/4]==0);s.stop(&p);s.free();}
 {IOPCIDevice p;p.regs[0x34/4]=3;LatitudeISHTransport s;arm=1;assert(s.start(&p));uint32_t sleep=kIOMessageSystemWillSleep;s.powerAction(&s,&sleep,nullptr,nullptr,nullptr);assert(!s.armed&&!s.timer->pending&&!s.irq->enabled&&!s.numbers["LatitudeISHOrientationReady"]);s.stop(&p);s.free();}

 // End-to-end firmware emulator drives the actual IRQ handler, PIO/HBM and
 // descriptor-bound HID frontend. Each whole client message consumes one grant.
 {IOPCIDevice p;p.regs[0x34/4]=3;LatitudeISHTransport s;arm=1;fakeTime=0;assert(s.start(&p));
  struct Frame {unsigned proto,cmd;std::vector<uint8_t> bytes;bool clientStart;};std::vector<Frame> incoming;unsigned grants=0;uint64_t sampleSequence=0;
  auto bus=[&](std::vector<uint8_t>body){std::vector<uint8_t>b={0,0,(uint8_t)body.size(),0x80};b.insert(b.end(),body.begin(),body.end());incoming.push_back({1,0,b,false});};
  auto client=[&](unsigned command,std::vector<uint8_t>body){std::vector<uint8_t>message={(uint8_t)(command|0x80),7,0,0,(uint8_t)body.size(),(uint8_t)(body.size()>>8)};if(command==33)message[1]=0;message.insert(message.end(),body.begin(),body.end());for(unsigned offset=0;offset<message.size();){unsigned n=(unsigned)message.size()-offset;if(n>124)n=124;std::vector<uint8_t>b={5,1,(uint8_t)n,(uint8_t)(offset+n==message.size()?0x80:0)};b.insert(b.end(),message.begin()+offset,message.begin()+offset+n);incoming.push_back({1,0,b,offset==0});offset+=n;}};
  for(unsigned step=0;step<300;step++){
   fakeTime=(uint64_t)step*10000000;
   uint32_t bell=p.regs[0x48/4];if(bell&0x80000000u){unsigned n=bell&0x3ff,protocol=(bell>>10)&15,cmd=(bell>>16)&15;std::vector<uint8_t>b(n);for(unsigned j=0;j<n;j++)b[j]=(uint8_t)(p.regs[(0xe0+j)/4]>>(8*(j%4)));p.regs[0x48/4]=0;
    if(protocol==3){assert(cmd==3);incoming.push_back({3,4,{1,0,0,0},false});}
    else if(!b[0]&&!b[1]){switch(b[4]){
     case 1:bus({0x81,1,0,1});break;
     case 4:{std::vector<uint8_t>e(36);e[0]=0x84;e[4]=1<<5;bus(e);break;}
     case 5:bus({0x85,5,0,0,0x58,0xcd,0xae,0x33,0x79,0xb6,0x54,0x4e,0x9b,0xd9,0xa0,0x4d,0x34,0xf0,0xc2,0x26,1,1,0,1,0,0x20,0,0,0,0,0,0});break;
     case 6:bus({0x86,5,1,0});bus({8,5,1,0,0,0,0,0});break;
     case 8:grants++;break;
     default:assert(false);
    }}else{assert(b[0]==5&&b[1]==1);unsigned command=b[4];
     switch(command){
      case 33:client(command,{1,7,0,0,0,0,0xc2,0x0a,0x87,0x80});break;
      case 0:client(command,{9,0x21});break;
      case 1:client(command,std::vector<uint8_t>(LatitudeISH::measuredAccelDescriptor,LatitudeISH::measuredAccelDescriptor+sizeof LatitudeISH::measuredAccelDescriptor));break;
      case 2:{std::vector<uint8_t>f(289);f[0]=3;f[4]=2;client(command,f);break;}
      case 4:{std::vector<uint8_t>r(136);r[0]=3;r[1]=2;r[2]=5;sampleSequence++;for(unsigned j=0;j<8;j++)r[11+j]=(uint8_t)(sampleSequence>>(8*j));int32_t xyz[3]={0,-708046,-712327};for(unsigned a=0;a<3;a++)for(unsigned j=0;j<4;j++)r[27+a*4+j]=(uint8_t)((uint32_t)xyz[a]>>(8*j));client(command,r);break;}
      default:assert(false); // In particular, HOSTIF_SET_FEATURE is never sent.
     }bus({8,5,1,0,0,0,0,0});
    }
   }
   if(!incoming.empty()&&(!incoming[0].clientStart||grants)){
    auto f=incoming[0];incoming.erase(incoming.begin());if(f.clientStart)grants--;
    for(unsigned j=0;j<128;j+=4)p.regs[(0x60+j)/4]=0;
    for(unsigned j=0;j<f.bytes.size();j++)p.regs[(0x60+j)/4]|=(uint32_t)f.bytes[j]<<(8*(j%4));
    p.regs[0x54/4]=0x80000000u|(f.proto<<10)|(f.cmd<<16)|(unsigned)f.bytes.size();p.regs[0xc/4]=1;
    assert(s.filter(&s,s.irq));s.interruptAction(&s,s.irq,1);p.regs[0xc/4]=0;
   }
   if(step%10==0)LatitudeISHTransport::tick(&s,s.timer);
   assert(s.armed);
  }
  assert(s.numbers["LatitudeISHOrientationReady"]&&(int64_t)s.numbers["LatitudeISHAccelerationY"]< -6900&&s.numbers["LatitudeISHClientMessages"]>5);
  fakeTime+=1100000000;LatitudeISHTransport::tick(&s,s.timer);assert(!s.numbers["LatitudeISHOrientationReady"]);
  s.stop(&p);s.free();
 }
 puts("PASS actual PCI driver source under ASan/UBSan: default zero-write preflight, opt-in start/stop, provider/map/event cleanup failures, foreign-DMA and D3 refusal, sleep disarm, end-to-end credit-limited firmware emulation through descriptor matching and acceleration publication, stale sample expiry; no DMA enable. PCI/IOKit mocked.");
}
'''
with tempfile.TemporaryDirectory() as tmp:
 d=Path(tmp);(d/'test.cpp').write_text(pre+source+post);subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-g','-I'+str(root/'tablet-sources'),str(d/'test.cpp'),'-o',str(d/'test')],check=True);subprocess.run([str(d/'test')],check=True)
