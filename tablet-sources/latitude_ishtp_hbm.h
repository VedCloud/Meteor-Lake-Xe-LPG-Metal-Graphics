// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include "latitude_ishtp_frame.h"
// HBM 1.0 wire definitions: Linux v6.19 drivers/hid/intel-ish-hid/ishtp/hbm.h.
// Serialized, bounded protocol engine. The PCI owner supplies only complete bus
// messages (fw=host=0), consumes the outgoing queue and reports link resets.
// This engine does not perform MMIO, allocate DMA, or imply a sensor is active.
namespace LatitudeISH {
class HBM {
public:
 enum class State {Idle,Start,Enumerate,Properties,Connect,Connected,Failed};
 struct BusMessage {uint8_t bytes[8];unsigned size;};
private:
 State phase=State::Idle;uint64_t epoch=0,previous=0,deadline=0,receiveDeadline=0;bool clockSeen=false;
 uint8_t addresses[32]={},firmware=0,host=1,checking=0;uint32_t maximum=0;
 bool singleReceive=false,receiveGranted=false,deliveryPending=false;
 unsigned credits=0,head=0,count=0;BusMessage outgoing[4]={};Reassembly receive;
 const char *failure="none";
 static uint32_t le32(const uint8_t*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
 bool fail(const char*why){phase=State::Failed;failure=why;credits=0;count=0;receiveGranted=false;deliveryPending=false;receive.reset();return false;}
 bool clock(uint64_t now){if(clockSeen&&now<previous)return fail("clock reversed");clockSeen=true;previous=now;if(phase!=State::Connected&&phase!=State::Idle&&phase!=State::Failed&&now>=deadline)return fail("handshake timeout");if(phase==State::Connected&&receive.size()&&now>=receiveDeadline)return fail("client receive timeout");return phase!=State::Failed;}
 bool queue(uint8_t cmd,uint8_t a=0,uint8_t b=0,bool flow=false){if(count==4)return fail("bus queue full");auto &m=outgoing[(head+count)%4];m={};m.bytes[0]=cmd;m.bytes[1]=a;m.bytes[2]=b;m.size=flow?8:4;count++;return true;}
 bool nextProperty(uint64_t now){
  for(unsigned n=(unsigned)checking+1;n<256;n++)if(addresses[n/8]&(1u<<(n%8))){checking=(uint8_t)n;phase=State::Properties;deadline=now+7000;return queue(5,checking);}
  if(!firmware)return fail("HID client GUID absent");phase=State::Connect;deadline=now+15000;return queue(6,firmware,host);
 }
public:
 void reset(){uint64_t next=epoch+1;*this=HBM();epoch=next;}
 bool start(uint64_t now){reset();phase=State::Start;previous=now;clockSeen=true;deadline=now+7000;return queue(1,0,0)&&((outgoing[head].bytes[3]=1),true);}
 bool tick(uint64_t now){return clock(now);}
 bool hasOutgoing()const{return count!=0;}
 bool pop(BusMessage &out){if(!count)return false;out=outgoing[head];head=(head+1)%4;count--;return true;}
 bool bus(const uint8_t*p,size_t n,uint64_t now,uint64_t generation){
  if(generation!=epoch)return false; // Old completion cannot mutate a new link.
  if(!clock(now)||!p||n<1||n>36)return false;
  if(p[0]==3)return fail("firmware requested stop");
  if(phase==State::Start){
   if(n!=4||p[0]!=0x81||p[1]!=1)return fail("unsupported HBM version reply");
   phase=State::Enumerate;deadline=now+7000;return queue(4);
  }
  if(phase==State::Enumerate){
   if(n!=36||p[0]!=0x84||p[1]||p[2]||p[3]||(p[4]&1))return fail("malformed enumeration");
   for(unsigned i=0;i<32;i++)addresses[i]=p[i+4];return nextProperty(now);
  }
  if(phase==State::Properties){
   if(n!=32||p[0]!=0x85||p[1]!=checking||p[2]||p[3])return fail("malformed properties");
   const uint8_t hid[16]={0x58,0xcd,0xae,0x33,0x79,0xb6,0x54,0x4e,0x9b,0xd9,0xa0,0x4d,0x34,0xf0,0xc2,0x26};
   bool match=true;for(unsigned i=0;i<16;i++)if(p[4+i]!=hid[i])match=false;
   if(match){
    if(firmware)return fail("ambiguous HID clients");
    uint32_t size=le32(p+24);if(!p[21]||p[22]||p[23]>1||size<136||size>8192||p[29]||p[30]||p[31])return fail("unsupported HID properties");
    firmware=checking;maximum=size;singleReceive=p[23]!=0;
   }
   return nextProperty(now);
  }
  if(phase==State::Connect){
   if(n!=4||p[0]!=0x86||p[1]!=firmware||p[2]!=host||p[3])return fail("HID connection refused");
   phase=State::Connected;receiveGranted=true;return queue(8,firmware,host,true);
  }
  if(phase==State::Connected){
   if(p[0]==7||p[0]==0x87)return fail("client disconnected");
   if(n!=8||p[0]!=8||p[1]!=firmware||(p[2]!=host&&!(singleReceive&&p[2]==0)))return fail("unexpected connected bus message");
   for(unsigned i=3;i<8;i++)if(p[i])return fail("malformed flow credit");
   if(credits>=255)return fail("flow credit overflow");credits++;return true;
  }
  return false;
 }
 // A complete ISHTP message consumes one receive credit, irrespective of the
 // number of IPC fragments. No credit is returned until the consumer commits.
 int input(const uint8_t*p,size_t n,uint64_t now,uint64_t generation){
  if(generation!=epoch)return -1;
  if(!clock(now)||phase!=State::Connected||!receiveGranted||deliveryPending)return -1;
  Fragment f={};if(!fragment(p,n,&f)||f.firmware!=firmware||f.host!=host){fail("wrong receive route");return -1;}
  if(!receive.size())receiveDeadline=now+1000;
  int result=receive.feed(p,n);if(result<0||receive.size()>maximum){fail("invalid client fragment");return -1;}
  if(result==1){receiveGranted=false;deliveryPending=true;}return result;
 }
 bool consumed(uint64_t generation){
  if(generation!=epoch||phase!=State::Connected||!deliveryPending)return false;
  if(!queue(8,firmware,host,true))return false;
  deliveryPending=false;receiveGranted=true;receive.reset();return true;
 }
 bool takeTransmitCredit(uint64_t generation,size_t length){
  if(generation!=epoch||phase!=State::Connected||!credits||!length||length>maximum)return false;credits--;return true;
 }
 const uint8_t*data()const{return deliveryPending?receive.data():nullptr;}
 size_t size()const{return deliveryPending?receive.size():0;}
 uint64_t generation()const{return epoch;}
 State state()const{return phase;}
 const char*error()const{return failure;}
 unsigned transmitCredits()const{return credits;}
 uint8_t firmwareAddress()const{return firmware;}
 uint8_t hostAddress()const{return host;}
};
}
