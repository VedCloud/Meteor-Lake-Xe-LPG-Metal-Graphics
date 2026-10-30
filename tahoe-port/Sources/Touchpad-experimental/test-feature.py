from pathlib import Path
import subprocess,tempfile
w=Path(__file__).resolve().parent
src=w/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID'
def function(s,name):
 a=s.index(name);b=s.index('{',a);i=b+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[a:i]
pre=r'''
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <map>
#include <cstdio>
using UInt8=uint8_t;using UInt16=uint16_t;using UInt32=uint32_t;using UInt64=uint64_t;
using IOReturn=int;using IOHIDReportType=int;using IOOptionBits=uint32_t;
constexpr int kIOHIDReportTypeFeature=2,kIOHIDReportTypeInput=0,kIOHIDReportTypeOutput=1;
constexpr int kIOReturnSuccess=0,kIOReturnBadArgument=1,kIOReturnNoMemory=2,kIOReturnUnderrun=3,kIOReturnError=4;
static std::map<void*,size_t> live;static int allocFailAfter=-1;
void*IOMalloc(size_t n){if(allocFailAfter==0)return nullptr;if(allocFailAfter>0)--allocFailAfter;void*p=malloc(n);assert(p);memset(p,0xa5,n);live[p]=n;return p;}
void IOFree(void*p,size_t n){assert(live.at(p)==n);live.erase(p);free(p);}
void IOSleep(unsigned){}
struct IOMemoryDescriptor {std::vector<UInt8>b;bool shortRead=false,shortWrite=false;size_t writes=0;IOMemoryDescriptor(size_t n):b(n,0x71){}size_t getLength(){return b.size();}size_t writeBytes(size_t o,const void*p,size_t n){assert(o+n<=b.size());++writes;size_t v=shortWrite&&n?n-1:n;memcpy(b.data()+o,p,v);return v;}size_t readBytes(size_t o,void*p,size_t n){assert(o+n<=b.size());size_t v=shortRead&&n?n-1:n;memcpy(p,b.data()+o,v);return v;}};
struct API {int result=0;unsigned calls=0;std::vector<UInt8>reply,command;IOReturn writeReadI2C(UInt8*p,UInt16 n,UInt8*b,UInt16 cap){++calls;command.assign(p,p+n);memcpy(b,reply.data(),reply.size()<cap?reply.size():cap);return result;}IOReturn writeI2C(UInt8*p,UInt16 n){++calls;command.assign(p,p+n);return result;}};
struct Descriptor{UInt16 wCommandRegister=5,wDataRegister=6,wVendorID=0x04f3,wProductID=0x328e;};
struct VoodooI2CHIDDevice{API*api;Descriptor hid_descriptor;IOReturn getReport(IOMemoryDescriptor*,IOHIDReportType,IOOptionBits);IOReturn setReport(IOMemoryDescriptor*,IOHIDReportType,IOOptionBits);};
struct OSData {std::vector<UInt8>b;static bool fail;static OSData*withBytes(const void*p,size_t n){if(fail)return nullptr;auto*d=new OSData;d->b.assign((UInt8*)p,(UInt8*)p+n);return d;}void release(){delete this;}};bool OSData::fail=false;
struct Element{UInt32 bits=16,count=1;std::vector<UInt8>stored;UInt32 getReportSize(){return bits;}UInt32 getReportCount(){return count;}void setDataValue(OSData*d){stored=d->b;}};
#define OSSafeReleaseNULL(p) do{if(p){p->release();p=nullptr;}}while(0)
constexpr int INPUT_MODE_TOUCHPAD=3;
struct VoodooI2CPrecisionTouchpadHIDEventDriver{bool ready=false;struct{Element*input_mode=nullptr;}digitiser;void enterPrecisionTouchpadMode();};
'''
post=r'''
int main(){API a;VoodooI2CHIDDevice d;d.api=&a;
 // Exhaust every possible on-wire length with a 3-byte feature request.
 for(unsigned len=0;len<65536;++len){IOMemoryDescriptor m(3);a.reply={(UInt8)len,(UInt8)(len>>8),3,7,8};a.result=0;int r=d.getReport(&m,kIOHIDReportTypeFeature,3);if(len==5){assert(r==0&&m.b[0]==3);for(size_t j=len-2;j<3;++j)assert(m.b[j]==0);}else assert(r!=0&&m.writes==0);assert(live.empty());}
 IOMemoryDescriptor m(3);a.reply={5,0,3,7,8};a.result=77;assert(d.getReport(&m,2,3)==77&&m.writes==0&&m.b[0]==0x71);a.result=0;
 a.reply={5,0,4,7,8};assert(d.getReport(&m,2,3)!=0&&m.writes==0);
 a.reply={5,0,3,7,8};m.shortWrite=true;assert(d.getReport(&m,2,3)==kIOReturnUnderrun);m.shortWrite=false;
 assert(d.getReport(nullptr,2,3)==kIOReturnBadArgument);assert(d.setReport(nullptr,2,3)==kIOReturnBadArgument);
 IOMemoryDescriptor empty(0),tooLarge(65536);assert(d.getReport(&empty,2,3)==1&&d.setReport(&empty,2,3)==1);assert(d.getReport(&tooLarge,2,3)==1&&d.setReport(&tooLarge,2,3)==1);
 allocFailAfter=0;assert(d.getReport(&m,2,3)==2&&d.setReport(&m,2,3)==2);allocFailAfter=1;assert(d.setReport(&m,2,3)==2);allocFailAfter=-1;assert(live.empty());
 // Every report ID, both feature/output reports and payload sizes spanning 255.
 for(unsigned id=0;id<256;++id)for(unsigned n:{1u,2u,255u,256u,300u,1024u})for(int type:{1,2}){
  IOMemoryDescriptor b(n+(id?1:0));for(size_t i=0;i<b.b.size();++i)b.b[i]=(UInt8)(i+19);if(id)b.b[0]=id;else b.b[0]=0;
  assert(d.setReport(&b,type,id)==0);auto&c=a.command;unsigned pos=4;assert(c[2]==((type==2?0x30:0x20)|(id>=15?15:id)));if(id>=15)assert(c[pos++]==id);assert(c[pos++]==6&&c[pos++]==0);unsigned wire=c[pos]|(c[pos+1]<<8);pos+=2;assert(wire==2+(id?1:0)+n);if(id)assert(c[pos++]==id);assert(c.size()==pos+n);assert(!memcmp(c.data()+pos,b.b.data()+(id?1:0),n));assert(live.empty());
  a.reply={5,0,(UInt8)id,9,0};IOMemoryDescriptor out(4);assert(d.getReport(&out,2,id)==0);assert(a.command.size()==(id>=15?7u:6u));if(id>=15)assert(a.command[4]==id);
 }
 IOMemoryDescriptor shortBuf(3);shortBuf.shortRead=true;unsigned calls=a.calls;assert(d.setReport(&shortBuf,2,3)==3&&a.calls==calls&&live.empty());
 IOMemoryDescriptor huge(65535);assert(d.setReport(&huge,2,3)==1&&live.empty());
 // 16-bit Dell mode write must include its high byte, initialized to zero.
 VoodooI2CPrecisionTouchpadHIDEventDriver ptp;ptp.enterPrecisionTouchpadMode();assert(!ptp.ready);Element e;ptp.digitiser.input_mode=&e;ptp.enterPrecisionTouchpadMode();assert(ptp.ready&&e.stored==std::vector<UInt8>({3,0}));e.bits=8;ptp.enterPrecisionTouchpadMode();assert(e.stored==std::vector<UInt8>({3}));for(unsigned bits:{0u,33u}){e.bits=bits;ptp.enterPrecisionTouchpadMode();assert(!ptp.ready);}e.bits=16;OSData::fail=true;ptp.enterPrecisionTouchpadMode();assert(!ptp.ready);assert(live.empty());
 puts("PASS actual functions: all 65536 GET lengths, 256 report IDs, zero-ID payload, extended IDs, >255-byte writes, transport/allocation/descriptor failures, 16-bit mode, and null readiness under ASan/UBSan.");
}
'''
s=(src/'VoodooI2CHIDDevice.cpp').read_text();ptp=(src/'VoodooI2CPrecisionTouchpadHIDEventDriver.cpp').read_text()
with tempfile.TemporaryDirectory()as t:
 p=Path(t);f=p/'test.cpp';f.write_text(pre+function(s,'IOReturn VoodooI2CHIDDevice::getReport(')+function(s,'IOReturn VoodooI2CHIDDevice::setReport(')+function(ptp,'void VoodooI2CPrecisionTouchpadHIDEventDriver::enterPrecisionTouchpadMode(')+post)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-O1',str(f),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
 # Negative controls execute the previous functions against the same protocol expectations.
 old=(w.parent/'Touchpad-CompleteInput-1.0.9/core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID/VoodooI2CHIDDevice.cpp').read_text()
 compat='''struct VoodooI2CHIDDeviceCommand { struct {UInt16 reg;UInt8 report_type_id,opcode;} c; };'''
 cases=[('failed-get',function(old,'IOReturn VoodooI2CHIDDevice::getReport('),'API a;a.result=77;VoodooI2CHIDDevice d;d.api=&a;IOMemoryDescriptor m(3);assert(d.getReport(&m,2,3)==77);assert(m.writes==0);'),('long-set',function(old,'IOReturn VoodooI2CHIDDevice::setReport('),'API a;VoodooI2CHIDDevice d;d.api=&a;IOMemoryDescriptor m(301);m.b[0]=3;assert(d.setReport(&m,2,3)==0);assert(a.command.size()==309);')]
 for name,fun,body in cases:
  f.write_text(pre+compat+fun+'int main(){'+body+'}')
  subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-O1',str(f),'-o',str(p/name)],check=True)
  r=subprocess.run([str(p/name)],capture_output=True,text=True);assert r.returncode!=0,name
  print('PASS negative control:',name,'previous implementation violates assertion')
