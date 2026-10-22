from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/kexts/NVRM/accel/nvrm-accel.cpp').read_text()
a=s.index('static void nmxFrontArm()');b=s.index('static void nmxFrontInit()',a)
pre='''#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "latitude_front_copy.h"
using SInt64=long long;using thread_call_param_t=void *;
struct Framebuffer{bool isInactive(){return false;}};
struct LatitudeGamma{uint64_t generation;};
static uint64_t gammaGeneration;
static bool nmxGetGamma(Framebuffer *,LatitudeGamma *g){g->generation=gammaGeneration;return true;}
struct IOSurface{uint64_t pending,published;};
static struct {void *src;IOSurface *surface;uint64_t published;Framebuffer *framebuffer;uint64_t gamma;} gNmxFront;
static int gNmxFrontHz=60;static constexpr unsigned kNmxFrontMaxHz=240;static unsigned gNmxFrontTicksLeft;
static bool gNmxFrontArmed;static uint64_t gNmxFrontNext;static SInt64 gNmxFrontSkipped,gNmxLateRefresh;
static void *gNmxFrontTimer=(void *)1,*gNmxFrontLock=(void *)1;static unsigned copies,locks;static uint64_t deadline;
void IOLockLock(void *){assert(!locks++);}void IOLockUnlock(void *){assert(locks--==1);}
uint64_t mach_absolute_time(){return 100;}void nanoseconds_to_absolutetime(uint64_t n,uint64_t *p){*p=n;}
long long OSAddAtomic64(long long n,long long *p){return *p+=n;}void thread_call_enter_delayed(void *,uint64_t n){deadline=n;}
uint64_t nmxSurfaceNumber(IOSurface *s,const char *){return s->pending;}uint64_t nmxPublished(IOSurface *s){return s->published;}
void nmxFrontCopyLocked(){assert(locks);copies++;gNmxFront.gamma=gammaGeneration;}
'''
post='''int main(){IOSurface s={2,1};gNmxFront={(void *)1,&s,1};gNmxFrontTicksLeft=1;
nmxFrontTick(nullptr,nullptr);assert(!copies&&gNmxFrontArmed&&!gNmxFrontTicksLeft&&deadline==200000100);
s.published=2;nmxFrontTick(nullptr,nullptr);assert(copies==1&&gNmxLateRefresh==1&&gNmxFront.published==2);
nmxFrontTick(nullptr,nullptr);assert(copies==1);
s.pending=3;nmxFrontTick(nullptr,nullptr);assert(copies==1);s.published=3;nmxFrontTick(nullptr,nullptr);assert(copies==2);
gammaGeneration=1;nmxFrontTick(nullptr,nullptr);assert(copies==3&&gNmxFront.gamma==1);nmxFrontTick(nullptr,nullptr);assert(copies==3);
gNmxFront.src=nullptr;nmxFrontTick(nullptr,nullptr);assert(!gNmxFrontArmed);
gNmxFront={(void *)1,nullptr,0};gNmxFrontTicksLeft=2;nmxFrontTick(nullptr,nullptr);nmxFrontTick(nullptr,nullptr);assert(copies==5&&!gNmxFrontArmed);
gNmxFrontHz=0;gNmxFrontTicksLeft=1;nmxFrontArm();assert(!gNmxFrontArmed&&!gNmxFrontNext);
puts("PASS actual front timer/arm: hold incomplete frames; full refresh on late publication; unchanged idle surface not recopied; 5Hz metadata deadline; source retirement stops; legacy retry stays bounded; idle gamma change triggers exactly one refresh; disabled timer respected.");}
'''
f=r/'desktop-build/test-front-publication.cpp';f.write_text(pre+s[a:b].replace('#include \"nmx/latitude_pipe_colour.inc\"','')+post)
exe=r/'desktop-build/test-front-publication';subprocess.run(['xcrun','clang++','-std=c++17','-arch','x86_64','-fsanitize=address,undefined','-I',str(r/'NullMoth/kexts/NVRM/accel/nmx'),str(f),'-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
