from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/kexts/NMXVGAFB/nmxvgafb.cpp').read_text();a=s.index('    IOReturn latitudeSetGamma(');b=s.index('{',a);d=1;j=b+1
while d:d+=(s[j]=='{')-(s[j]=='}');j+=1
pre='''#include "latitude_gamma.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
using UInt32=unsigned;using IOReturn=int;constexpr int kIOReturnBadArgument=-1,kIOReturnNotReady=-2,kIOReturnSuccess=0;static void *kOSBooleanTrue=(void *)1,*kOSBooleanFalse=nullptr;
static int locks;void IOLockLock(void *){assert(!locks++);}void IOLockUnlock(void *){assert(locks--==1);}
struct Mock {void *fGammaLock=(void *)1;LatitudeGamma fGamma;template<typename... A>void setProperty(A...){}
'''
post='''};int main(){Mock m;latitudeGammaIdentity(&m.fGamma);unsigned char t[768];for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++)t[c*256+i]=(unsigned char)(c==2?i/2:i);
assert(!m.latitudeSetGamma(3,256,8,t)&&m.fGamma.active&&m.fGamma.generation==1);
assert(!m.latitudeSetGamma(3,256,8,t)&&m.fGamma.generation==1);
assert(m.latitudeSetGamma(4,256,8,t)==kIOReturnBadArgument&&m.fGamma.generation==1);
assert(!m.latitudeSetGamma(0,0,0,nullptr)&&!m.fGamma.active&&m.fGamma.generation==2);
m.fGammaLock=nullptr;assert(m.latitudeSetGamma(3,256,8,t)==kIOReturnNotReady);
puts("PASS actual framebuffer gamma setter: valid updates, identity reset, generation changes only with content, invalid input preserves table and absent lock refuses.");}
'''
f=r/'desktop-build/test-framebuffer-gamma.cpp';f.write_text(pre+s[a:j]+post);exe=r/'desktop-build/test-framebuffer-gamma';subprocess.run(['xcrun','clang++','-arch','x86_64','-std=c++17','-fsanitize=address,undefined','-I',str(r/'NullMoth/kexts/NVRM'),str(f),'-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
