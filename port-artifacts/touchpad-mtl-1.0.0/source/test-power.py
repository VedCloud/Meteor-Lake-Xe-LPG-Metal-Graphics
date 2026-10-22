from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
s=r'''
#include <cassert>
#include <cstdio>
#include "latitude_i2c_power.h"
struct PCI{
 uint16_t vendor=0x8086,device=0x7e7b,subvendor=0x1028,subsystem=0x0cb6,pm=0;
 uint8_t cap=0x80;bool present=true,stuck=false;unsigned writes=0,enables=0,lastOffset=0;uint16_t lastValue=0;
 uint16_t configRead16(unsigned offset){switch(offset){case 0:return vendor;case 2:return device;case 0x2c:return subvendor;case 0x2e:return subsystem;default:assert(offset==cap+4);return pm;}}
 unsigned findPCICapability(unsigned id,uint8_t*out){assert(id==1);*out=cap;return present?1:0;}
 void enablePCIPowerManagement(unsigned state){assert(state==0);}
 void configWrite16(unsigned offset,uint16_t value){writes++;lastOffset=offset;lastValue=value;if(!stuck)pm=value;}
 void setBusMasterEnable(bool b){assert(b);enables++;}void setMemoryEnable(bool b){assert(b);enables++;}
};
int main(){
 unsigned cases=0;
 for(unsigned old=0;old<65535;old++){
  PCI pci;pci.pm=old;unsigned sleeps=0;bool ok=latitude_i2c_d0(&pci,[&]{sleeps++;});assert(ok && pci.enables==2 && !sleeps);
  if(old&3){assert(pci.writes==1&&pci.lastOffset==0x84&&pci.lastValue==(old&~0x8003u));}
  else assert(!pci.writes);
  cases++;
 }
 for(unsigned c=0;c<256;c++){
  PCI p;p.cap=c;p.pm=3;bool ok=latitude_i2c_d0(&p,[]{});bool valid=c>=0x40&&c<=0xf8&&!(c&3);assert(ok==valid);if(!valid)assert(!p.writes&&!p.enables);
 }
 for(unsigned field=0;field<4;field++){
  PCI p;if(field==0)p.vendor=0;if(field==1)p.device=0x7f4d;if(field==2)p.subvendor=0;if(field==3)p.subsystem=0;assert(!latitude_i2c_d0(&p,[]{}));assert(!p.writes&&!p.enables);
 }
 PCI p;p.pm=0xffff;assert(!latitude_i2c_d0(&p,[]{}));assert(!p.writes);
 p.pm=3;p.stuck=true;unsigned waits=0;assert(!latitude_i2c_d0(&p,[&]{waits++;}));assert(waits==10&&!p.enables);
 p.present=false;p.writes=0;assert(!latitude_i2c_d0(&p,[]{}));assert(!p.writes);
 printf("PASS actual Dell I2C power helper ASan/UBSan: %u PMCSR words, discovered capability bounds, D0 verify, PME-status preservation, exact PCI/subsystem scope, stuck/missing hardware refusal\n",cases);
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(s);subprocess.run(['xcrun','clang++','-std=c++11','-fsanitize=address,undefined','-g','-I',str(r/'touchpad-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
