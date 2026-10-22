from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
s=(r/'research/voodooi2c-core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID/VoodooI2CHIDDevice.cpp').read_text()
def function(s,name):
 a=s.index('void VoodooI2CHIDDevice::'+name+'(');b=s.index('{',a);i=b+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[a:i]
pre=r'''
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
using UInt16=uint16_t;using IOReturn=int;using AbsoluteTime=uint64_t;
constexpr int kIOReturnSuccess=0,kIOHIDReportTypeInput=0,INTERRUPT_SIMULATOR_TIMEOUT=10,INTERRUPT_SIMULATOR_TIMEOUT_BUSY=2,INTERRUPT_SIMULATOR_TIMEOUT_IDLE=10;
struct OSObject{}; struct IOTimerEventSource{unsigned ms=0;void setTimeoutMS(unsigned n){ms=n;}};
static bool allocation_fail=false,descriptor_fail=false,short_write=false;static unsigned live=0;
static void*IOMalloc(size_t n){if(allocation_fail)return nullptr;auto p=malloc(n);memset(p,1,n);live++;return p;}
static void IOFree(void*p,size_t){assert(live);live--;free(p);}
static void IOSleep(unsigned){};static void IOLog(const char*,...){};
static void clock_get_uptime(uint64_t*p){*p=2000000000;}static void absolutetime_to_nanoseconds(uint64_t a,uint64_t*p){*p=a;}
#define SUB_ABSOLUTETIME(p,q) (*(p)-=*(q))
#define OSSafeReleaseNULL(x) do{if(x){x->release();x=nullptr;}}while(0)
static int kernel_task=0;
struct IOBufferMemoryDescriptor{std::vector<unsigned char>v;explicit IOBufferMemoryDescriptor(unsigned n):v(n,0xcc){}
static IOBufferMemoryDescriptor*inTaskWithOptions(int,int,unsigned n){if(descriptor_fail)return nullptr;assert(n<1000);return new IOBufferMemoryDescriptor(n);}
unsigned writeBytes(unsigned at,const unsigned char*p,unsigned n){assert(at+n<=v.size());if(short_write)return 0;memcpy(v.data()+at,p,n);return n;}void release(){delete this;}};
struct API{int rc=0;unsigned length=3,calls=0;IOReturn readI2C(unsigned char*p,unsigned n){calls++;if(rc)return rc;assert(n>=2);p[0]=length;p[1]=length>>8;for(unsigned i=2;i<n;i++)p[i]=(unsigned char)i;return 0;}};
struct Gate{unsigned wakes=0;void commandWakeup(bool*){wakes++;}};
struct VoodooI2CHIDDevice{
 struct{UInt16 wMaxInputLength=31,wVendorID=0x4f3,wProductID=0x328e;}hid_descriptor;
 uint64_t latitude_input_reads=0,latitude_input_failures=0,latitude_input_malformed=0,latitude_input_delivered=0;unsigned latitude_input_failure_streak=0;
 API*api;Gate*command_gate;IOTimerEventSource*interrupt_simulator;bool reset_event=false,ready_for_input=true,awake=true;uint64_t last_multi_touch_event=0;unsigned handled=0;
 void setProperty(const char*,uint64_t,unsigned){}const char*getName(){return "fixture";}const char*name="fixture";
 int handleReport(IOBufferMemoryDescriptor*b,int){assert(b->v.size()==api->length-2);for(unsigned i=0;i<b->v.size();i++)assert(b->v[i]==(unsigned char)(i+2));handled++;return 0;}
 void getInputReport();void simulateInterrupt(OSObject*,IOTimerEventSource*);
};
'''
post=r'''
int main(){
 for(unsigned length=0;length<65536;length++){
  API a;a.length=length;Gate g;IOTimerEventSource t;VoodooI2CHIDDevice d;d.api=&a;d.command_gate=&g;d.interrupt_simulator=&t;d.getInputReport();
  assert(d.handled==(length>2&&length<=31?1u:0u));assert(g.wakes==(length==0?1u:0u));assert(!live);
 }
 API a;Gate g;IOTimerEventSource t;VoodooI2CHIDDevice d;d.api=&a;d.command_gate=&g;d.interrupt_simulator=&t;
 a.rc=42;for(unsigned i=0;i<20;i++)d.simulateInterrupt(nullptr,&t);
 assert(d.handled==0&&g.wakes==0&&d.latitude_input_failures==20&&d.latitude_input_failure_streak==16&&t.ms==250&&!live);
 a.rc=0;a.length=3;d.simulateInterrupt(nullptr,&t);assert(d.handled==1&&d.latitude_input_failure_streak==0);
 allocation_fail=true;d.getInputReport();allocation_fail=false;assert(!live);
 descriptor_fail=true;d.getInputReport();descriptor_fail=false;assert(!live);
 short_write=true;d.getInputReport();short_write=false;assert(!live);
 d.ready_for_input=false;d.getInputReport();assert(d.handled==1);d.ready_for_input=true;
 d.hid_descriptor.wMaxInputLength=1;unsigned calls=a.calls;d.getInputReport();assert(a.calls==calls&&!live);
 puts("PASS actual HID functions ASan/UBSan: all65536 wire lengths, failed reads, no false reset/report, exact initialized payload, allocation/short-write failure, readiness/capacity guard, bounded polling backoff and recovery.");
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);src=p/'test.cpp';src.write_text(pre+function(s,'getInputReport')+function(s,'simulateInterrupt')+post)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-O1',str(src),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
 old=(r/'backups/input-recovery-source-20261021/VoodooI2CHID/VoodooI2CHIDDevice.cpp').read_text()
 negative='int main(){API a;a.length=1;Gate g;VoodooI2CHIDDevice d;d.api=&a;d.command_gate=&g;d.getInputReport();}'
 src.write_text(pre+function(old,'getInputReport')+negative)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-O1',str(src),'-o',str(p/'old')],check=True)
 result=subprocess.run([str(p/'old')],capture_output=True,text=True)
 assert result.returncode!=0 and ('Assertion failed' in result.stderr or 'AddressSanitizer' in result.stderr),result.stderr
 print('PASS negative control: previous actual report function fails on length1 underflow.')
