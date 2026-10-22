// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include "latitude_ishtp_frame.h"
#include "latitude_accel_descriptor.h"
namespace LatitudeISH {
// Read-only HOSTIF frontend: never sets a feature or enables sensor power.
class HID {
public:enum class Phase {Idle,Enumerate,HIDDescriptor,ReportDescriptor,Feature,Poll,NeedsPower,Failed};
 struct Request {uint8_t data[7];unsigned length;};
 struct Sample {int32_t x,y,z;uint64_t firmwareTimestamp,hostMS;bool fresh;};
private:
 Phase phase=Phase::Idle;uint8_t ids[32]={},count=0,index=0,target=0;Request request={};bool queued=false,waiting=false,seenTimestamp=false;
 uint64_t deadline=0,previous=0,lastPoll=0,lastTimestamp=0;Sample sample={};const char*problem="none";
 bool fail(const char*message){phase=Phase::Failed;problem=message;queued=waiting=false;sample.fresh=false;return false;}
 static uint32_t word(const uint8_t*p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
 bool send(uint8_t command,uint8_t device,uint64_t now,bool report=false){
  if(queued||waiting)return fail("HOSTIF request overlap");request={};request.data[0]=command;request.data[1]=device;request.length=report?7:6;request.data[6]=3;queued=true;deadline=now+3000;return true;
 }
 bool next(uint64_t now){if(index>=count)return fail("measured accelerometer descriptor absent");phase=Phase::HIDDescriptor;return send(0,ids[index],now);}
 bool report(const uint8_t*p,size_t n,uint64_t now){
  if(n!=136||p[0]!=3)return false;
  if(p[1]!=2||(p[2]!=4&&p[2]!=5)){sample.fresh=false;return false;}
  uint64_t timestamp=word(p+11)|((uint64_t)word(p+15)<<32);if(!timestamp||(seenTimestamp&&timestamp<=lastTimestamp))return false;
  int32_t axes[3];if(!accelerationReport(p,n,axes))return false;int32_t mm[3];long long norm=0;
  for(unsigned i=0;i<3;i++){long long scaled=(long long)axes[i]*9806/1000000;if(scaled< -20000||scaled>20000)return false;mm[i]=(int32_t)scaled;norm+=scaled*scaled;}
  if(norm<49000000||norm>144000000)return false;
  seenTimestamp=true;lastTimestamp=timestamp;sample={mm[0],mm[1],mm[2],timestamp,now,true};return true;
 }
public:
 void reset(){*this=HID();}
 bool start(uint64_t now){reset();previous=now;phase=Phase::Enumerate;return send(33,0,now);}
 bool tick(uint64_t now){
  if(now<previous)return fail("HOSTIF clock reversed");previous=now;
  if((queued||waiting)&&now>=deadline)return fail("HOSTIF reply timeout");
  if(sample.fresh&&now-sample.hostMS>1000)sample.fresh=false;
  if(phase==Phase::Poll&&!queued&&!waiting&&now-lastPoll>=100){lastPoll=now;return send(4,target,now,true);}
  return phase!=Phase::Failed;
 }
 bool hasRequest()const{return queued;}
 const Request &peek()const{return request;}
 bool sent(){if(!queued)return false;queued=false;waiting=true;return true;}
 bool inputSingle(const uint8_t*p,size_t n,uint64_t now){
  if(!tick(now))return false;HostMessage m={};if(!hostMessage(p,n,&m)||m.status||m.flags)return fail("malformed HOSTIF reply");
  // Unsolicited reports cannot acknowledge a pending control request.
  if(m.command==6)return true; // aggregated unsolicited data is not used for orientation; poll responses remain required
  if(m.command==5){if(phase==Phase::Poll&&m.device==target)report(m.payload,m.length,now);return true;}
  if(!waiting||m.command!=(uint8_t)(request.data[0]|0x80)||m.device!=request.data[1])return fail("unexpected HOSTIF response");
  waiting=false;
  if(phase==Phase::Enumerate){
   if(m.length<1||!m.payload[0]||m.payload[0]>32||m.length!=1+(unsigned)m.payload[0]*9)return fail("invalid HID device table");count=m.payload[0];
   for(unsigned i=0;i<count;i++){uint32_t id=word(m.payload+1+9*i);if(id>255)return fail("unrepresentable HID device ID");ids[i]=(uint8_t)id;for(unsigned j=0;j<i;j++)if(ids[j]==ids[i])return fail("duplicate HID device ID");}
   return next(now);
  }
  if(phase==Phase::HIDDescriptor){if(!m.length||m.length>1024)return fail("invalid HID descriptor size");phase=Phase::ReportDescriptor;return send(1,ids[index],now);}
  if(phase==Phase::ReportDescriptor){
   bool matches=m.length==sizeof(measuredAccelDescriptor);if(matches)for(unsigned i=0;i<m.length;i++)if(m.payload[i]!=measuredAccelDescriptor[i]){matches=false;break;}
   if(!matches){index++;return next(now);}target=ids[index];phase=Phase::Feature;return send(2,target,now,true);
  }
  if(phase==Phase::Feature){
   if(m.length!=289||m.payload[0]!=3)return fail("wrong accelerometer feature report");
   if(m.payload[4]!=2){phase=Phase::NeedsPower;problem="sensor not in D0; no power write performed";return true;}
   phase=Phase::Poll;lastPoll=now;return send(4,target,now,true);
  }
  if(phase==Phase::Poll){report(m.payload,m.length,now);return true;}
  return fail("unexpected HOSTIF phase");
 }
 bool input(const uint8_t*p,size_t n,uint64_t now){
  if(!p||!n||n>8192)return fail("invalid HOSTIF buffer");
  size_t offset=0;unsigned messages=0;
  while(offset<n){if(++messages>64||n-offset<6)return fail("invalid HOSTIF packet chain");size_t length=(unsigned)p[offset+4]|((unsigned)p[offset+5]<<8);if(length>n-offset-6)return fail("truncated HOSTIF packet chain");if(!inputSingle(p+offset,length+6,now))return false;offset+=length+6;}
  return true;
 }
 Sample current(uint64_t now)const{auto result=sample;if(now<result.hostMS||now-result.hostMS>1000)result.fresh=false;return result;}
 Phase state()const{return phase;}const char*error()const{return problem;}
};
}
