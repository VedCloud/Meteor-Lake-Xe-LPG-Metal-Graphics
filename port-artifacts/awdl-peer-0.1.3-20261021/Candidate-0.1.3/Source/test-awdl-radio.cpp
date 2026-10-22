#include "LatitudeAWDLEngineBridge.h"
#include "LatitudeAWDLPeerTransport.h"
#include <cassert>
#include <cstdio>
int main(){using namespace LatitudeAWDLRadio;
 Queue queue;uint8_t raw[40]={};raw[0]=0xd0;memset(raw+4,255,6);uint8_t local[]={2,1,2,3,4,5},remote[]={2,4,3,2,1,8};memcpy(raw+10,remote,6);uint8_t bssid[]={0,0x25,0,0xff,0x94,0x73};memcpy(raw+16,bssid,6);raw[24]=127;raw[26]=0x17;raw[27]=0xf2;raw[28]=8;
 assert(incoming(raw,sizeof(raw),local));
 for(size_t n=0;n<40;n++)assert(!incoming(raw,n,local));
 raw[1]=0x40;assert(!incoming(raw,40,local));raw[1]=8;assert(incoming(raw,40,local));raw[1]=0;
 memcpy(raw+4,local,6);assert(incoming(raw,40,local));raw[4]^=4;assert(!incoming(raw,40,local));memset(raw+4,255,6);
 for(unsigned i=0;i<Capacity;i++)assert(queue.push(raw,40,5220,-50,i));assert(!queue.push(raw,40,5220,-50,99));assert(queue.dropped==1&&queue.count==Capacity);
 Received received;for(unsigned i=0;i<Capacity;i++){assert(queue.pop(received));assert(received.sequence==i+1&&received.uptimeNs==i&&!memcmp(raw,received.frame,40));for(unsigned j=40;j<MaxFrame;j++)assert(received.frame[j]==0);}
 assert(!queue.pop(received));queue.push(raw,40,5220,-50,10);queue.clear();assert(queue.dropped==2&&!queue.count);
 // Exercise the patched upstream wire codec at deliberately odd addresses.
 uint8_t storage[18]={};auto odd=const_cast<buf*>(buf_new_const(storage+1,16));assert(odd);
 uint16_t shortValue=0;uint32_t longValue=0;
 assert(write_le16(odd,0,0x4321)==2&&read_le16(odd,0,&shortValue)==2&&shortValue==0x4321);
 assert(write_be16(odd,1,0x1234)==2&&read_be16(odd,1,&shortValue)==2&&shortValue==0x1234);
 assert(write_le32(odd,3,0x89abcdef)==4&&read_le32(odd,3,&longValue)==4&&longValue==0x89abcdef);
 assert(write_be32(odd,7,0x10203040)==4&&read_be32(odd,7,&longValue)==4&&longValue==0x10203040);
 assert(read_le32(odd,INT32_MAX,&longValue)<0&&write_le32(odd,INT32_MAX,1)<0);
 assert(read_bytes(odd,0,nullptr,-1)<0&&write_bytes(odd,0,storage,-1)<0);buf_free(odd);
 Status self;self.flags=Associated|ExactHardware;self.frequency=5220;memcpy(self.mac,local,6);
 LatitudeAWDLEngineBridge sender;assert(sender.configure(self));uint8_t frame[MaxFrame+12];auto n=sender.action(frame,sizeof(frame),false);assert(n>52);
 Status peer=self;memcpy(peer.mac,remote,6);LatitudeAWDLEngineBridge receiver;assert(receiver.configure(peer));
 Received f;f.length=(uint32_t)(n-12);f.frequency=self.frequency;f.rssi=-50;memcpy(f.frame,frame+12,n-12);assert(receiver.receive(f)==RX_OK);assert(awdl_peers_length(receiver.state.peers.peers)==1);
 // Every truncated frame is bounded, even if a complete prefix creates a peer.
 for(unsigned length=0;length<f.length;length++){auto truncated=f;truncated.length=length;receiver.receive(truncated);}
 n=sender.action(frame,sizeof(frame),true);assert(n>52);f.length=(uint32_t)(n-12);memcpy(f.frame,frame+12,n-12);assert(receiver.receive(f)==RX_OK);
 // Compare the new kernel-safe peer codec against the real OWL data parser,
 // using the peer created above by actual action-frame exchange.
 LatitudeAWDLPeer::DataPath data;assert(data.configure(local));assert(data.observe(remote,1));
 uint8_t ethernet[54]={},air[80]={};memcpy(ethernet,remote,6);memcpy(ethernet+6,local,6);
 ethernet[12]=0x86;ethernet[13]=0xdd;ethernet[14]=0x60;ethernet[20]=59;ethernet[21]=64;
 assert(data.encode(ethernet,sizeof(ethernet),air,sizeof(air),2)==sizeof(air));
 auto dataBuffer=buf_new_const(air+24,sizeof(air)-24);buf *outputs[2]={},**next=outputs;
 ether_addr dataSource{},dataDestination{};memcpy(dataSource.octet,local,6);memcpy(dataDestination.octet,remote,6);
 assert(awdl_rx_data(dataBuffer,&next,&dataSource,&dataDestination,&receiver.state)==RX_OK);
 assert(next==outputs+1&&!memcmp(buf_data(outputs[0]),ethernet,sizeof(ethernet)));
 buf_free(outputs[0]);buf_free(dataBuffer);
 uint8_t owlData[256]={};auto owlLength=awdl_init_full_data_frame(owlData,&dataSource,&dataDestination,ethernet+14,40,&sender.state,&sender.wifi);
 unsigned radiotapLength=LatitudeAWDL::le16(owlData+2);assert(owlLength==int(radiotapLength+80));
 LatitudeAWDLPeer::DataPath decode;assert(decode.configure(remote)&&decode.observe(local,1));uint8_t decoded[54]={};
 assert(decode.decode(owlData+radiotapLength,80,decoded,54,2)==54&&!memcmp(decoded,ethernet,54));
 self.frequency=5500;assert(!sender.configure(self));self.frequency=5240;assert(sender.configure(self));
 puts("PASS actual AWDL radio protocol: bounded FIFO/drop/reset, action validation, OWL PSF/MIF generation to receive/parser, peer creation, truncation sweep, peer data codec interoperability in both directions. No hardware I/O.");
}
