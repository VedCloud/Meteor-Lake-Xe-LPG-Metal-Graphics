from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
pre=r'''
#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
using IOReturn=int;
enum {kIOReturnSuccess=0,kIOReturnUnsupported=1,kIOReturnNotPrivileged=2,kIOReturnBadArgument=3,kIOReturnNotReady=4,kIOReturnNoMemory=5};
static bool privileged=true,failAlloc=false;static int publication=0;
struct OSObject { virtual ~OSObject(){};void release(){};};
static std::vector<std::unique_ptr<OSObject>> allocations;
template<class T,class...A>T *make(A...args){if(failAlloc)return nullptr;auto p=new T(args...);allocations.emplace_back(p);return p;}
#define OSDynamicCast(T,x) dynamic_cast<T *>(x)
#define OSSafeReleaseNULL(x) do{if(x)(x)->release();x=nullptr;}while(0)
struct OSNumber:OSObject{unsigned long long n;OSNumber(unsigned long long v):n(v){};unsigned long long unsigned64BitValue(){return n;}static OSNumber*withNumber(unsigned long long n,int){return make<OSNumber>(n);}};
struct OSString:OSObject{std::string s;OSString(std::string a):s(a){};const char*getCStringNoCopy(){return s.c_str();}unsigned getLength(){return (unsigned)s.size();}};
struct OSData:OSObject{std::string s;OSData(const void*b,unsigned n):s((const char*)b,n){};static OSData*withBytes(const void*b,unsigned n){return make<OSData>(b,n);}};
struct OSDictionary:OSObject{std::map<std::string,OSObject*>d;OSObject*getObject(const char*k){return d[k];}};
static OSObject yes;
#define kOSBooleanTrue (&yes)
struct IORegistryEntry:OSObject{std::map<std::string,OSObject*>d;bool setProperty(const char*k,OSObject*v){d[k]=v;publication++;return true;}OSObject*getProperty(const char*k){return d[k];}};
struct IOService:IORegistryEntry{IOService*p=nullptr;IOService*getProvider(){return p;}};
struct IOPCIDevice:IOService{unsigned vendor=0x8086,device=0x7d55;unsigned configRead16(unsigned offset){return offset?device:vendor;}};
struct IOFramebuffer:IOService{virtual IOReturn setProperties(OSObject*){return kIOReturnUnsupported;}};
struct NMXVGAFB:IOFramebuffer{bool fPanelMemoryValidated=true;IOService*fAccel=nullptr;IOReturn setProperties(OSObject*)override;};
static int current_task(){return 1;}
static constexpr int kIOClientPrivilegeAdministrator=1;
struct IOUserClient{static int clientHasPrivilege(int,int){return privileged?kIOReturnSuccess:kIOReturnNotPrivileged;}};
#define VFBLOG(...) do{}while(0)
#define LATITUDE_MTL_PANEL 1
#include "latitude_gpu_identity.h"
#include "latitude_gpu_identity_kernel.inc"
int main(){
 IOPCIDevice pci;IOService claim,accel;NMXVGAFB fb;claim.p=&pci;fb.p=&claim;fb.fAccel=&accel;pci.d["nmxgpu-claimed"]=&yes;
 OSDictionary report,outer;outer.d["LatitudeGPUIdentity"]=&report;
 report.d["version"]=make<OSNumber>(1);report.d["vendor"]=make<OSNumber>(0x8086);report.d["device"]=make<OSNumber>(0x7d55);
 report.d["name"]=make<OSString>(std::string("queried device identity"));report.d["physicalRAMBytes"]=make<OSNumber>(16ull<<30);
 for(unsigned long long heap: {1ull<<20,1537ull<<20,3ull<<30,8ull<<30,12ull<<30,16ull<<30}){
  report.d["sharedHeapBytes"]=make<OSNumber>(heap);assert(fb.setProperties(&outer)==kIOReturnSuccess);
  for(IORegistryEntry*entry:{(IORegistryEntry*)&pci,(IORegistryEntry*)&fb,(IORegistryEntry*)&accel}){
   assert(dynamic_cast<OSNumber*>(entry->d["VRAM,totalMB"])->n==(heap>>20));
   assert(dynamic_cast<OSNumber*>(entry->d["NMXSharedMemoryBytes"])->n==heap);
   assert(dynamic_cast<OSData*>(entry->d["model"])->s==std::string("queried device identity\0",24));
   assert(!entry->d.count("IOFBMemorySize"));
  }
 }
 int before=publication;
 privileged=false;assert(fb.setProperties(&outer)==kIOReturnNotPrivileged);privileged=true;
 pci.device=0x1234;assert(fb.setProperties(&outer)==kIOReturnNotReady);pci.device=0x7d55;
 fb.fPanelMemoryValidated=false;assert(fb.setProperties(&outer)==kIOReturnNotReady);fb.fPanelMemoryValidated=true;
 pci.d["nmxgpu-claimed"]=nullptr;assert(fb.setProperties(&outer)==kIOReturnNotReady);pci.d["nmxgpu-claimed"]=&yes;
 assert(fb.setProperties(nullptr)==kIOReturnUnsupported);
 for(auto key:{"version","vendor","device","name","sharedHeapBytes","physicalRAMBytes"}){auto old=report.d[key];report.d[key]=&yes;assert(fb.setProperties(&outer)==kIOReturnBadArgument);report.d[key]=old;}
 auto original=report.d["name"];
 for(auto text:{std::string(""),std::string(256,'x'),std::string("bad\nname"),std::string("bad\0name",8)}){report.d["name"]=make<OSString>(text);assert(fb.setProperties(&outer)==kIOReturnBadArgument);}report.d["name"]=original;
 for(auto value:{0ull,1ull,17ull<<30,~0ull}){report.d["sharedHeapBytes"]=make<OSNumber>(value);assert(fb.setProperties(&outer)==kIOReturnBadArgument);}
 report.d["sharedHeapBytes"]=make<OSNumber>(3ull<<30);
 report.d["vendor"]=make<OSNumber>(0x100008086ull);assert(fb.setProperties(&outer)==kIOReturnBadArgument);report.d["vendor"]=make<OSNumber>(0x8086);
 failAlloc=true;assert(fb.setProperties(&outer)==kIOReturnNoMemory);failAlloc=false;
 assert(publication==before);
 puts("PASS actual kernel reporting method ASan/UBSan: queried capacities including >4GiB, name bytes, PCI/FB/accelerator mirrors, root/device/claim/type/bounds/OOM guards; no physical mapping or IOFBMemorySize change");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(pre)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-g','-I',str(r/'desktop-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
