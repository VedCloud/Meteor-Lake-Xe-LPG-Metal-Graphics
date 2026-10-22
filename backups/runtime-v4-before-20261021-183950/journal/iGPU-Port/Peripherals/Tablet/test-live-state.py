from pathlib import Path
import ast,subprocess,tempfile
root=Path(__file__).resolve().parent.parent
module=ast.parse((root/'tablet-sources/test-tablet-observer.py').read_text());pre=next(ast.literal_eval(n.value) for n in module.body if isinstance(n,ast.Assign) and any(isinstance(x,ast.Name) and x.id=='pre' for x in n.targets))
pre=pre.replace('void clock_get_uptime(uint64_t*v){*v=1234;}void absolutetime_to_nanoseconds(uint64_t t,uint64_t*v){*v=t*1000;}','static uint64_t fakeNS=0;void clock_get_uptime(uint64_t*v){*v=fakeNS;}void absolutetime_to_nanoseconds(uint64_t t,uint64_t*v){*v=t;}')
a=pre.index('struct IOACPIPlatformDevice:');b=pre.index('struct IOCommandGate;',a)
pre=pre[:a]+r'''
constexpr int kIOReturnBadArgument=7,kIOMessageSystemWillSleep=999,kIOMessageSystemHasPoweredOn=1000;
struct OSNumber:OSObject{uint64_t n;OSNumber(uint64_t x):n(x){}uint64_t unsigned64BitValue(){return n;}};
struct OSArray:OSObject{std::vector<OSObject*>items;unsigned getCount(){return items.size();}OSObject*getObject(unsigned i){return items.at(i);}};
struct IOACPIPlatformDevice:IOService {
 std::string name="HIDD";bool badVersion=false;unsigned rawByte=0;bool valid=true,failRead=false,badPackage=false;
 bool compareName(OSString*n,OSString**){return n->s==name;}
 IOReturn evaluateInteger(const char*n,UInt64*v){assert(std::string(n)=="LTSV");*v=badVersion?1:2;return 0;}
 IOReturn evaluateObject(const char*n,OSObject**v){assert(std::string(n)=="LTSM");if(failRead){*v=nullptr;return 42;}auto a=make<OSArray>();a->items={make<OSNumber>(0x4c545331),make<OSNumber>(valid),make<OSNumber>(rawByte)};if(badPackage)a->items.pop_back();*v=a;return 0;}
};
static IOService root;IOService *getPMRootDomain(){return &root;}
''' + pre[b:]
pre=pre.replace('IOReturn addEventSource(IOCommandGate*);void removeEventSource(IOCommandGate*);','IOReturn addEventSource(OSObject*);void removeEventSource(OSObject*);')
a=pre.index('IOReturn IOWorkLoop::addEventSource(');pre=pre[:a]+r'''
struct IOTimerEventSource:OSObject {bool pending=false;unsigned interval=0;using Tick=void(*)(OSObject*,IOTimerEventSource*);static IOTimerEventSource*timerEventSource(OSObject*,Tick){return make<IOTimerEventSource>();}void setTimeoutMS(unsigned n){pending=true;interval=n;}void cancelTimeout(){pending=false;}};
IOReturn IOWorkLoop::addEventSource(OSObject*o){if(addFail)return 1;if(auto g=dynamic_cast<IOCommandGate*>(o))g->added=true;return 0;}
void IOWorkLoop::removeEventSource(OSObject*o){if(auto g=dynamic_cast<IOCommandGate*>(o)){assert(g->added);g->added=false;}}
'''
source=(root/'tablet-sources/LatitudeTabletState.cpp').read_text();source='\n'.join(s for s in source.splitlines() if not s.startswith('#include <'));source=source.replace('class LatitudeTabletState:public IOService {','struct LatitudeTabletState:public IOService {')
post=r'''
#include "LatitudeInputSuppression.h"
int main(){
 for(unsigned failure=0;failure<6;failure++){
  IOACPIPlatformDevice d;LatitudeTabletState s;loopFail=failure==1;addFail=failure==2;notifierFail=failure==3;d.badVersion=failure==4;if(failure==5)d.name="WMI0";
  bool ok=s.start(&d);assert(ok==(failure==0));if(!ok){s.free();assert(!s.active&&!s.loop&&!s.gate&&!s.timer);continue;}
  auto step=[&](unsigned ms){fakeNS=ms*1000000ull;LatitudeTabletState::tick(&s,s.timer);};
  assert(s.nums["LatitudeTabletInputSuppressionEnabled"]);
  auto lease=[&](){return LatitudeTablet::suppressionLease(fakeNS,s.nums["LatitudeTabletInputSuppressionEnabled"],s.nums["LatitudeTabletModeQualified"],s.nums["LatitudeTabletModeCurrent"],s.nums["LatitudeTabletModeHeartbeatUptimeNs"]);};
  step(100);assert(!lease());assert(s.nums["LatitudeTabletModeQualified"]==1&&s.nums["LatitudeTabletModeCurrent"]==0);
  d.rawByte=1;step(1100);assert(!s.nums["LatitudeTabletModeQualified"]); // no fold event
  UInt32 cc=0xcc;fakeNS=1200*1000000ull;s.notify(&s,nullptr,kIOACPIMessageDeviceNotification,&d,&cc,4);
  step(1300);assert(!s.nums["LatitudeTabletModeQualified"]);step(2300);assert(s.nums["LatitudeTabletModeQualified"]&&s.nums["LatitudeTabletModeCurrent"]);
  assert(lease());fakeNS+=1600000000ull;assert(!lease());fakeNS-=1600000000ull;
  UInt32 cd=0xcd;s.notify(&s,nullptr,kIOACPIMessageDeviceNotification,&d,&cd,4);assert(!s.nums["LatitudeTabletModeCurrent"]&&!lease());step(2400);assert(!s.nums["LatitudeTabletModeQualified"]);
  d.rawByte=0;step(2500);assert(s.nums["LatitudeTabletModeQualified"]&&!s.nums["LatitudeTabletModeCurrent"]);
  d.badPackage=true;step(3500);assert(!s.nums["LatitudeTabletModeQualified"]);d.badPackage=false;d.failRead=true;step(4500);assert(s.errors==2);d.failRead=false;
  s.powerNotify(&s,nullptr,kIOMessageSystemWillSleep,nullptr,nullptr,0);assert(s.asleep&&!s.timer->pending&&!s.nums["LatitudeTabletModeQualified"]);
  s.powerNotify(&s,nullptr,kIOMessageSystemHasPoweredOn,nullptr,nullptr,0);assert(!s.asleep&&s.timer->pending&&!s.nums["LatitudeTabletModeQualified"]);
  auto notice=s.notice,power=s.power;s.stop(&d);assert(notice->removed&&power->removed&&!s.active&&!s.timer&&!s.gate&&!s.loop);s.free();
 }
 LatitudeTablet::ModeQualification q;
 assert(!q.update(0,true,1));q.event(0xcc,100);assert(!q.update(1000,true,1)); // cold boot folded is not qualified
 assert(q.update(2000,true,0));q.event(0xcc,2100);assert(q.update(2200,true,0));assert(!q.update(2300,true,1));assert(q.update(3000,true,1)&&q.isTablet());
 assert(!q.update(3100,false,0)&&!q.isTablet());assert(!q.update(3200,true,1));
 q.reset();assert(q.update(5000,true,0));q.event(0xcc,5100);assert(!q.update(11000,true,1));assert(!q.update(1,true,1));
 puts("PASS actual live-state driver and qualifier under ASan/UBSan: valid/malformed firmware packages, corroborated folding, no-event/cold-fold refusal, unfold, read-error fail-open, clock reset, wake invalidation, start failure and teardown. Firmware reads mocked; no input/PCI writes.");
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(pre+source+post);subprocess.run(['xcrun','clang++','-std=c++17','-g','-fsanitize=address,undefined','-I'+str(root/'tablet-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
