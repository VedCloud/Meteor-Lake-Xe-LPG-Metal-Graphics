from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent
s=(r/'latitude-brightness.m').read_text();body=s[s.index('static int step(int dir)'):s.index('static CGEventRef onEvent')]
head=r'''
#include <cassert>
#include <cstdio>
#include <cmath>
#include "latitude_brightness_policy.h"
using io_service_t=unsigned;using CGDirectDisplayID=unsigned;
constexpr int kIOReturnBadArgument=-3;
int panelOK=1,idOK=1,getResult=0,setResult=0,getCalls=0,sets=0,notices=0,huds=0;float current=.5f,written=-1;
void loadServices(){}io_service_t panel(){return panelOK;}unsigned panelID(unsigned){return idOK;}void IOObjectRelease(unsigned){}
int get(unsigned,float*p){getCalls++;*p=current;return getResult;}int set(unsigned,float v){sets++;written=v;return setResult;}bool can(unsigned){return true;}void changed(unsigned,double){notices++;}void showBrightness(unsigned,float){huds++;}
int(*getBrightness)(unsigned,float*)=get;int(*setBrightness)(unsigned,float)=set;bool(*canBrightness)(unsigned)=can;void(*brightnessChanged)(unsigned,double)=changed;
'''
main=r'''
int main(){assert(!step(1));assert(sets==1&&written==.5625f&&huds==1&&notices==1);
getResult=-1;assert(step(-1));assert(sets==1&&huds==1);getResult=0;current=NAN;assert(step(-1));assert(sets==1);current=.5;setResult=-1;assert(step(1));assert(huds==1);setResult=0;
getBrightness=nullptr;assert(step(1));assert(sets==2);getBrightness=get;idOK=0;assert(step(1));assert(sets==2);idOK=1;panelOK=0;assert(step(1));assert(sets==2);
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(head+body+main)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-I'+str(r),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
print('PASS actual brightness step: system get/set, HUD on success only, invalid state and API/display failure handling. Native framework/physical result requires Dell test.')
