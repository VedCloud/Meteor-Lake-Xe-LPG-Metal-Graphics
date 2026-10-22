#include "LatitudeAWDLPeerTransport.h"
#include <cassert>
#include <cstring>
#include <cstdio>
using namespace LatitudeAWDLPeer;
static void availability(){
 Availability a{},b{};Window w{};assert(!mutualWindow(a,b,0,w));
 a.clockDomain=b.clockDomain=99;a.stepTU=b.stepTU=16;a.frequency[1]=b.frequency[1]=5220;
 assert(mutualWindow(a,b,0,w)&&w.startNS==17*TU&&w.endNS==31*TU&&w.frequency==5220);
 b.clockDomain=100;assert(!mutualWindow(a,b,0,w));b.clockDomain=99;
 b.anchorNS=8*TU;assert(mutualWindow(a,b,0,w)&&w.startNS==25*TU&&w.endNS==31*TU);
 b.frequency[1]=2437;assert(!mutualWindow(a,b,0,w));
 a.frequency[1]=b.frequency[1]=5500;assert(!mutualWindow(a,b,0,w));
 a.frequency[1]=b.frequency[1]=5220;b.anchorNS=0;
 assert(!mutualWindow(a,b,UINT64_MAX,w));a.stepTU=65;assert(!mutualWindow(a,b,0,w));
 // Every returned grant is inside both advertised windows and their channel
 // intersection, including unequal periods, offsets and cycle wrap.
 for(unsigned l=3;l<=64;++l)for(unsigned p=3;p<=64;++p){
  a.stepTU=l;b.stepTU=p;a.anchorNS=3*TU;b.anchorNS=7*TU;
  for(unsigned i=0;i<16;++i){a.frequency[i]=i%3?5220:2437;b.frequency[i]=i%2?5220:5745;}
  if(mutualWindow(a,b,101*TU,w)){
   assert(w.startNS>=101*TU&&w.endNS>w.startNS&&w.endNS-w.startNS<=32*TU);
   auto li=(w.startNS-a.anchorNS)/(l*TU)%16,pi=(w.startNS-b.anchorNS)/(p*TU)%16;
   assert(a.frequency[li]==w.frequency&&b.frequency[pi]==w.frequency);
   assert((w.endNS-1-a.anchorNS)/(l*TU)%16==li&&(w.endNS-1-b.anchorNS)/(p*TU)%16==pi);
  }
 }
}
static void schedule(){
 SessionEvent e;uint8_t notification[17]={};
 assert(route(nullptr,16,0,e)==Route::Invalid);
 for(unsigned n=0;n<16;++n)assert(route(notification,n,0,e)==Route::Invalid);
 assert(route(notification,17,0,e)==Route::Invalid&&route(notification+1,16,0,e)==Route::Association);
 notification[12]=2;assert(route(notification,16,0,e)==Route::Peer);
 notification[12]=1;assert(route(notification,16,0,e)==Route::Unowned);
 notification[12]=0;notification[0]=1;assert(route(notification,16,0,e)==Route::Unowned);
 notification[4]=2;assert(route(notification,16,1,e)==Route::Invalid);
 Scheduler s;SessionCommand cmd{};uint64_t cookie=0;
 assert(!s.prepare(5220,16,1,cmd)&&!s.take(1,5220,1,cookie));
 Lease lease{1,1,0,1,1,3,7};auto bad=lease;bad.mac=0;assert(!s.bind(bad));assert(s.bind(lease));
 assert(!s.bind(lease)&&!s.prepare(5500,16,1,cmd)&&!s.prepare(5220,33,1,cmd));
 assert(s.prepare(5220,16,1,cmd)&&cmd.idAndColor==1&&cmd.action==1&&cmd.configuration==2&&cmd.durationTU==16);
 assert(!s.take(1,5220,2,cookie)); // synchronous command ACK is not START
 assert(!s.notification(0,0,1,1,2)&&s.phase==Phase::AwaitStart);
 assert(!s.notification(1,3,1,1,2)&&s.phase==Phase::AwaitStart);
 assert(s.notification(1,2,1,1,3)&&s.phase==Phase::Active);
 assert(!s.take(2,5220,4,cookie)&&!s.take(1,2437,4,cookie));
 uint64_t tokens[MaxOutstanding];for(auto&t:tokens)assert(s.take(1,5220,4,t));
 assert(!s.take(1,5220,4,cookie));assert(!s.complete(2,tokens[0]));
 assert(s.notification(1,2,1,0,5)&&s.phase==Phase::Draining);
 assert(!s.take(1,5220,6,cookie)&&!s.unbind());
 for(int i=MaxOutstanding-1;i>=0;--i){assert(s.complete(1,tokens[i]));assert(!s.complete(1,tokens[i]));}
 assert(s.phase==Phase::Home&&!s.prepare(5220,16,6,cmd));
 assert(s.prepare(5745,32,5+HomeGuard,cmd));assert(s.tick(5+HomeGuard+StartTimeout));
 assert(s.phase==Phase::Fault&&!s.take(1,5745,6+HomeGuard+StartTimeout,cookie));
 assert(s.remove(cmd)&&cmd.action==3&&!s.remove(cmd));
 assert(s.notification(1,2,1,0,6+HomeGuard+StartTimeout));assert(s.unbind());
 assert(s.bind(lease));assert(s.prepare(2437,1,1000000000,cmd));s.submitted(5);assert(s.phase==Phase::Fault);
 s.resetAfterDMAQuiesced();assert(s.phase==Phase::Unbound&&!s.outstanding);
 assert(s.bind(lease));assert(s.prepare(5220,1,1,cmd));
 assert(s.notification(1,2,1,1,2));assert(!s.take(1,5220,2+TU,cookie));assert(s.tick(2+TU));
 s.resetAfterDMAQuiesced();assert(s.bind(lease));assert(!s.prepare(5220,1,UINT64_MAX,cmd));
}
static void codec(){
 uint8_t a[]={2,0,0,0,0,1},b[]={2,0,0,0,0,2};DataPath tx{},rx{};
 assert(tx.configure(a)&&rx.configure(b));assert(tx.observe(b,1)&&rx.observe(a,1));
 uint8_t ethernet[1514]={},air[1540]={},out[1514]={};
 memcpy(ethernet,b,6);memcpy(ethernet+6,a,6);ethernet[12]=0x86;ethernet[13]=0xdd;
 ethernet[14]=0x60;ethernet[18]=uint8_t(1460>>8);ethernet[19]=uint8_t(1460);ethernet[20]=17;ethernet[21]=64;
 for(unsigned i=54;i<sizeof(ethernet);++i)ethernet[i]=uint8_t(i);
 auto n=tx.encode(ethernet,sizeof(ethernet),air,sizeof(air),2);assert(n==1540);
 assert(rx.decode(air,n,out,sizeof(out),2)==1514&&!memcmp(out,ethernet,1514));
 assert(!rx.decode(air,n,out,sizeof(out),2)); // replay
 for(size_t size=0;size<n;++size){DataPath fresh{};fresh.configure(b);fresh.observe(a,1);assert(!fresh.decode(air,size,out,sizeof(out),2));}
 for(unsigned i=0;i<40;++i){auto save=air[i];air[i]^=0x40;DataPath fresh{};fresh.configure(b);fresh.observe(a,1);auto rc=fresh.decode(air,n,out,sizeof(out),2);if(i!=2&&i!=3&&i!=22&&i!=23&&i!=34&&i!=35)assert(!rc);air[i]=save;}
 assert(!tx.encode(ethernet,1514,air,1539,2));assert(!tx.encode(ethernet,1514,air,1540,60000000002ULL));
 assert(!rx.decode(air,n,out,1513,2));assert(!rx.decode(air,n,out,1514,0));
 // IPv6 multicast maps the low 32 destination bits to 33:33:xx:xx:xx:xx.
 ethernet[0]=0x33;ethernet[1]=0x33;ethernet[2]=0;ethernet[3]=0;ethernet[4]=0;ethernet[5]=0xfb;
 ethernet[38]=0xff;ethernet[39]=2;ethernet[53]=0xfb;
 n=tx.encode(ethernet,1514,air,1540,2);assert(n==1540);assert(rx.decode(air,n,out,1514,2)==1514);
 ethernet[5]^=1;assert(!tx.encode(ethernet,1514,air,1540,2));
 Peer peer{};assert(peer.accept(65534)&&peer.accept(0)&&peer.accept(65535)&&!peer.accept(65535));
 assert(peer.accept(63)&&!peer.accept(65534)&&!peer.accept(0)&&peer.accept(62));
 for(unsigned i=0;i<100000;++i){uint16_t seq=uint16_t(i+100);assert(peer.accept(seq));assert(!peer.accept(seq));}
 DataPath bounded{};bounded.configure(a);for(unsigned i=0;i<MaxPeers;++i){b[5]=uint8_t(i+2);assert(bounded.observe(b,1));}
 b[5]=99;assert(!bounded.observe(b,2));assert(bounded.observe(b,60000000002ULL));
}
int main(){availability();schedule();codec();puts("PASS peer transport: session grant/deadlines/isolation, credits/completion/reset, IPv6 AWDL frames, multicast, replay/wrap, truncation and capacity. No hardware I/O.");}
