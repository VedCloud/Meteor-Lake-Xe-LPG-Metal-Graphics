from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/kexts/NVRM/accel/nmx/latitude_pipe_colour.inc').read_text()
s=s.replace('../../latitude_colour_math.h','latitude_colour_math.h')
def replace_body(s,needle,replacement):
 a=s.index(needle);b=s.index('{',a);j=b+1;d=1
 while d:d+=(s[j]=='{')-(s[j]=='}');j+=1
 return s[:b]+replacement+s[j:]
s=replace_body(s,'static bool latitudeColourEntryABI()', '{return true;}')
s=replace_body(s,'static void latitudeColourEntry(', '{table->getTableEntry(index,(float *)&rgb[0],(float *)&rgb[1],(float *)&rgb[2]);}')
a=s.index('class IOAccelDisplayPipeGammaTable2 {');b=s.index('};',a)+2;s=s[:a]+s[b:]
a=s.index('struct LatitudeColourMatrix {');b=s.index('};',a)+2;s=s[:a]+s[b:]
pre=r'''#include <assert.h>
#include <initializer_list>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "latitude_gamma.h"
struct OSObject{int refs=1;void retain(){refs++;}void release(){assert(refs>0);refs--;}};
struct IOAccelDisplayPipeGammaTable2:OSObject{unsigned count=256;uint32_t v[3][4096];unsigned getTableCount()const{return count;}void getTableEntry(unsigned i,float *r,float *g,float *b)const{assert(i<count&&i<4096);memcpy(r,&v[0][i],4);memcpy(g,&v[1][i],4);memcpy(b,&v[2][i],4);}};
struct LatitudeColourMatrix{uint32_t bits[3][4];};
struct IOAccelDisplayPipeTransaction2{unsigned options=0,planeOptions=0;IOAccelDisplayPipeGammaTable2 *pre=nullptr,*post=nullptr,*plane=nullptr;LatitudeColourMatrix m={};unsigned getTransactionOptions(){return options;}unsigned getPlaneTransactionOptions(unsigned){return planeOptions;}auto getPipePreGammaTable(){return pre;}auto getPipePostGammaTable(){return post;}auto getPlaneGammaTable(unsigned){return plane;}auto getPipeColorMatrix(){return m;}};
struct OSSymbol{static const OSSymbol *withCStringNoCopy(const char *){static OSSymbol v;return &v;}void release()const{}};
using IOReturn=int;const int kIOReturnSuccess=0,kIOReturnNoMemory=-1;static void *kOSBooleanTrue=(void *)1,*kOSBooleanFalse=nullptr;
struct IOService{LatitudeGamma gamma;unsigned calls=0;bool fail=false,physical=true;void *getProperty(const char *){return physical?(void *)1:nullptr;}template<class...A>void setProperty(A...){}int callPlatformFunction(const OSSymbol *,bool,void *p,void *,void *,void *){if(fail)return -1;gamma=*(LatitudeGamma *)p;calls++;return 0;}};
static uint32_t bits(float v){uint32_t b;memcpy(&b,&v,4);return b;}
'''
post=r'''int main(){IOAccelDisplayPipeGammaTable2 sampled;uint16_t table[3][256];for(unsigned count:{2u,3u,17u,256u,1024u,4096u}){sampled.count=count;for(unsigned c=0;c<3;c++)for(unsigned i=0;i<count;i++)sampled.v[c][i]=bits((float)i/(count-1));assert(latitudeColourTable(&sampled,table));for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++){int delta=(int)table[c][i]-(int)(i*257);assert(delta>=-2&&delta<=2);}}sampled.count=1;assert(!latitudeColourTable(&sampled,table));sampled.count=4097;assert(!latitudeColourTable(&sampled,table));
 IOService fb;IOAccelDisplayPipeTransaction2 t;void *owner=(void *)123;
assert(latitudePipeColour(&fb,&t,owner)&&fb.calls==1&&!fb.gamma.active);
assert(latitudePipeColour(&fb,&t,owner)&&fb.calls==1);
t.m.bits[0][0]=bits(1);t.m.bits[1][1]=bits(.75f);t.m.bits[2][2]=bits(.5f);
assert(latitudePipeColour(&fb,&t,owner)&&fb.gamma.active&&fb.gamma.rgb[2][254]==127&&fb.gamma.rgb[0][254]==254);
IOAccelDisplayPipeGammaTable2 post;for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++)post.v[c][i]=bits((float)i/255);
t.post=&post;assert(latitudePipeColour(&fb,&t,owner)&&post.refs==2);
unsigned calls=fb.calls;assert(latitudePipeColour(&fb,&t,owner)&&fb.calls==calls&&post.refs==2);
post.v[0][30]=0x7fc00000;gLatitudeColourCache.valid=false;assert(!latitudePipeColour(&fb,&t,owner)&&fb.calls==calls);post.v[0][30]=bits(30.f/255);
t.options=0xc4;assert(latitudePipeColour(&fb,&t,owner)&&!fb.gamma.active&&post.refs==1);
t.options=0;t.m.bits[0][1]=bits(.1f);assert(!latitudePipeColour(&fb,&t,owner));t.m.bits[0][1]=0;
fb.fail=true;assert(!latitudePipeColour(&fb,&t,owner));fb.fail=false;assert(latitudePipeColour(&fb,&t,owner));latitudeColourCacheClear();assert(post.refs==1);
fb.physical=false;calls=fb.calls;assert(latitudePipeColour(&fb,&t,owner)&&fb.calls==calls);
puts("PASS actual colour bridge: identity, warm gains, table/cache ownership, unchanged-frame skip, NaN/bad CSC refusal without publishing, bypass restores identity, framebuffer rejection, physical-panel guard and cleanup.");}
'''
f=r/'desktop-build/test-pipe-colour.cpp';f.write_text(pre+s+post);exe=f.with_suffix('')
subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-I',str(r/'NullMoth/kexts/NVRM'),str(f),'-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
# Exact recovery-family getter fingerprint supports legacy SSE state preservation.
code=bytes.fromhex('554889e5488b471089f648c1e604f30f10443010f30f1102f30f10443014f30f1101f30f10443018f3410f11005dc3')
assert code in (r/'research/IOAcceleratorFamily2-recovery-reference.macho').read_bytes()
print('PASS reference getter fingerprint; target runtime refuses differing getter code before calling it. Privileged FP guard is compiled, not executed on host.')
