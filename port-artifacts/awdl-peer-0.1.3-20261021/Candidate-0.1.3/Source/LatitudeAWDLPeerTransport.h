#pragma once
#include <stdint.h>
#include <stddef.h>
// Private AWDL transport policy, shared by the Intel HAL and its host tests.
// All callers serialize on the controller workloop. No pointers cross this API.
namespace LatitudeAWDLPeer {
constexpr uint64_t TU=1024000, StartTimeout=100000000, HomeGuard=32768000;
constexpr unsigned MaxPeers=16, MaxOutstanding=32, MaxEthernet=1514, MaxAir=1540;
inline uint16_t le16(const uint8_t*p){return uint16_t(p[0])|uint16_t(p[1])<<8;}
inline void put16(uint8_t*p,uint16_t v){p[0]=uint8_t(v);p[1]=uint8_t(v>>8);}
inline bool equal(const uint8_t*a,const uint8_t*b,unsigned n=6){for(unsigned i=0;i<n;++i)if(a[i]!=b[i])return false;return true;}
inline bool unicast(const uint8_t*p){uint8_t any=0;for(unsigned i=0;i<6;++i)any|=p[i];return any&&!(p[0]&1);}
inline void copy(uint8_t*d,const uint8_t*s,size_t n){for(size_t i=0;i<n;++i)d[i]=s[i];}
constexpr uint8_t BSSID[6]={0,0x25,0,0xff,0x94,0x73};
struct Availability {
 // Host-monotonic anchors must first be derived from the elected master's
 // validated sync state. A channel advertisement alone is not a time anchor.
 uint64_t anchorNS=0,clockDomain=0;uint32_t stepTU=0;uint32_t frequency[16]={};
 bool valid()const{
  if(!clockDomain||!stepTU||stepTU>64)return false;
  for(auto f:frequency)if(f&&f!=2437&&f!=5220&&f!=5745)return false;
  return true;
 }
};
struct Window {uint64_t startNS=0,endNS=0;uint32_t frequency=0;};
inline bool mutualWindow(const Availability&local,const Availability&peer,uint64_t earliest,Window&out){
 if(!local.valid()||!peer.valid()||local.clockDomain!=peer.clockDomain)return false;
 uint64_t t=earliest;if(t<local.anchorNS)t=local.anchorNS;if(t<peer.anchorNS)t=peer.anchorNS;
 // Never search without a bound or overflow a deadline. Two whole cycles of
 // either sequence are the search horizon; the iteration bound is an
 // additional CPU limit. Failure means no window was found in that search.
 uint64_t span=32*TU*(local.stepTU>peer.stepTU?local.stepTU:peer.stepTU);
 if(t>UINT64_MAX-span)return false;uint64_t limit=t+span;
 for(unsigned attempt=0;attempt<128&&t<limit;++attempt){
  uint64_t ls=local.stepTU*TU,ps=peer.stepTU*TU,la=t-local.anchorNS,pa=t-peer.anchorNS;
  unsigned li=unsigned((la/ls)%16),pi=unsigned((pa/ps)%16);
  uint64_t left=ls-la%ls,right=ps-pa%ps,duration=left<right?left:right;
  if(duration>limit-t)duration=limit-t;
  uint32_t f=local.frequency[li];
  // Keep one TU clear at both boundaries. Only whole TUs may be granted by
  // API83. A maximum 32-TU dwell leaves opportunities for infrastructure.
  if(f&&f==peer.frequency[pi]&&duration>=3*TU){
   uint64_t useful=(duration-2*TU)/TU;if(useful>32)useful=32;
   out={t+TU,t+TU+useful*TU,f};return true;
  }
  if(!duration)return false;t+=duration;
 }
 return false;
}
struct SessionCommand {uint32_t idAndColor,action,configuration,durationTU,repetitions,interval;};
static_assert(sizeof(SessionCommand)==24,"API83 SESSION_PROTECTION v1");
struct SessionEvent {uint32_t mac=0,status=0,start=0,configuration=0;};
enum class Route {Invalid,Association,Peer,Unowned};
inline Route route(const void*bytes,size_t length,uint32_t associatedMac,SessionEvent&event){
 if(!bytes||length!=16)return Route::Invalid;
 auto p=static_cast<const uint8_t*>(bytes);uint32_t values[4]={};
 for(unsigned i=0;i<4;++i)for(unsigned j=0;j<4;++j)values[i]|=uint32_t(p[4*i+j])<<(8*j);
 event={values[0],values[1],values[2],values[3]};
 if(event.status>1||event.start>1||event.configuration>3)return Route::Invalid;
 if(event.configuration==2||event.configuration==3)return Route::Peer;
 // GO-client protection (1) is not the infrastructure ASSOC session (0).
 return associatedMac<4&&event.mac==associatedMac&&event.configuration==0?Route::Association:Route::Unowned;
}
// A lease is supplied ONLY after the HAL has successfully created its distinct
// MAC/link/PHY/station/TX queue. This structure is not a resource allocator.
struct Lease {uint32_t epoch=0,mac=0,color=0,phy=0,link=0,station=0,queue=0;};
enum class Phase:uint32_t {Unbound,Home,AwaitStart,Active,Draining,Removing,Fault};
struct Scheduler {
 Scheduler()=default;Scheduler(const Scheduler&)=delete;Scheduler&operator=(const Scheduler&)=delete;
 Lease lease{};Phase phase=Phase::Unbound;uint32_t frequency=0,durationTU=0;
 uint64_t deadline=0,nextEligible=0,serial=0;unsigned outstanding=0;
 uint64_t starts=0,ends=0,failures=0,foreign=0,expired=0;
 bool bind(const Lease&l){
  // Context 0 is infrastructure; station 1/2 may belong to firmware AUX.
  if(phase!=Phase::Unbound||!l.epoch||l.mac==0||l.mac>3||l.color>255||l.phy==0||l.phy>3||l.link==0||l.link>15||l.station<3||l.station>15||l.queue==0||l.queue>511)return false;
  lease=l;phase=Phase::Home;return true;
 }
 static bool social(uint32_t f){return f==2437||f==5220||f==5745;}
 bool prepare(uint32_t f,uint32_t tu,uint64_t now,SessionCommand&out){
  // Caller separately verifies EEPROM/regulatory flags and configures only
  // the leased PHY. Submission is not evidence that firmware granted airtime.
  if(phase!=Phase::Home||outstanding||now<nextEligible||!social(f)||!tu||tu>32||now>UINT64_MAX-StartTimeout)return false;
  frequency=f;durationTU=tu;phase=Phase::AwaitStart;deadline=now+StartTimeout;
  out={lease.mac|(lease.color<<8),1,2,tu,0,0};return true;
 }
 void submitted(int error){if(error&&phase==Phase::AwaitStart){++failures;phase=Phase::Fault;}}
 // SESSION_PROTECTION_NOTIF v2 carries no host cookie. Never start a new
 // session until the previous END has arrived (or the firmware epoch resets).
 bool notification(uint32_t mac,uint32_t configuration,uint32_t status,uint32_t start,uint64_t now){
  if(phase==Phase::Unbound||mac!=lease.mac||configuration!=2){++foreign;return false;}
  if(status>1||start>1){++failures;phase=Phase::Fault;return true;}
  if(!status){++failures;phase=Phase::Fault;return true;}
  if(start){
   if(phase!=Phase::AwaitStart||now>=deadline||now>UINT64_MAX-durationTU*TU){++failures;phase=Phase::Fault;return true;}
   phase=Phase::Active;deadline=now+durationTU*TU;++starts;
  }else{
   ++ends;
   if(phase!=Phase::Active&&phase!=Phase::Draining&&phase!=Phase::Removing){++failures;phase=Phase::Fault;return true;}
   if(now>UINT64_MAX-HomeGuard){phase=Phase::Fault;return true;}
   nextEligible=now+HomeGuard;phase=outstanding?Phase::Draining:Phase::Home;
  }
  return true;
 }
 bool tick(uint64_t now){
  if((phase==Phase::AwaitStart||phase==Phase::Active)&&now>=deadline){++expired;phase=Phase::Fault;return true;}return false;
 }
private:
 bool acquire(uint32_t epoch,uint32_t f,uint64_t now,uint64_t&cookie){
  if(epoch!=lease.epoch||phase!=Phase::Active||f!=frequency||now>=deadline||outstanding==MaxOutstanding||serial==UINT64_MAX)return false;
  ++outstanding;cookie=++serial;return true;
 }
 // Completion tokens are individually tracked, so duplicate/out-of-order
 // firmware completions cannot release a different packet's DMA ownership.
 uint64_t tokens[MaxOutstanding]={};
public:
 bool take(uint32_t epoch,uint32_t f,uint64_t now,uint64_t&cookie){
  unsigned i=0;while(i<MaxOutstanding&&tokens[i])++i;
  if(i==MaxOutstanding||!acquire(epoch,f,now,cookie))return false;tokens[i]=cookie;return true;
 }
 bool complete(uint32_t epoch,uint64_t cookie){
  if(epoch!=lease.epoch||!cookie)return false;
  for(auto&t:tokens)if(t==cookie){t=0;--outstanding;if(!outstanding&&phase==Phase::Draining)phase=Phase::Home;return true;}return false;
 }
 bool remove(SessionCommand&out){
  if(phase==Phase::Unbound||phase==Phase::Home||phase==Phase::Removing)return false;
  out={lease.mac|(lease.color<<8),3,2,0,0,0};phase=Phase::Removing;return true;
 }
 bool unbind(){if(phase!=Phase::Home||outstanding)return false;lease={};phase=Phase::Unbound;return true;}
 void resetAfterDMAQuiesced(){// HAL must stop firmware/DMA before calling.
  lease={};phase=Phase::Unbound;outstanding=0;deadline=nextEligible=0;
  for(auto&t:tokens)t=0;
 }
};
struct Peer {
 uint8_t mac[6]={};uint64_t lastSeen=0,window=0;uint16_t newest=0;bool used=false,seenSequence=false;
 bool accept(uint16_t sequence){
  if(!seenSequence){newest=sequence;window=1;seenSequence=true;return true;}
  uint16_t forward=uint16_t(sequence-newest);
  if(forward&&forward<0x8000){window=forward>=64?1:(window<<forward)|1;newest=sequence;return true;}
  uint16_t age=uint16_t(newest-sequence);if(age>=64||(window&(uint64_t(1)<<age)))return false;
  window|=uint64_t(1)<<age;return true;
 }
};
struct DataPath {
 uint8_t local[6]={};Peer peers[MaxPeers];uint16_t sequence=0,wifiSequence=0;
 bool configure(const uint8_t*mac){if(!mac||!unicast(mac))return false;copy(local,mac,6);for(auto&p:peers)p={};sequence=wifiSequence=0;return true;}
 Peer*find(const uint8_t*mac,uint64_t now){for(auto&p:peers)if(p.used&&equal(mac,p.mac)&&now>=p.lastSeen&&now-p.lastSeen<=60000000000ULL)return &p;return nullptr;}
 bool observe(const uint8_t*mac,uint64_t now){
  if(!mac||!unicast(local)||!unicast(mac)||equal(local,mac))return false;
  // Preserve replay state on refresh, including peers older than the TTL.
  for(auto&p:peers)if(p.used&&equal(p.mac,mac)){if(now<p.lastSeen)return false;p.lastSeen=now;return true;}
  for(auto&p:peers)if(!p.used||(now>=p.lastSeen&&now-p.lastSeen>60000000000ULL)){p={};copy(p.mac,mac,6);p.lastSeen=now;p.used=true;return true;}return false;
 }
 static bool ipv6(const uint8_t*p,size_t n){return n>=40&&n<=1500&&(p[0]>>4)==6&&size_t(p[4])*256+p[5]+40==n;}
 static bool destination(const uint8_t*ether,const uint8_t*ip){
  if(!(ether[0]&1))return unicast(ether)&&ip[24]!=0xff;
  return ether[0]==0x33&&ether[1]==0x33&&ip[24]==0xff&&equal(ether+2,ip+36,4);
 }
 size_t encode(const uint8_t*ether,size_t n,uint8_t*out,size_t capacity,uint64_t now){
  if(!ether||!out||n<54||n>MaxEthernet||capacity<n+26||!equal(ether+6,local)||ether[12]!=0x86||ether[13]!=0xdd||!ipv6(ether+14,n-14)||!destination(ether,ether+14))return 0;
  if(!(ether[0]&1)&&!find(ether,now))return 0;
  // Non-QoS, no DS, no encryption/fragmentation. Peer traffic never carries
  // infrastructure BSSID or AP security keys. Firmware supplies the FCS.
  for(unsigned i=0;i<40;++i)out[i]=0;out[0]=8;
  copy(out+4,ether,6);copy(out+10,local,6);copy(out+16,BSSID,6);
  put16(out+22,uint16_t((wifiSequence++&0xfff)<<4));
  const uint8_t llc[8]={0xaa,0xaa,3,0,0x17,0xf2,8,0};copy(out+24,llc,8);
  put16(out+32,0x0403);put16(out+34,sequence++);out[38]=0x86;out[39]=0xdd;
  copy(out+40,ether+14,n-14);return n+26;
 }
 size_t decode(const uint8_t*air,size_t n,uint8_t*out,size_t capacity,uint64_t now){
  if(!air||!out||n<80||n>MaxAir||capacity<n-26||air[0]!=8||(air[1]&~8)||(le16(air+22)&15)||!equal(air+16,BSSID))return 0;
  const uint8_t llc[8]={0xaa,0xaa,3,0,0x17,0xf2,8,0};
  if(!equal(air+24,llc,8)||le16(air+32)!=0x0403||le16(air+36)||air[38]!=0x86||air[39]!=0xdd||!ipv6(air+40,n-40)||!destination(air+4,air+40))return 0;
  if(!(air[4]&1)&&!equal(air+4,local))return 0;
  auto peer=find(air+10,now);if(!peer||!peer->accept(le16(air+34)))return 0;
  copy(out,air+4,6);copy(out+6,air+10,6);out[12]=0x86;out[13]=0xdd;copy(out+14,air+40,n-40);return n-26;
 }
};
}
