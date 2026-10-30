from pathlib import Path
import subprocess,json
b=Path(__file__).resolve().parent;s=(b/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID/VoodooI2CHIDDevice.cpp').read_text();a=s.index('IOReturn VoodooI2CHIDDevice::getReport(');z=s.index('\nvoid VoodooI2CHIDDevice::interruptOccured',a);get=s[a:z];a=s.index('IOReturn VoodooI2CHIDDevice::setReport(');z=s.index('\nvoid VoodooI2CHIDDevice::lookupQuirks',a);put=s[a:z]
code=r'''
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
using UInt8=unsigned char;using UInt16=unsigned short;using IOReturn=int;using IOOptionBits=unsigned;using IOHIDReportType=int;
const int kIOHIDReportTypeInput=0,kIOHIDReportTypeOutput=1,kIOHIDReportTypeFeature=2,kIOReturnBadArgument=-1;
static int allocations=0;void* IOMalloc(unsigned n){++allocations;return malloc(n);}void IOFree(void*p,unsigned){--allocations;free(p);}void IOSleep(int){}
struct IOMemoryDescriptor{unsigned char data[3]={14,2,4};unsigned getLength(){return 3;}unsigned readBytes(unsigned,void*p,unsigned n){memcpy(p,data,n);return n;}unsigned writeBytes(unsigned,const void*p,unsigned n){memcpy(data,p,n);return n;}};
struct VoodooI2CHIDDeviceCommand{struct __attribute__((packed)){UInt16 reg;UInt8 report_type_id,opcode;}c;};
struct API{std::vector<UInt8> command;int writeReadI2C(UInt8*p,unsigned n,UInt8*out,unsigned want){assert(want==5);command.assign(p,p+n);UInt8 response[]={5,0,14,0,4};memcpy(out,response,5);return 0;}int writeI2C(UInt8*p,unsigned n){command.assign(p,p+n);return 0;}};
struct VoodooI2CHIDDevice{struct{UInt16 wDataRegister=5,wCommandRegister=4;}hid_descriptor;API*api;int getReport(IOMemoryDescriptor*,int,unsigned);int setReport(IOMemoryDescriptor*,int,unsigned);};
'''+get+put+r'''
int main(){API api;VoodooI2CHIDDevice d;d.api=&api;IOMemoryDescriptor report;
 assert(d.getReport(&report,2,14)==0);assert(api.command==std::vector<UInt8>({4,0,0x3e,2,5,0}));assert(report.data[0]==14&&report.data[1]==0&&report.data[2]==4);
 report.data[1]=2;assert(d.setReport(&report,2,14)==0);assert(api.command==std::vector<UInt8>({4,0,0x3e,3,5,0,5,0,14,2,4}));assert(!allocations);
 puts("PASS actual unchanged I2C feature transport: GET/SET 0x0e wire bytes, length prefixes, single report ID and device-index preservation");}
''';f=b/'test-feature-wire.cpp';f.write_text(code);subprocess.run(['xcrun','clang++','-std=c++14','-fsanitize=address,undefined',str(f),'-o',str(b/'test-feature-wire')],check=True);r=subprocess.run([str(b/'test-feature-wire')],text=True,capture_output=True,check=True);print(r.stdout,end='');f=b/'MODE-TESTS.json';d=json.loads(f.read_text());d['actualFeatureWireReplay']=r.stdout.strip();f.write_text(json.dumps(d,indent=2)+'\n')
