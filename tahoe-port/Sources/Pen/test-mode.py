from pathlib import Path
import subprocess,json
b=Path(__file__).resolve().parent;src=b/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID';s=(src/'VoodooI2CTouchscreenHIDEventDriver.cpp').read_text();a=s.index('void VoodooI2CTouchscreenHIDEventDriver::latitudeInitPenMode(');z=s.index('void VoodooI2CTouchscreenHIDEventDriver::latitudeReleasePen(',a);method=s[a:z]
code=r'''
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
using UInt32=unsigned;using IOReturn=int;int kernel_task=0;
const int kIOHIDReportTypeFeature=2,kIOReturnSuccess=0;
struct IOBufferMemoryDescriptor{unsigned char data[3]={};static int live;static bool fail;bool shortRead=false;
 IOBufferMemoryDescriptor(){++live;}static IOBufferMemoryDescriptor* inTaskWithOptions(int,int,int n){assert(n==3);return fail?nullptr:new IOBufferMemoryDescriptor;}
 unsigned writeBytes(int,const void* p,unsigned n){memcpy(data,p,n);return n;}unsigned readBytes(int,void*p,unsigned n){memcpy(p,data,n);return shortRead?2:n;}
 void release(){--live;delete this;}};int IOBufferMemoryDescriptor::live=0;bool IOBufferMemoryDescriptor::fail=false;
struct Device{int gets=0,sets=0,mode=0,index=4,id=14;bool readFail=false,writeFail=false,verifyFail=false,shortRead=false;std::vector<std::vector<unsigned char>>sent;
 int getReport(IOBufferMemoryDescriptor*p,int type,int report){assert(type==2&&report==14);++gets;if(readFail|| (verifyFail&&gets==2))return -3;p->data[0]=id;p->data[1]=mode;p->data[2]=index;p->shortRead=shortRead;return 0;}
 int setReport(IOBufferMemoryDescriptor*p,int type,int report){assert(type==2&&report==14);++sets;sent.push_back({p->data[0],p->data[1],p->data[2]});if(writeFail&&sets==1)return -4;mode=p->data[1];index=p->data[2];return 0;}
};
struct VoodooI2CTouchscreenHIDEventDriver{bool latitude_pen_qualified=true;Device*hid_device=nullptr;std::string status;
 template<class T,class U>void setProperty(const char*,T,U){}void setProperty(const char*k,const char*v){if(std::string(k)=="LatitudePenInputModeStatus")status=v;}
 void latitudeInitPenMode();};
'''+method+r'''
int main(){
 for(int scenario=0;scenario<12;scenario++){
  Device d;VoodooI2CTouchscreenHIDEventDriver v;v.hid_device=&d;
  if(scenario==1)d.mode=2;if(scenario==2)d.id=13;if(scenario==3)d.mode=11;if(scenario==4)d.index=11;
  if(scenario==5)d.readFail=true;if(scenario==6)d.writeFail=true;if(scenario==7)d.verifyFail=true;if(scenario==8)d.shortRead=true;if(scenario==9)v.latitude_pen_qualified=false;
  if(scenario==10||scenario==11){d.id=255;d.mode=255;d.index=255;d.writeFail=scenario==11;}
  v.latitudeInitPenMode();assert(IOBufferMemoryDescriptor::live==0);
  if(scenario<2){assert(d.sets==1&&d.gets==2&&d.mode==2&&d.index==4&&v.status=="native-mode-verified");assert(d.sent[0]==std::vector<unsigned char>({14,2,4}));}
  else if(scenario==6||scenario==7){assert(d.sets==2&&d.mode==0&&d.index==4);assert(d.sent[1]==std::vector<unsigned char>({14,0,4}));}
  else if(scenario==10||scenario==11){assert(d.sets==1&&d.gets==1);assert(d.sent[0]==std::vector<unsigned char>({14,2,0}));assert(v.status==(scenario==10?"native-mode-written-no-readback":"set-only-write-failed"));}
  else assert(d.sets==0);
 }
 Device d;VoodooI2CTouchscreenHIDEventDriver v;v.hid_device=&d;IOBufferMemoryDescriptor::fail=true;v.latitudeInitPenMode();assert(!d.gets&&!d.sets&&v.status=="allocation-failed");
 puts("PASS actual input-mode initializer: reassert mode 2, preserve device index, malformed/short/read/write/verify failures, rollback, target guard and no buffer leaks");}
'''
f=b/'test-mode.cpp';f.write_text(code);subprocess.run(['xcrun','clang++','-std=c++14','-fsanitize=address,undefined',str(f),'-o',str(b/'test-mode')],check=True);r=subprocess.run([str(b/'test-mode')],text=True,capture_output=True,check=True);print(r.stdout,end='');(b/'MODE-TESTS.json').write_text(json.dumps({'actualInitializerReplay':r.stdout.strip(),'hardwareVerified':False},indent=2)+'\n')
