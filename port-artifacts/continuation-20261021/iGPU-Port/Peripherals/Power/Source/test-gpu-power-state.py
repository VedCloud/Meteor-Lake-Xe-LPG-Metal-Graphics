"""Exercise the actual kernel callback, including failed suspend/resume retry."""
from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
s=(r/'NullMoth/gpu-port/xkpi/xnu/nmx_kext.cpp').read_text()
a=s.index('IOReturn NMXGPU::setPowerState(');b=s.index('\nvoid NMXGPU::systemWillShutdown',a)
body=s[a:b]
with tempfile.TemporaryDirectory() as t:
 p=Path(t);src=p/'test.cpp'
 src.write_text('''#include <cassert>
#include <cstdint>
using IOReturn=int;struct IOService{};constexpr int kIOPMAckImplied=0;
static int result=0,calls=0;static bool requested=false;
static int nmx_port_power(void*,bool on){++calls;requested=on;return result;}
static void IOLog(const char*,...){}
struct NMXGPU {void*dev_=(void*)1;bool suspended_=false;
template<class...T>void setProperty(T...){};IOReturn setPowerState(unsigned long,IOService*);};
'''+body+'''
int main(){NMXGPU g;g.setPowerState(1,nullptr);assert(calls==0);
 result=5;g.setPowerState(0,nullptr);assert(calls==1&&!g.suspended_&&!requested);
 result=0;g.setPowerState(0,nullptr);assert(calls==2&&g.suspended_);
 g.setPowerState(0,nullptr);assert(calls==2);
 result=5;g.setPowerState(1,nullptr);assert(calls==3&&g.suspended_&&requested);
 result=0;g.setPowerState(1,nullptr);assert(calls==4&&!g.suspended_);
 g.setPowerState(1,nullptr);assert(calls==4);
 g.dev_=nullptr;g.setPowerState(0,nullptr);assert(calls==4);}
''')
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined',str(src),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS actual GPU power callback: initial/repeated power, failed suspend/resume retry, absent device. No hardware claim.')
