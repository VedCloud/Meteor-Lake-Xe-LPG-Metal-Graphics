#pragma once
#include "LatitudeAWDLPeerTransport.h"
namespace LatitudeAWDLNative {
// Native 24H32 dequeue packets already contain Ethernet(802.3 length), LLC
// and AWDL SNAP/data headers. Preserve those native headers and sequence;
// feeding this packet to the ordinary Ethernet encoder would encapsulate twice.
constexpr size_t MaxPrepared=LatitudeAWDLPeer::MaxAir-10;
inline size_t airFrame(const uint8_t*prepared,size_t n,const uint8_t*local,
                      uint16_t wifiSequence,uint8_t*out,size_t capacity){
 using namespace LatitudeAWDLPeer;
 if(!prepared||!local||!out||n<70||n>MaxPrepared||capacity<n+10||!unicast(local)||
    !equal(prepared+6,local)||!DataPath::destination(prepared,prepared+30)||
    size_t(prepared[12])*256+prepared[13]!=n-14)return 0;
 const uint8_t llc[]={0xaa,0xaa,3,0,0x17,0xf2,8,0};
 if(!equal(prepared+14,llc,8)||le16(prepared+22)!=0x0403||
    le16(prepared+26)||prepared[28]!=0x86||prepared[29]!=0xdd||
    !DataPath::ipv6(prepared+30,n-30))return 0;
 for(unsigned i=0;i<24;++i)out[i]=0;out[0]=8;
 copy(out+4,prepared,6);copy(out+10,local,6);copy(out+16,BSSID,6);
 put16(out+22,uint16_t((wifiSequence&0xfff)<<4));copy(out+24,prepared+14,n-14);
 return n+10;
}
// Track originals separately from the mbufs used for DMA. No reference is
// returned until reclaim/reset has proven the radio copy is no longer in use.
template<class Packet> struct Originals {
 struct Slot {uint64_t cookie=0,credit=0;Packet packet{};int status=0;bool response=false;};
 Slot slots[LatitudeAWDLPeer::MaxOutstanding];unsigned count=0;
 Originals()=default;Originals(const Originals&)=delete;Originals&operator=(const Originals&)=delete;
 bool insert(uint64_t cookie,Packet p,uint64_t credit=0){
  if(!cookie||!p||count==LatitudeAWDLPeer::MaxOutstanding)return false;
  for(auto&s:slots)if(s.cookie==cookie)return false;
  for(auto&s:slots)if(!s.cookie){s={cookie,credit,p,0,false};++count;return true;}return false;
 }
 bool response(uint64_t cookie,int status){
  for(auto&s:slots)if(s.cookie==cookie&&cookie&&!s.response){s.response=true;s.status=status;return true;}return false;
 }
 Packet reclaim(uint64_t cookie,int missing,int&status,uint64_t*credit=nullptr){
  for(auto&s:slots)if(cookie&&s.cookie==cookie){auto p=s.packet;status=s.response?s.status:missing;if(credit)*credit=s.credit;s={};--count;return p;}return Packet{};
 }
 template<class Complete>void abortAfterDMAQuiesced(int status,Complete complete){
  for(auto&s:slots)if(s.cookie){auto p=s.packet;s={};--count;complete(p,status);}
 }
};
}
