from pathlib import Path
import subprocess,json
b=Path(__file__).resolve().parent;src=b/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID';s=(src/'VoodooI2CTouchscreenHIDEventDriver.cpp').read_text()
a=s.index('void VoodooI2CTouchscreenHIDEventDriver::latitudeReleasePen(');z=s.index('bool VoodooI2CTouchscreenHIDEventDriver::handleStart(',a);methods=s[a:z]
prefix=r'''
#include <cassert>
#include <cstdio>
#include <cstring>
#include "LatitudeWacomPen.hpp"
using UInt64=unsigned long long;using UInt32=unsigned;using UInt8=unsigned char;
using AbsoluteTime=UInt64;using IOFixed=int;using IOHIDReportType=unsigned;
const unsigned kIOHIDReportTypeInput=0,kDigitiserTransducerFinger=2,kDigitiserTransducerStylus=0;
const char* kIOFBTransformKey="transform";
struct OSNumber {unsigned8BitValue(){return 0;} };
#define OSDynamicCast(type,x) static_cast<type*>(x)
UInt64 last_multi_touch_event=0, fakeClock=0;
void clock_get_uptime(UInt64* t){*t=++fakeClock;}
void absolutetime_to_nanoseconds(AbsoluteTime t,UInt64* v){*v=t;}
struct IOMemoryDescriptor {unsigned char data[24]={};unsigned length=22;bool shortRead=false;
 unsigned getLength(){return length;}unsigned readBytes(unsigned,void* p,unsigned n){memcpy(p,data,n);return shortRead?n-1:n;}};
struct Timer {int cancels=0;void cancelTimeout(){++cancels;}};
struct Framebuffer {void retain(){} OSNumber* getProperty(const char*){return nullptr;}};
struct Base {int fallback=0;void handleInterruptReport(AbsoluteTime,IOMemoryDescriptor*,IOHIDReportType,UInt32){++fallback;}};
struct VoodooI2CTouchscreenHIDEventDriver:Base {
 bool latitude_pen_qualified=false,latitude_pen_in_range=false,allowed=true;
 IOFixed latitude_pen_x=0,latitude_pen_y=0; UInt64 latitude_pen_reports=0,latitude_pen_tips=0,latitude_pen_rejected=0,latitude_pen_timestamp_ns=0;
 Timer t,d,s;Timer* timer_source=&t;Timer* drag_timer_source=&d;Timer* scroll_timer_source=&s;
 Framebuffer* active_framebuffer=nullptr;UInt8 current_rotation=0;
 UInt64 latitude_hid_report_counts[32]={},latitude_hid_publish_ns=0;
 UInt64 latitude_pen_raw_tips=0,latitude_pen_raw_range=0,latitude_pen_pressure_reports=0;
 UInt64 latitude_pen_interval_samples=0,latitude_pen_interval_ns=0,latitude_pen_interval_max_ns=0,latitude_pen_gaps_over32ms=0;
 UInt32 latitude_pen_last_buttons=0;
 UInt32 latitude_pen_flags_or=0,latitude_pen_pressure_max=0,latitude_pen_last_flags=0,latitude_pen_last_pressure=0;
 bool is_dragging=false,is_right_clicking=false;unsigned last_id=0;int last_x=0,last_y=0;
 int releases=0;int dispatches=0,resets=0,lastButtons=-1,lastX=-1,lastY=-1;bool lastRange=false;
 bool latitudePenAllowed(){return allowed;} Framebuffer* getFramebuffer(){return nullptr;}
 void resetTouch(){++resets;is_dragging=is_right_clicking=false;}void resetScroll(){}
 void checkRotation(IOFixed*,IOFixed*){}
 template<class T>void setProperty(const char*,T){}template<class T,class U>void setProperty(const char*,T,U){}
 void dispatchDigitizerEventWithTiltOrientation(AbsoluteTime,unsigned,unsigned,bool range,unsigned buttons,int x,int y,int=0,int=0,int=0,int=0,int=0,int=0){if(range&&!buttons)++releases;++dispatches;lastRange=range;lastButtons=buttons;lastX=x;lastY=y;}
 void latitudeReleasePen(AbsoluteTime);void handleInterruptReport(AbsoluteTime,IOMemoryDescriptor*,IOHIDReportType,UInt32);
};
#define super Base
'''.replace('unsigned8BitValue()','unsigned unsigned8BitValue()')
main=r'''
int main(){
 VoodooI2CTouchscreenHIDEventDriver v;IOMemoryDescriptor r;r.data[0]=20;r.data[1]=32;
 v.handleInterruptReport(1,&r,0,20);assert(v.fallback==1&&v.dispatches==0);
 v.latitude_pen_qualified=true;
 v.handleInterruptReport(2,&r,0,28);v.handleInterruptReport(3,&r,1,20);assert(v.fallback==3);
 v.handleInterruptReport(4,&r,0,20);assert(v.dispatches==1&&v.latitude_pen_in_range&&v.lastButtons==0&&v.t.cancels==1);
 r.data[1]=33;r.data[2]=0xc0;r.data[3]=0x72;r.data[4]=0x8c;r.data[5]=0x40;r.data[6]=0xff;r.data[7]=15;
 v.handleInterruptReport(5,&r,0,20);assert(v.lastButtons==1&&v.lastX==65535&&v.lastY==65535&&v.latitude_pen_tips==1);
 r.data[1]=0;r.data[2]=r.data[3]=r.data[4]=r.data[5]=0;
 v.handleInterruptReport(6,&r,0,20);assert(!v.lastRange&&!v.latitude_pen_in_range&&v.lastButtons==0&&v.lastX==65535&&v.releases==2);
 int n=v.dispatches;v.handleInterruptReport(7,&r,0,20);assert(v.dispatches==n);
 r.length=4;v.handleInterruptReport(8,&r,0,20);assert(v.latitude_pen_rejected==1&&v.dispatches==n);
 r.length=22;r.shortRead=true;v.handleInterruptReport(9,&r,0,20);assert(v.latitude_pen_rejected==2);
 r.shortRead=false;r.data[0]=11;r.data[1]=33;
 v.handleInterruptReport(10,&r,0,11);assert(v.lastRange&&v.lastButtons==1);
 v.allowed=false;v.handleInterruptReport(11,&r,0,11);assert(!v.lastRange&&!v.lastButtons&&!v.latitude_pen_in_range);
 n=v.dispatches;v.handleInterruptReport(12,&r,0,11);assert(v.dispatches==n);
 v.allowed=true;r.data[1]=1; // Contact with proximity bit clear.
 v.handleInterruptReport(13,&r,0,11);assert(v.lastRange&&v.lastButtons==1&&v.latitude_pen_raw_tips==3);
 assert(last_multi_touch_event>0);
 r.data[1]=32;r.data[6]=255;r.data[7]=15; // Pressure alone must never create a click.
 v.handleInterruptReport(14,&r,0,11);assert(v.lastRange&&v.lastButtons==0);
 assert(v.latitude_pen_last_pressure==4095&&v.latitude_pen_pressure_max==4095);
 assert(v.latitude_pen_last_flags==32&&v.latitude_hid_report_counts[28]==1);
 puts("PASS actual adapter: mixed reports, hover/contact/lift, vendor report, short read, disabled input held-button release, contact-only proximity, pressure-not-click and adaptive polling activity");
}
'''
(b/'test-routing.cpp').write_text(prefix+methods+main)
subprocess.run(['xcrun','clang++','-std=c++11','-fsanitize=address,undefined','-I'+str(src),str(b/'test-routing.cpp'),'-o',str(b/'test-routing')],check=True)
p=subprocess.run([str(b/'test-routing')],text=True,capture_output=True,check=True);print(p.stdout,end='');f=b/'TESTS.json';d=json.loads(f.read_text());d['actualAdapterReplay']=p.stdout.strip();f.write_text(json.dumps(d,indent=2)+'\n')
