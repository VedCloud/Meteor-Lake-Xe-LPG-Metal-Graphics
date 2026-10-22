#pragma once
#include "LatitudeAWDLRadio.h"
#include "LatitudeAWDLFrame.h"
#include <string.h>
#include <algorithm>
extern "C" {
#include "state.h"
#include "rx.h"
#include "tx.h"
#include "wire.h"
}
// A current-channel radio adapter, not Apple's native AWDL data interface.
struct LatitudeAWDLEngineBridge {
 awdl_state state{};ieee80211_state wifi{};bool active=false;uint32_t frequency=0;uint8_t mac[6]={};
 ~LatitudeAWDLEngineBridge(){close();}
 LatitudeAWDLEngineBridge()=default;
 LatitudeAWDLEngineBridge(const LatitudeAWDLEngineBridge&)=delete;
 void close(){if(active)awdl_peers_free(state.peers.peers);active=false;}
 bool configure(const LatitudeAWDLRadio::Status&s){
  using namespace LatitudeAWDLRadio;
  if(s.version!=Version||(s.flags&(Associated|ExactHardware))!=(Associated|ExactHardware)){close();return false;}
  if(active&&frequency==s.frequency&&!memcmp(mac,s.mac,6))return true;
  if((s.mac[0]&1)||!memcmp(s.mac,"\0\0\0\0\0\0",6)){close();return false;}
  unsigned channel=0,opclass=0;
  if(s.frequency>=2412&&s.frequency<=2472&&(s.frequency-2407)%5==0){channel=(s.frequency-2407)/5;opclass=81;}
  if(s.frequency==5180||s.frequency==5200||s.frequency==5220||s.frequency==5240){channel=(s.frequency-5000)/5;opclass=115;}
  if(s.frequency==5745||s.frequency==5765||s.frequency==5785||s.frequency==5805){channel=(s.frequency-5000)/5;opclass=124;}
  if(s.frequency==5825){channel=165;opclass=125;}
  if(!channel){close();return false;} // No DFS/passive or guessed regulatory class.
  close();state={};wifi={};awdl_chan chan{};chan.opclass.chan_num=(uint8_t)channel;chan.opclass.opclass=(uint8_t)opclass;
  ether_addr self{};memcpy(self.octet,s.mac,6);
  awdl_init_state(&state,"Latitude5350-radio",&self,chan,clock_time_us());
  // Driver RSSI units have not been qualified as dBm. Keep the bounded parser
  // independent of that unverified calibration rather than drop valid frames.
  state.filter_rssi=0;frequency=s.frequency;memcpy(mac,s.mac,6);active=true;return true;
 }
 size_t action(uint8_t*out,size_t capacity,bool mif){
  if(!active||!out||capacity<12+LatitudeAWDLRadio::MaxFrame)return 0;
  uint8_t raw[8192]={};int length=awdl_init_full_action_frame(raw,&state,&wifi,mif?AWDL_ACTION_MIF:AWDL_ACTION_PSF);
  if(length<8||length>(int)sizeof(raw))return 0;
  size_t header=LatitudeAWDL::le16(raw+2);if(header<8||header>(size_t)length)return 0;
  size_t payload=(size_t)length-header;if(payload>LatitudeAWDLRadio::MaxFrame||payload+12>capacity)return 0;
  memset(out,0,12);out[2]=12;out[4]=8;out[8]=(uint8_t)frequency;out[9]=(uint8_t)(frequency>>8);
  memcpy(out+12,raw+header,payload);LatitudeAWDL::Frame parsed;
  if(!LatitudeAWDL::radiotap(out,payload+12,(uint16_t)frequency,mac,parsed))return 0;
  return payload+12;
 }
 int receive(const LatitudeAWDLRadio::Received&frame){
  using namespace LatitudeAWDLRadio;
  if(!active||frame.version!=Version||frame.frequency!=frequency||!incoming(frame.frame,frame.length,mac))return RX_UNEXPECTED_FORMAT;
  uint64_t now=clock_time_us();if(now>30000000)awdl_peers_remove(state.peers.peers,now-30000000,nullptr,nullptr);
  ether_addr src{},dst{};memcpy(src.octet,frame.frame+10,6);memcpy(dst.octet,frame.frame+4,6);
  if(awdl_peers_length(state.peers.peers)>=64&&awdl_peer_get(state.peers.peers,&src,nullptr)!=PEERS_OK)return RX_IGNORE;
  auto buf=buf_new_const(frame.frame+24,frame.length-24);if(!buf)return RX_IGNORE;
  int rc=awdl_rx_action(buf,(signed char)std::clamp(frame.rssi,-128,127),now,&src,&dst,&state);buf_free(buf);return rc;
 }
};
