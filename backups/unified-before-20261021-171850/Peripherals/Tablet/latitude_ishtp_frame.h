// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <stdint.h>
#include <stddef.h>
// Wire layout from Linux v6.19 ishtp/hbm.h and ishtp-hid.h. This is bounded
// packet framing, NOT firmware bring-up, DMA allocation or a live transport.
namespace LatitudeISH {
struct Fragment {uint8_t firmware,host;uint16_t length;bool complete;const uint8_t *payload;};
inline bool fragment(const uint8_t *data,size_t size,Fragment *out){
 if(!data || !out || size<4 || size>128)return false;
 uint32_t word=(uint32_t)data[0]|((uint32_t)data[1]<<8)|((uint32_t)data[2]<<16)|((uint32_t)data[3]<<24);
 size_t length=(word>>16)&0x1ff;
 if((word&0x7e000000u) || length!=size-4)return false;
 *out={data[0],data[1],(uint16_t)length,(word>>31)!=0,data+4};return true;
}
class Reassembly {
 uint8_t bytes[8192]={};size_t used=0;uint8_t firmware=0,host=0;bool active=false,completed=false;
public:
 void reset(){used=0;active=false;completed=false;}
 // Caller must serialize delivery and reset on timeout/reset/disconnect or new
 // connection generation. One instance per connected firmware/host address pair.
 // Return -1 invalid, 0 incomplete, 1 complete. Returned bytes valid until reset.
 int feed(const uint8_t *data,size_t size){
  if(completed)reset();
  Fragment f={};if(!fragment(data,size,&f) || f.host==0 || f.firmware==0 || !f.length){reset();return -1;}
  if(active && (f.host!=host || f.firmware!=firmware)){reset();return -1;}
  if(!active){firmware=f.firmware;host=f.host;active=true;}
  if(f.length>sizeof(bytes)-used){reset();return -1;}
  for(size_t i=0;i<f.length;i++)bytes[used++]=f.payload[i];
  completed=f.complete;return completed?1:0;
 }
 const uint8_t *data()const{return bytes;}size_t size()const{return used;}
};
struct HostMessage {uint8_t command,device,status,flags;uint16_t length;const uint8_t*payload;};
inline bool hostMessage(const uint8_t*data,size_t size,HostMessage*out){
 if(!data || !out || size<6)return false;
 size_t length=(unsigned)data[4]|((unsigned)data[5]<<8);if(length!=size-6)return false;
 *out={data[0],data[1],data[2],data[3],(uint16_t)length,data+6};return true;
}
inline bool accelerationReport(const uint8_t *report,size_t length,int32_t *axes){
 // Exact descriptor SHA c27ec39155c1dc56ea12ab0a231be92ebfd0e56e9089dd8482c4edc564b9f278.
 // Hub device ID must first be paired with this descriptor during enumeration.
 if(!report || !axes || length!=136 || report[0]!=3)return false;
 for(unsigned a=0;a<3;a++){size_t at=27+a*4;uint32_t v=0;for(unsigned b=0;b<4;b++)v|=(uint32_t)report[at+b]<<(b*8);axes[a]=(int32_t)v;}
 return true;
}
}
