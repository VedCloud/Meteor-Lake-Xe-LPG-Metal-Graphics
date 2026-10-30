from pathlib import Path
import subprocess,json
b=Path(__file__).resolve().parent;s=(b/'test-routing.cpp').read_text();s=s.replace('struct Timer {int cancels=0;','struct Timer {int timeout=0;void setTimeoutMS(int n){timeout=n;}int cancels=0;')
r=b/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID';device=(r/'VoodooI2CHIDDevice.cpp').read_text();a=device.index('void VoodooI2CHIDDevice::simulateInterrupt(');z=device.index('\nbool VoodooI2CHIDDevice::open(',a);body=device[a:z]
insert=r'''
struct OSObject{};using IOTimerEventSource=Timer;using uint64_t=UInt64;
#define INTERRUPT_SIMULATOR_TIMEOUT 5
#define INTERRUPT_SIMULATOR_TIMEOUT_BUSY 2
#define INTERRUPT_SIMULATOR_TIMEOUT_IDLE 50
#define SUB_ABSOLUTETIME(a,b) (*(a)-=*(b))
void IOSleep(unsigned n){fakeClock+=static_cast<UInt64>(n)*1000000ULL;}
struct VoodooI2CHIDDevice {
 struct{unsigned wVendorID=0x56a,wProductID=0x4a19;}hid_descriptor;unsigned latitude_input_failure_streak=0;
 bool awake=true,deliver=true;Timer timer;Timer* interrupt_simulator=&timer;
 VoodooI2CTouchscreenHIDEventDriver* pen;
 void getInputReport(){if(deliver){IOMemoryDescriptor r;r.data[0]=20;r.data[1]=32;pen->handleInterruptReport(fakeClock,&r,0,20);}}
 void simulateInterrupt(OSObject*,IOTimerEventSource*);
};
'''+body+r'''
int main(){VoodooI2CTouchscreenHIDEventDriver pen;pen.latitude_pen_qualified=true;
 VoodooI2CHIDDevice device;device.pen=&pen;
 fakeClock=2000000000ULL;last_multi_touch_event=1;device.deliver=false;
 device.simulateInterrupt(nullptr,nullptr);assert(device.timer.timeout==50);
 fakeClock=4000000000ULL;device.deliver=true;
 device.simulateInterrupt(nullptr,nullptr);assert(device.timer.timeout==2);
 fakeClock+=10000000ULL;device.simulateInterrupt(nullptr,nullptr);assert(device.timer.timeout==2);
 puts("PASS actual unchanged transport: idle 50ms; native pen traffic restores 2ms busy polling");}
'''
s=s[:s.index('int main(){')]+insert;(b/'test-polling.cpp').write_text(s)
subprocess.run(['xcrun','clang++','-std=c++11','-fsanitize=address,undefined','-I'+str(r),str(b/'test-polling.cpp'),'-o',str(b/'test-polling')],check=True)
p=subprocess.run([str(b/'test-polling')],text=True,capture_output=True,check=True);print(p.stdout,end='');f=b/'TESTS.json';d=json.loads(f.read_text());d['actualTransportPollingReplay']=p.stdout.strip();f.write_text(json.dumps(d,indent=2)+'\n')
