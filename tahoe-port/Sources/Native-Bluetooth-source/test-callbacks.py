from pathlib import Path
import subprocess,json
b=Path(__file__).resolve().parent;s=(b/'IntelBTPatcher.cpp').read_text()
helpers=s[s.index('static IOSimpleLock* nativeLock'):s.index('static CIntelBTPatcher ibtPatcher')]
methods=s[s.index('static void rewriteHciEvent('):s.index('#define VENDOR_USB_INTEL')]
a=s.index('bool CIntelBTPatcher::isIntelSource(');z=s.index('\nint CIntelBTPatcher::',a);filterfn=s[a:z]
prefix=r'''
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "NativeLEHandshake.hpp"
using UInt64=unsigned long long;using IOReturn=int;using mach_vm_address_t=uintptr_t;
const int kIOReturnSuccess=0,kIOReturnNoMemory=-1,kIODirectionInOut=3;
struct IOSimpleLock{};int IOSimpleLockLockDisableInterrupt(IOSimpleLock*){return 0;}void IOSimpleLockUnlockEnableInterrupt(IOSimpleLock*,int){}
void clock_get_uptime(UInt64* t){*t=100;}void absolutetime_to_nanoseconds(UInt64 t,UInt64* n){*n=t;}
#define SYSLOG(...) do{}while(0)
#define FunctionCast(f,p) reinterpret_cast<decltype(&f)>(p)
struct IOMemoryDescriptor{
 unsigned char data[64]={};unsigned length=64;int refs=1,prepares=0,completes=0;bool failPrepare=false;
 unsigned getLength(){return length;}int prepare(int){++prepares;return failPrepare?-2:0;}void complete(int){++completes;}
 unsigned readBytes(unsigned,void* p,unsigned n){memcpy(p,data,n);return n;}
 unsigned writeBytes(unsigned,const void* p,unsigned n){assert(n<=length);memcpy(data,p,n);return n;}
 void retain(){++refs;}void release(){assert(refs>1);--refs;}
};
using IOUSBHostCompletionAction=void(*)(void*,void*,IOReturn,uint32_t);
using IOUSBHostBundledCompletionAction=void(*)(void*,uint32_t,IOMemoryDescriptor**,void**,IOReturn*,uint32_t*);
struct IOUSBHostCompletion{void* owner;IOUSBHostCompletionAction action;void* parameter;};
struct IOUSBHostBundledCompletion{void* owner;IOUSBHostBundledCompletionAction action;void* parameter;};
struct AsyncOwnerData{void* owner;IOMemoryDescriptor* dataBuffer;IOUSBHostCompletionAction action;IOUSBHostBundledCompletionAction bundledAction;};
'''
clazz=r'''
class CIntelBTPatcher{public:
 static void* _hookPipeInstance;static bool _iosSourceCompletionRouted;static bool isIntelSource(void*);
 mach_vm_address_t oldAsyncIO=0,oldIOSourceIOCompletion=0,oldIOSourceIORjj=0,oldIOSourceIOBundled=0;
 static IOReturn newAsyncIO(void*,IOMemoryDescriptor*,uint32_t,IOUSBHostCompletion*,uint32_t);
 static IOReturn newIOSourceIOCompletion(void*,IOMemoryDescriptor*,uint32_t,IOUSBHostCompletion*,uint32_t);
 static IOReturn newIOSourceIORjj(void*,IOMemoryDescriptor*,uint32_t,uint32_t&,uint32_t);
 static IOReturn newIOSourceIOBundled(void*,IOMemoryDescriptor*,uint32_t,IOUSBHostBundledCompletion*,uint32_t);
};
void* CIntelBTPatcher::_hookPipeInstance=(void*)1;bool CIntelBTPatcher::_iosSourceCompletionRouted=true;
static CIntelBTPatcher instance;static CIntelBTPatcher* callbackIBTPatcher=&instance;
'''
main=r'''
static IOUSBHostCompletion saved;static IOUSBHostBundledCompletion bundledSaved;
static int oldCalls=0,userCalls=0,errorRet=0;static bool nest=false;
static void originalDone(void* owner,void* parameter,int status,unsigned n){assert(owner==(void*)11);assert(parameter==(void*)12);assert(status==0);++userCalls;}
static void originalBundled(void* owner,unsigned n,IOMemoryDescriptor**,void**,int*,unsigned*){assert(owner==(void*)11&&n==1);++userCalls;}
static int originalSync(void*,IOMemoryDescriptor*,unsigned,unsigned& n,unsigned){n=14;return errorRet;}
static int originalAsync(void* source,IOMemoryDescriptor* p,unsigned n,IOUSBHostCompletion* c,unsigned t){++oldCalls;if(c)saved=*c;if(nest){unsigned actual=0;CIntelBTPatcher::newIOSourceIORjj(source,p,n,actual,t);}return errorRet;}
static int originalBundle(void*,IOMemoryDescriptor*,unsigned,IOUSBHostBundledCompletion* c,unsigned){++oldCalls;if(c)bundledSaved=*c;return errorRet;}
static void remote(IOMemoryDescriptor& p){memset(p.data,0,64);p.data[0]=0x3e;p.data[1]=12;p.data[2]=4;p.data[4]=1;}
int main(){IOSimpleLock lock;nativeLock=&lock;
 instance.oldAsyncIO=instance.oldIOSourceIOCompletion=(uintptr_t)&originalAsync;
 instance.oldIOSourceIOBundled=(uintptr_t)&originalBundle;instance.oldIOSourceIORjj=(uintptr_t)&originalSync;
 IOMemoryDescriptor p;remote(p);IOUSBHostCompletion c={(void*)11,originalDone,(void*)12};
 assert(nativeBegin(1));
 // Foreign USB sources must remain untouched, including completion ownership.
 CIntelBTPatcher::newIOSourceIOCompletion((void*)2,&p,64,&c,0);assert(saved.action==originalDone&&p.refs==1);
 // A nested transfer must not consume the same remote-features event twice.
 nest=true;CIntelBTPatcher::newIOSourceIOCompletion((void*)1,&p,64,&c,0);nest=false;
 assert(p.refs==2&&c.action==originalDone);saved.action(saved.owner,saved.parameter,0,14);
 assert(p.data[2]==4&&p.refs==1&&userCalls==1);
 // A successful deferred completion can now produce PHY using retained storage.
 remote(p);CIntelBTPatcher::newIOSourceIOCompletion((void*)1,&p,64,&c,0);
 saved.action(saved.owner,saved.parameter,0,14);assert(p.data[2]==12&&p.refs==1);
 remote(p);IOUSBHostBundledCompletion bc={(void*)11,originalBundled,nullptr};
 CIntelBTPatcher::newIOSourceIOBundled((void*)1,&p,64,&bc,0);
 IOMemoryDescriptor* array[]={&p};void* params[]={nullptr};int status[]={0};unsigned count[]={14};
 bundledSaved.action(bundledSaved.owner,1,array,params,status,count);
 assert(count[0]==13&&p.data[2]==7&&p.data[3]==1&&p.refs==1);
 // Immediate submission errors release the wrapper and descriptor exactly once.
 errorRet=-7;int result=CIntelBTPatcher::newIOSourceIOCompletion((void*)1,&p,64,&c,0);assert(result==-7&&p.refs==1);
 assert(nativeClaim(&p));nativeRelease(&p);errorRet=0;
 // Failed prepare / malformed length are untouched; successful prepare is paired.
 p.failPrepare=true;unsigned n=14;rewriteHciEvent(&p,n);assert(n==14);p.failPrepare=false;
 p.length=4;rewriteHciEvent(&p,n);assert(n==14);p.length=64;
 assert(p.prepares==p.completes+1);
 puts("PASS actual native USB callbacks: source scope, nested wrappers, deferred lifetimes, bundled length, immediate failure cleanup, bounded buffers");
}
'''
f=b/'test-callbacks.cpp';f.write_text(prefix+helpers+clazz+methods+filterfn+main)
subprocess.run(['xcrun','clang++','-std=c++14','-fsanitize=address,undefined',str(f),'-o',str(b/'test-callbacks')],check=True)
p=subprocess.run([str(b/'test-callbacks')],text=True,capture_output=True,check=True);print(p.stdout,end='');f=b/'TESTS.json';d=json.loads(f.read_text());d['actualUSBCallbackReplay']=p.stdout.strip();f.write_text(json.dumps(d,indent=2)+'\n')
