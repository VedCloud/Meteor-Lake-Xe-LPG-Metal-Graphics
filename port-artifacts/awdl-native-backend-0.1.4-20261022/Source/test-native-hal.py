"""Actual native HAL bodies; replace only external kernel/firmware primitives."""
from pathlib import Path
import subprocess,re,json,hashlib
r=Path(__file__).resolve().parent.parent;out=r/'desktop-build/native-awdl-abi'
real=(r/'desktop-build/airport-awdl-connected/itlwm/hal_iwx/ItlIwx.cpp').read_text()
reg=(r/'desktop-build/airport-awdl-connected/itlwm/hal_iwx/if_iwxreg.h').read_text()
def declaration(text,name):return re.search(r'struct '+name+r' \{.*?\} __packed;',text,re.S)[0]
# Exact command structures from the compiled source, not hand-mirrored fields.
commands='\n'.join(declaration(real,n) for n in ['latitude_link_v1','latitude_sta_v1'])
commands+='\n'+'\n'.join(declaration(reg,n) for n in ['iwx_fw_channel_info','iwx_phy_context_cmd','iwx_session_prot_cmd'])
prefix=r'''
#include "LatitudeAWDLNativePacket.h"
#include "LatitudeAWDLTx.h"
#include <vector>
#include <set>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#define __packed __attribute__((packed))
#define M_NOWAIT 1
#define M_ZERO 2
#define MBUF_DONTWAIT 0
#define IWX_NUM_PHY_CTX 4
#define IFF_RUNNING 1
#define IEEE80211_S_RUN 2
#define IWX_FLAG_SHUTDOWN 1
#define IWX_FLAG_SCANNING 2
#define IWX_FLAG_BGSCAN 4
#define IWX_FLAG_TXFLUSH 8
#define IEEE80211_CHAN_PASSIVE 16
#define IEEE80211_CHAN_DFS 32
#define IWX_MAC_CONF_GROUP 3
#define IWX_DATA_PATH_GROUP 5
#define IWX_LONG_GROUP 1
#define IWX_SESSION_PROTECTION_CMD 5
#define IWX_PHY_CONTEXT_CMD 8
#define IWX_FW_CTXT_INVALID UINT32_MAX
#define IWX_MGMT_TID 15
#define IWX_PHY_BAND_24 1
#define IWX_PHY_BAND_5 0
#define IWX_PHY_VHT_CHANNEL_MODE20 0
#define EDCA_AC_BE 0
#define kIOReturnSuccess 0
#define kIOReturnIOError 5
#define kIOReturnAborted 6
#define htole32(x) (uint32_t(x))
#define IEEE80211_IS_CHAN_2GHZ(c) ((c)->ic_freq<3000)
#define IEEE80211_ADDR_COPY(a,b) memcpy(a,b,6)
struct iwx_ac_qos {uint8_t data[12];};struct iwx_he_backoff_conf {uint8_t data[8];};
static uint64_t now=1;static bool armed=true,hardware=true,inGate=true,dmaQuiesced=false;
static int failAt=0,operation=0,failTransmit=0,callbacks=0,callbackStatus=0,received=0;
static void*malloc(size_t n,int,int){return calloc(1,n);}
static int PE_parse_boot_argn(const char*,int*p,size_t){*p=armed;return 1;}
static void clock_get_uptime(uint64_t*p){*p=now;}
static void absolutetime_to_nanoseconds(uint64_t a,uint64_t*b){*b=a;}
struct OSObject {int refs=1;void retain(){++refs;}void release(){assert(refs>0);--refs;}};
struct Mbuf {std::vector<uint8_t>bytes;};using mbuf_t=Mbuf*;static std::set<mbuf_t>live;
static mbuf_t packet(size_t n){auto p=new Mbuf{std::vector<uint8_t>(n)};live.insert(p);return p;}
static void mbuf_freem(mbuf_t p){assert(live.erase(p));delete p;}
static size_t mbuf_len(mbuf_t p){return p->bytes.size();}
static size_t mbuf_pkthdr_len(mbuf_t p){return mbuf_len(p);}
static int mbuf_copydata(mbuf_t p,size_t at,size_t n,void*out){assert(at+n<=p->bytes.size());memcpy(out,p->bytes.data()+at,n);return 0;}
static int mbuf_allocpacket(int,size_t n,void*,mbuf_t*p){*p=packet(n);return 0;}
static int mbuf_copyback(mbuf_t p,size_t at,size_t n,const void*in,int){assert(at+n<=p->bytes.size());memcpy(p->bytes.data()+at,in,n);return 0;}
static int mbuf_pullup(mbuf_t*,size_t){assert(false);return 1;}
struct ieee80211_channel {uint32_t ic_freq=0,ic_flags=0;};
struct ieee80211_node {int ni_refcnt=0;ieee80211_channel*ni_chan=nullptr;struct{unsigned rs_nrates;uint8_t rs_rates[16];}ni_rates;uint8_t ni_macaddr[6];};
struct iwx_node {ieee80211_node in_ni;int in_id=0;};
struct iwx_softc {struct{int ic_state=2;struct{int if_flags=1;}ic_if;ieee80211_node*ic_bss=nullptr;ieee80211_channel ic_channels[256];}sc_ic;int ns_nstate=2,sc_flags=0;struct{unsigned ref=0;ieee80211_channel*channel=nullptr;}sc_phyctxt[4];struct{void*desc=(void*)1;unsigned queued=0,ring_count=64;}txq[32];};
static bool latitude_mld(iwx_softc*){return hardware;}
static uint32_t iwx_cmd_id(uint32_t id,uint32_t group,uint32_t){return id|(group<<8);}
template<class IC>static unsigned ieee80211_chan2ieee(IC*ic,ieee80211_channel*c){return unsigned(c-ic->ic_channels);}
static ieee80211_node*ieee80211_ref_node(ieee80211_node*p){++p->ni_refcnt;return p;}
template<class IC>static void ieee80211_release_node(IC*,ieee80211_node*p){assert(p->ni_refcnt>1);--p->ni_refcnt;}
template<class T,size_t N>constexpr size_t nitems(T(&)[N]){return N;}
struct Loop {bool inGate(){return ::inGate;}};
struct Cmd {uint32_t id;std::vector<uint8_t>bytes;};static std::vector<Cmd>sent;
'''
members=(r/'desktop-build/airport-awdl-connected/itlwm/hal_iwx/ItlIwx.hpp').read_text()
a=members.index('    using LatitudeNativeComplete=');b=members.index('\n    void latitudeAWDLStatus',a)
klass='''struct ItlIwx {
 iwx_softc com;uint64_t dmaEpoch=1;Loop loop;
 LatitudeAWDLPeer::Scheduler latitudeAWDLScheduler;
 LatitudeAWDLTxLedger latitudeAWDLTx;
'''+members[a:b]+r'''
 Loop*getMainWorkLoop(){return &loop;}
 int iwx_lookup_cmd_ver(iwx_softc*,int group,int id){if(group==3)return id==8?2:1;if(group==5)return id==0x17?3:2;return 4;}
 int iwx_lmac_id(iwx_softc*,ieee80211_channel*){return 0;}
 int iwx_send_cmd_pdu(iwx_softc*,uint32_t id,int,uint16_t n,const void*p){sent.push_back({id,std::vector<uint8_t>((uint8_t*)p,(uint8_t*)p+n)});return ++operation==failAt?EIO:0;}
 int iwx_tvqm_enable_txq(iwx_softc*,int,int,unsigned,int station){assert(station==3);return ++operation==failAt?-EIO:7;}
 struct Pending {mbuf_t packet;ieee80211_node*node;uint64_t cookie;};std::vector<Pending>pending;
 int iwx_tx(iwx_softc*,mbuf_t p,ieee80211_node*node,int,uint64_t cookie,int queue){
  assert(queue==7);if(failTransmit){mbuf_freem(p);return EIO;}pending.push_back({p,node,cookie});return 0;
 }
 void reclaim(bool reset){for(auto&p:pending){mbuf_freem(p.packet);ieee80211_release_node(&com.sc_ic,p.node);latitudeNativeFinished(p.cookie,reset);}pending.clear();}
};
'''
body=(r/'wireless-sources/LatitudeAWDLNativeHAL.inc').read_text()
tests=r'''
static ItlIwx*reentrant=nullptr;
static void complete(OSObject*owner,mbuf_t p,int status){
 assert(owner->refs>=2);++callbacks;callbackStatus=status;
 if(status==kIOReturnAborted)assert(dmaQuiesced);
 if(reentrant){auto d=reentrant;reentrant=nullptr;auto probe=packet(70);assert(d->latitudeNativeSubmit(probe)==ENXIO);mbuf_freem(probe);d->latitudeNativeResetAfterDMAQuiesced();}
 mbuf_freem(p);
}
static void input(OSObject*,const uint8_t*p,size_t n){assert(n==54&&p[12]==0x86&&p[13]==0xdd);++received;}
static uint8_t local[]={2,0,0,0,0,1},peer[]={2,0,0,0,0,2};
static void setup(ItlIwx&d,iwx_node&bss){sent.clear();operation=0;d.com.sc_ic.ic_bss=&bss.in_ni;d.com.sc_ic.ic_channels[44]={5220,1};d.com.sc_phyctxt[1].channel=&d.com.sc_ic.ic_channels[1];}
static mbuf_t prepared(){auto p=packet(70);auto&b=p->bytes;memcpy(b.data(),peer,6);memcpy(b.data()+6,local,6);b[13]=56;uint8_t llc[]={0xaa,0xaa,3,0,0x17,0xf2,8,0};memcpy(b.data()+14,llc,8);b[22]=3;b[23]=4;b[28]=0x86;b[29]=0xdd;b[30]=0x60;return p;}
int main(){
 for(int fail=1;fail<=11;++fail){ItlIwx d;iwx_node bss{};OSObject owner;setup(d,bss);failAt=fail;
  assert(d.latitudeNativeBind(&owner,complete,input,local,peer,5220)==EIO);assert(owner.refs==1&&d.latitudeNativePoisoned);
  auto count=operation;assert(d.latitudeNativeBind(&owner,complete,input,local,peer,5220)==EBUSY&&operation==count);
  dmaQuiesced=true;d.latitudeNativeResetAfterDMAQuiesced();assert(!d.latitudeNativePoisoned);dmaQuiesced=false;
 }
 failAt=0;ItlIwx d;iwx_node bss{};OSObject owner;setup(d,bss);
 armed=false;assert(d.latitudeNativeBind(&owner,complete,input,local,peer,5220)==EPERM&&sent.empty());armed=true;
 hardware=false;assert(d.latitudeNativeBind(&owner,complete,input,local,peer,5220)==ENOTSUP&&sent.empty());hardware=true;
 d.com.sc_phyctxt[1].ref=1;assert(d.latitudeNativeBind(&owner,complete,input,local,peer,5220)==EBUSY);d.com.sc_phyctxt[1].ref=0;
 assert(!d.latitudeNativeBind(&owner,complete,input,local,peer,5220)&&owner.refs==2);
 assert(operation==11&&sent.size()==10&&d.latitudeNativeQueue==7&&d.latitudeNativeMulticastQueue==7);
 for(auto&c:sent){uint32_t id=0;memcpy(&id,c.bytes.data(),4);assert(id!=0);}
 assert(sent[0].id==iwx_cmd_id(8,3,0)&&sent[0].bytes[8]==6);
 assert(sent[5].id==iwx_cmd_id(8,3,0)&&sent[5].bytes[0]==2&&sent[5].bytes[8]==2);
 assert(sent[9].id==iwx_cmd_id(0xa,3,0)&&sent[9].bytes[0]==3&&sent[9].bytes[4]==2&&sent[9].bytes[24]==1);
 auto p=prepared();assert(d.latitudeNativeSubmit(p)==EAGAIN&&live.count(p)&&!d.latitudeNativeOriginals.count);
 assert(!d.latitudeNativeWindow(5220,16));assert(!d.pending.size());
 assert(d.latitudeAWDLScheduler.notification(1,2,1,1,now));
 failTransmit=1;assert(d.latitudeNativeSubmit(p)==EIO&&live.count(p)&&!d.latitudeNativeOriginals.count&&!d.latitudeAWDLScheduler.outstanding);failTransmit=0;
 assert(!d.latitudeNativeSubmit(p)&&d.latitudeNativeOriginals.count==1&&callbacks==0);
 auto cookie=d.pending[0].cookie;assert(d.latitudeNativeOriginals.response(cookie,0)&&callbacks==0);
 d.reclaim(false);assert(callbacks==1&&callbackStatus==0&&d.latitudeNativeOriginals.count==0);
 d.latitudeNativeFinished(cookie,false);assert(callbacks==1);
 p=prepared();assert(!d.latitudeNativeSubmit(p));d.latitudeNativeOriginals.response(d.pending[0].cookie,5);d.reclaim(false);assert(callbacks==2&&callbackStatus==5);
 // A validated peer frame reaches the Ethernet RX callback exactly once.
 LatitudeAWDLPeer::DataPath peerData;peerData.configure(peer);peerData.observe(local,now);uint8_t e[54]={};memcpy(e,local,6);memcpy(e+6,peer,6);e[12]=0x86;e[13]=0xdd;e[14]=0x60;
 auto radio=packet(80);assert(peerData.encode(e,54,radio->bytes.data(),80,now)==80);assert(d.latitudeNativeInput(radio,44)&&received==1);assert(!d.latitudeNativeInput(radio,44));mbuf_freem(radio);
 // Reset completion after DMA quiescence closes admission before callbacks.
 p=prepared();assert(!d.latitudeNativeSubmit(p));dmaQuiesced=true;d.reclaim(true);assert(callbacks==3&&callbackStatus==6);
 p=prepared();assert(!d.latitudeNativeSubmit(p)); // before reset entry, still same epoch
 for(auto&x:d.pending){mbuf_freem(x.packet);ieee80211_release_node(&d.com.sc_ic,x.node);}d.pending.clear();
 reentrant=&d;d.latitudeNativeResetAfterDMAQuiesced();assert(owner.refs==1&&!d.latitudeNativeOwner&&callbacks==4&&live.empty());
 puts("PASS actual native HAL: 11 firmware/queue failure stages, primary-context isolation, exact listener/P2P layouts, grant-before-TX, ownership on submission failure, response/reclaim/reset separation, RX delivery/replay, reentrant reset closure.");
}
'''
source=out/'native-hal.cpp';source.write_text(prefix+commands+'\n'+klass+body+tests)
binary=out/'test-native-hal'
subprocess.run(['xcrun','clang++','-std=c++14','-O1','-g','-Wall','-Wextra','-Werror','-Wno-unused-parameter','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(r/'wireless-sources'),str(source),'-o',str(binary)],check=True)
subprocess.run([str(binary)],check=True)
(out/'NATIVE-HAL-VALIDATION.json').write_text(json.dumps({'actualHALBodies':'strict ASan/UBSan passed','externalKernelAndFirmwarePrimitives':'substituted; no hardware I/O','AirDropFunctional':False,'sourceSHA256':hashlib.sha256(body.encode()).hexdigest()},indent=2)+'\n')
