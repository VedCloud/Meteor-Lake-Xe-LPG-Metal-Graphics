// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include "latitude_ishtp_hbm.h"
// MTL IPC register/doorbell layout from Linux v6.19. IO provides bounded MMIO
// read32/write32/flush. Caller owns PCI device and serializes on one workloop.
// Deliberately does not enable DMA or bus mastering; a sleeping firmware which
// needs Linux's DMA wake sequence must time out visibly, not be assumed ready.
namespace LatitudeISH {
template<class IO>class PIO {
public:enum class Phase {Idle,ResetWait,FirmwareWait,HBM,Connected,Failed};
private:
 IO &io;HBM busEngine;Phase phase=Phase::Idle;uint64_t previous=0,deadline=0,rxCount=0,txCount=0,pendingAt=0;bool clockSeen=false,hostReadyOwned=false,pending=false;
 uint32_t initialHost=0,txDoorbell=0;uint8_t tx[128]={};unsigned txLength=0;uint16_t resetId=0;
 const char *problem="none";
 bool fail(const char *why){phase=Phase::Failed;problem=why;pending=false;busEngine.reset();return false;}
 static uint32_t word(const uint8_t*p,unsigned n){uint32_t value=0;for(unsigned i=0;i<n;i++)value|=(uint32_t)p[i]<<(8*i);return value;}
 bool queueIPC(unsigned protocol,unsigned command,const uint8_t*p,unsigned n){
  if(pending||n>128||(!p&&n))return fail("IPC transmit queue collision");
  for(unsigned i=0;i<n;i++)tx[i]=p[i];txLength=n;pendingAt=previous;txDoorbell=0x80000000u|(protocol<<10)|(command<<16)|n;pending=true;return true;
 }
 bool sendHBM(const typename HBM::BusMessage&m){
  uint8_t wire[12]={0,0,(uint8_t)m.size,0x80};for(unsigned i=0;i<m.size;i++)wire[4+i]=m.bytes[i];return queueIPC(1,0,wire,4+m.size);
 }
 void drain(){
  if(!pending)return;uint32_t doorbell=io.read32(0x48);if(doorbell==0xffffffffu){fail("device inaccessible");return;}if(doorbell&0x80000000u)return;
  for(unsigned i=0;i<txLength;i+=4)io.write32(0xe0+i,word(tx+i,txLength-i<4?txLength-i:4));
  io.flush();io.write32(0x48,txDoorbell);io.flush();pending=false;txCount++;
 }
public:
 explicit PIO(IO &backend):io(backend){}
 bool start(uint64_t now){
  if(phase!=Phase::Idle)return false;initialHost=io.read32(0x38);uint32_t fw=io.read32(0x34);
  if(initialHost==0xffffffffu||fw==0xffffffffu)return fail("device inaccessible");
  if(fw&0xf0000u)return fail("foreign DMA still active");
  // This exclusive PCI session owns HOST_READY once it starts a reset.
  // Clear it on teardown even if firmware/BIOS left it set initially.
  hostReadyOwned=true;io.write32(0x38,initialHost|0x80);io.flush();
  resetId++;if(!resetId)resetId=1;uint8_t reset[4]={(uint8_t)resetId,(uint8_t)(resetId>>8),0,0};
  previous=now;clockSeen=true;deadline=now+10000;phase=Phase::ResetWait;
  if(!queueIPC(3,3,reset,4))return false;drain();return true;
 }
 void stop(){
  pending=false;busEngine.reset();
  if(hostReadyOwned){uint32_t now=io.read32(0x38);if(now!=0xffffffffu){io.write32(0x38,now&~0x80u);io.flush();}}
  hostReadyOwned=false;phase=Phase::Idle;clockSeen=false;
 }
 bool tick(uint64_t now){
  if(phase==Phase::Idle||phase==Phase::Failed)return false;
  if(clockSeen&&now<previous)return fail("monotonic clock reversed");previous=now;clockSeen=true;
  if(pending&&now-pendingAt>=2000)return fail("IPC transmitter busy timeout");
  if((phase==Phase::ResetWait||phase==Phase::FirmwareWait)&&now>=deadline)return fail("reset/firmware not ready; DMA wake may be required");
  if(phase==Phase::FirmwareWait){uint32_t status=io.read32(0x34);if(status==0xffffffffu)return fail("device inaccessible");if((status&3)==3){busEngine.start(now);phase=Phase::HBM;}}
  if(phase==Phase::HBM||phase==Phase::Connected){
   if(!busEngine.tick(now))return fail(busEngine.error());
   if(busEngine.state()==HBM::State::Connected)phase=Phase::Connected;
   if(!pending){typename HBM::BusMessage message;if(busEngine.pop(message)){if(!sendHBM(message))return false;}

   }
  }
  drain();return phase!=Phase::Failed;
 }
 // Copy/ack a single incoming doorbell. Never loops unbounded in IRQ context.
 // Return 1 only for a complete client message awaiting consumer acknowledgement.
 int interrupt(uint64_t now){
  if(phase==Phase::Idle||phase==Phase::Failed)return -1;
  if(!tick(now))return -1;
  uint32_t bell=io.read32(0x54);if(bell==0xffffffffu){fail("device inaccessible");return -1;}if(!(bell&0x80000000u))return 0;
  unsigned length=bell&0x3ff,protocol=(bell>>10)&15,command=(bell>>16)&15;
  if(length>128){io.write32(0x54,0);io.flush();fail("oversized IPC datagram");return -1;}
  uint8_t bytes[128]={};for(unsigned i=0;i<length;i+=4){uint32_t v=io.read32(0x60+i);for(unsigned j=0;j<4&&i+j<length;j++)bytes[i+j]=(uint8_t)(v>>(8*j));}
  // Incoming data has been copied before releasing firmware ownership.
  io.write32(0x54,0);io.flush();rxCount++;
  if(protocol==3){
   if(command==3){
    if(length!=4){fail("malformed firmware reset");return -1;}
    busEngine.reset();pending=false;bytes[2]=bytes[3]=0;
    if(!queueIPC(3,4,bytes,4))return -1;phase=Phase::FirmwareWait;deadline=now+2000;
   }else if(command==4){
    if(phase!=Phase::ResetWait||length!=4||word(bytes,4)!=resetId){fail("unexpected reset acknowledgement");return -1;}
    phase=Phase::FirmwareWait;deadline=now+2000;
   }else if(command!=2){fail("unsupported management command");return -1;}
   tick(now);return 0;
  }
  if(protocol!=1||phase==Phase::ResetWait||phase==Phase::FirmwareWait){fail("unexpected IPC protocol/state");return -1;}
  Fragment fragmentView={};if(!fragment(bytes,length,&fragmentView)){fail("invalid ISHTP header");return -1;}
  int completed=0;
  if(!fragmentView.host&&!fragmentView.firmware){
   if(!fragmentView.complete||!busEngine.bus(fragmentView.payload,fragmentView.length,now,busEngine.generation())){fail(busEngine.error());return -1;}
  }else{completed=busEngine.input(bytes,length,now,busEngine.generation());if(completed<0){fail(busEngine.error());return -1;}}
  tick(now);return completed;
 }
 bool sendClient(const uint8_t*data,unsigned length,uint64_t now){
  if(!tick(now)||phase!=Phase::Connected||pending||busEngine.hasOutgoing()||!data||!length||length>124)return false;
  if(!busEngine.takeTransmitCredit(busEngine.generation(),length))return false;
  uint8_t frame[128]={busEngine.firmwareAddress(),busEngine.hostAddress(),(uint8_t)length,0x80};for(unsigned i=0;i<length;i++)frame[4+i]=data[i];
  if(!queueIPC(1,0,frame,length+4))return false;drain();return true;
 }
 bool consumed(){return busEngine.consumed(busEngine.generation());}
 const uint8_t*data()const{return busEngine.data();}size_t size()const{return busEngine.size();}
 Phase state()const{return phase;}const char*error()const{return problem;}
 uint64_t received()const{return rxCount;}uint64_t sent()const{return txCount;}
};
}
