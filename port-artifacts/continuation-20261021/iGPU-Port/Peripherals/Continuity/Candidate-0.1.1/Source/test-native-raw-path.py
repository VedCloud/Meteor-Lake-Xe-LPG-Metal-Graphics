"""Exercise the actual new callback/HAL bodies with mocked kernel/radio APIs."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;w=r/'desktop-build/airport-awdl';out=r/'desktop-build/airport-awdl-objects'
upper=(w/'AirportItlwm/AirportItlwmV2.cpp').read_text();upper=upper[upper.index('int AirportItlwm::bpfOutputPacket('):]
lower=(w/'itlwm/hal_iwx/ItlIwx.cpp').read_text();lower=lower[lower.index('int ItlIwx::latitudeAWDLCurrentChannel('):]
harness=r'''#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <set>
#include <cstdio>
#include "LatitudeAWDLFrame.h"
using UInt32=uint32_t;using IOReturn=int;
struct OSObject {virtual ~OSObject()=default;};
#define OSDynamicCast(T,x) dynamic_cast<T*>(x)
constexpr int kIOPCIConfigDeviceID=2,IFF_RUNNING=1,IFF_UP=2,IEEE80211_S_RUN=3,IEEE80211_M_STA=4;
constexpr int IWX_FLAG_SHUTDOWN=1,IWX_FLAG_TXFLUSH=2,IWX_FLAG_SCANNING=4,IWX_FLAG_BGSCAN=8,IEEE80211_F_TX_MGMT_ONLY=16;
constexpr int IEEE80211_CHAN_PASSIVE=1,IEEE80211_CHAN_DFS=2,EDCA_AC_BE=0,MBUF_DONTWAIT=0;
struct Channel {unsigned ic_flags=0;uint16_t ic_freq=2412;};
#define IEEE80211_CHAN_ANYC ((Channel*)-1)
struct Node {Channel*ni_chan;unsigned refs=1;};
struct Pci {int device=0x7e40;int configRead16(int){return device;}};
struct Ring {int ring_count=64,queued=0;void*desc=(void*)1;};
struct Mask {bool full=false;bool empty(){return !full;}};
struct Ifnet {int if_flags=3;};
struct IC {Ifnet ic_if;int ic_state=3,ic_opmode=4,ic_xflags=0;Node*ic_bss;uint8_t ic_myaddr[6]={2,1,2,3,4,5};};
struct Softc {Pci*sc_pcitag;const char*sc_fwname="iwlwifi-ma-b0-gf-a0-83.ucode";IC sc_ic;int ns_nstate=3,sc_flags=0,first_data_qid=0;Ring txq[4];Mask qfullmsk;};
template<class T,size_t N>constexpr size_t nitems(const T(&)[N]){return N;}
struct Mbuf {std::vector<uint8_t>data;};using mbuf_t=Mbuf*;
static std::set<mbuf_t>live;static int allocations=0,opt=0,baseCalls=0,txCalls=0,txError=0;static bool failMalloc=false,failCopy=false,failAlloc=false;static std::vector<uint8_t>sent;
static void mbuf_freem(mbuf_t m){assert(live.erase(m)==1);delete m;}
static size_t mbuf_pkthdr_len(mbuf_t m){return m->data.size();}
static size_t mbuf_len(mbuf_t m){return m->data.size();}
static int mbuf_copydata(mbuf_t m,size_t offset,size_t n,void*p){if(failCopy)return EIO;assert(offset+n<=m->data.size());memcpy(p,m->data.data()+offset,n);return 0;}
static int mbuf_allocpacket(int,size_t n,void*,mbuf_t*m){if(failAlloc)return ENOMEM;*m=new Mbuf{std::vector<uint8_t>(n)};live.insert(*m);return 0;}
static int mbuf_copyback(mbuf_t m,size_t offset,size_t n,const void*p,int){assert(offset+n<=m->data.size());memcpy(m->data.data()+offset,p,n);return 0;}
static int mbuf_pullup(mbuf_t*,size_t){assert(false);return ENOMEM;}
static void*IOMalloc(size_t n){if(failMalloc)return nullptr;allocations++;return malloc(n);}
static void IOFree(void*p,size_t){assert(p);allocations--;free(p);}
static bool PE_parse_boot_argn(const char*,int*p,size_t){*p=opt;return true;}
static Node*ieee80211_ref_node(Node*n){n->refs++;return n;}
static void ieee80211_release_node(IC*,Node*n){assert(n->refs>1);n->refs--;}
class ItlIwx:public OSObject {public:Softc com;int latitudeAWDLCurrentChannel(const void*,size_t);int iwx_tx(Softc*,mbuf_t m,Node*,int){txCalls++;sent=m->data;mbuf_freem(m);return txError;}};
class IO80211Controller:public OSObject {public:int bpfOutputPacket(OSObject*,UInt32,mbuf_t m){baseCalls++;mbuf_freem(m);return ENOTSUP;}};
class AirportItlwm:public IO80211Controller {public:OSObject*fHalService=nullptr;int bpfOutputPacket(OSObject*,UInt32,mbuf_t);static IOReturn latitudeAWDLRawGated(OSObject*,void*,void*,void*,void*);template<class...T>void setProperty(T...){}};
struct Gate {OSObject*owner=nullptr;IOReturn runAction(IOReturn(*f)(OSObject*,void*,void*,void*,void*),void*a,void*b){return f(owner,a,b,nullptr,nullptr);}};
static Gate*_fCommandGate=nullptr;
#define super IO80211Controller
'''
tests=r'''
static mbuf_t packet(){
 auto m=new Mbuf{std::vector<uint8_t>(55,0)};live.insert(m);auto p=m->data.data();p[2]=12;p[4]=8;p[8]=0x6c;p[9]=9;p+=12;p[0]=0xd0;
 uint8_t source[]={2,1,2,3,4,5},bssid[]={0,0x25,0,0xff,0x94,0x73};for(unsigned i=0;i<6;i++){p[4+i]=0xff;p[10+i]=source[i];p[16+i]=bssid[i];}p[24]=127;p[26]=0x17;p[27]=0xf2;p[28]=8;p[29]=0x10;p[40]=21;return m;
}
int main(){
 Pci pci;Channel channel;Node node{&channel};ItlIwx hal;hal.com.sc_pcitag=&pci;hal.com.sc_ic.ic_bss=&node;AirportItlwm driver;driver.fHalService=&hal;Gate gate{&driver};_fCommandGate=&gate;
 assert(driver.bpfOutputPacket(nullptr,127,packet())==ENOTSUP&&baseCalls==1&&txCalls==0);opt=1;
 assert(!driver.bpfOutputPacket(nullptr,127,packet())&&txCalls==1&&sent.size()==43&&sent[0]==0xd0&&node.refs==2);ieee80211_release_node(&hal.com.sc_ic,&node);
 txError=EIO;assert(driver.bpfOutputPacket(nullptr,127,packet())==EIO&&node.refs==1);txError=0;
 int count=txCalls;channel.ic_freq=2437;assert(driver.bpfOutputPacket(nullptr,127,packet())==EINVAL);channel.ic_freq=2412;
 channel.ic_flags=IEEE80211_CHAN_DFS;assert(driver.bpfOutputPacket(nullptr,127,packet())==EBUSY);channel.ic_flags=0;
 hal.com.sc_flags=IWX_FLAG_SCANNING;assert(driver.bpfOutputPacket(nullptr,127,packet())==EBUSY);hal.com.sc_flags=0;
 hal.com.qfullmsk.full=true;assert(driver.bpfOutputPacket(nullptr,127,packet())==ENOBUFS);hal.com.qfullmsk.full=false;
 failMalloc=true;assert(driver.bpfOutputPacket(nullptr,127,packet())==ENOMEM);failMalloc=false;
 failCopy=true;assert(driver.bpfOutputPacket(nullptr,127,packet())==EIO);failCopy=false;
 failAlloc=true;assert(driver.bpfOutputPacket(nullptr,127,packet())==ENOMEM);failAlloc=false;
 _fCommandGate=nullptr;assert(driver.bpfOutputPacket(nullptr,127,packet())==ENXIO);_fCommandGate=&gate;
 driver.fHalService=nullptr;assert(driver.bpfOutputPacket(nullptr,127,packet())==ENOTSUP);driver.fHalService=&hal;
 assert(txCalls==count&&node.refs==1&&allocations==0&&live.empty());
 puts("PASS actual native callback -> gated IWX submit bodies: packet/node ownership, submission-error cleanup, channel/DFS/scan/full-queue refusal, allocation/copy/missing-gate failures and default superclass behavior. Kernel and radio mocked; no RF proof.");
}
'''
source=out/'test-native-raw.cpp';source.write_text(harness+upper+lower+tests);binary=out/'test-native-raw'
subprocess.run(['xcrun','clang++','-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(r/'wireless-sources'),str(source),'-o',str(binary)],check=True)
subprocess.run([str(binary)],check=True)
