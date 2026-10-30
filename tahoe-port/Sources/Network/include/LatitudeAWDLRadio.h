#pragma once
#include <stdint.h>
#include <stddef.h>
// Private, versioned root-only IOKit boundary. Native Intel x86_64 layout.
namespace LatitudeAWDLRadio {
constexpr uint32_t Connection=0x4c415744, Version=0x0102;
constexpr unsigned Capacity=16, MaxFrame=2304;
enum Selector:uint32_t { Query=0, TransmitAction=1, ReceiveAction=2 };
enum Flag:uint32_t { Associated=1, TransmitArmed=2, ExactHardware=4 };
struct Status {
 uint32_t version=Version,flags=0,frequency=0,device=0;
 uint8_t mac[6]={};uint8_t reserved[2]={};
 uint64_t submitted=0,responses=0,firmwareSuccess=0,firmwareFailure=0;
 uint64_t reclaimed=0,aborted=0,missingResponse=0,lastCookie=0;
 uint64_t received=0,dropped=0;uint32_t queued=0,lastStatus=0;
};
struct Received {
 uint32_t version=Version,length=0,frequency=0;int32_t rssi=0;
 uint64_t uptimeNs=0,sequence=0;
 uint8_t frame[MaxFrame]={};
};
static_assert(sizeof(Status)==112,"status wire layout");
static_assert(sizeof(Received)==2336,"receive wire layout");
// Only accessed on the controller workloop / command gate. Drop newest on
// overflow, preserving existing queued frames; never publish payloads in IOReg.
struct Queue {
 Received slots[Capacity];unsigned head=0,count=0;uint64_t received=0,dropped=0;
 bool push(const uint8_t*p,size_t n,uint32_t frequency,int rssi,uint64_t ns){
  if(!p||n<40||n>MaxFrame)return false;
  ++received;if(count==Capacity){++dropped;return false;}
  auto &slot=slots[(head+count)%Capacity];slot={};slot.length=(uint32_t)n;
  slot.frequency=frequency;slot.rssi=rssi;slot.uptimeNs=ns;slot.sequence=received;
  for(size_t i=0;i<n;i++)slot.frame[i]=p[i];++count;return true;
 }
 bool pop(Received&out){if(!count)return false;out=slots[head];slots[head]={};head=(head+1)%Capacity;--count;return true;}
 void clear(){dropped+=count;for(auto &slot:slots)slot={};head=0;count=0;}
};
// Match a bounded unencrypted AWDL vendor action. Accept broadcast or an exact
// local receiver. Do not consume the original Wi-Fi packet or change filters.
inline bool incoming(const uint8_t*p,size_t n,const uint8_t*local){
 if(!p||!local||n<40||n>MaxFrame||p[0]!=0xd0||(p[1]&~0x08))return false;
 const uint8_t bssid[]={0,0x25,0,0xff,0x94,0x73};bool broadcast=true,ours=true,self=true;
 for(unsigned i=0;i<6;i++){if(p[16+i]!=bssid[i])return false;broadcast&=p[4+i]==0xff;ours&=p[4+i]==local[i];self&=p[10+i]==local[i];}
 if((!broadcast&&!ours)||self||(p[10]&1))return false;
 bool zero=true;for(unsigned i=0;i<6;i++)zero&=p[10+i]==0;if(zero)return false;
 if(p[24]!=0x7f||p[25]!=0||p[26]!=0x17||p[27]!=0xf2||p[28]!=8||p[31]||(p[30]!=0&&p[30]!=3))return false;
 size_t at=40;unsigned fields=0;
 while(at<n){if(n-at<3||++fields>256)return false;size_t bytes=(size_t)p[at+1]|(size_t)p[at+2]<<8;at+=3;if(bytes>n-at)return false;at+=bytes;}
 return at==n;
}
}
