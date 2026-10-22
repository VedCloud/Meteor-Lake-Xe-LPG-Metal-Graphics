/* Private integration tests for the pinned GPL-3.0-or-later OWL core. No radio I/O. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "state.h"
#include "sync.h"
#include "schedule.h"
#include "rx.h"
#include "tx.h"
#include "ieee80211.h"
int main(void){
 struct ether_addr self={{2,0,0,0,0,1}},remote={{2,0,0,0,0,2}};
 struct awdl_state s;awdl_init_state(&s,"latitude-engine-test",&self,CHAN_OPCLASS_6,0);
 for(unsigned tu=0;tu<65536;tu++){
  uint64_t now=ieee80211_tu_to_usec(tu);
  assert(awdl_sync_current_aw(now,&s.sync)==(uint16_t)(tu/s.sync.aw_period));
  assert(awdl_sync_next_aw_tu(now,&s.sync)>0);
 }
 assert(awdl_peer_add(s.peers.peers,&remote,1,0,0)==PEERS_OK);
 struct awdl_peer*p=0;assert(awdl_peer_get(s.peers.peers,&remote,&p)==PEERS_OK&&p);
 assert(awdl_peers_length(s.peers.peers)==1);
 // Real transmitter and parser share the exact AWDL action-header format.
 unsigned char packet[8192]={0};int length=awdl_init_action(packet,AWDL_ACTION_PSF);assert(length>0&&length<8192);
 for(int n=0;n<length;n++){const struct buf*b=buf_new_const(packet,n);assert(awdl_parse_action_hdr(b)<0);buf_free(b);}
 const struct buf*b=buf_new_const(packet,length);assert(awdl_parse_action_hdr(b)>=0);buf_free(b);
 // A full action frame includes radiotap+802.11+AWDL TLVs, generated offline.
 struct ieee80211_state wifi={0};length=awdl_init_full_action_frame(packet,&s,&wifi,AWDL_ACTION_PSF);assert(length>24&&length<8192);
 assert(awdl_peer_remove(s.peers.peers,&remote,0,0)==PEERS_OK);assert(awdl_peers_length(s.peers.peers)==0);
 awdl_peers_free(s.peers.peers);
 puts("PASS real OWL sync-window arithmetic, peer lifecycle, action encoder/parser and truncated-header rejection under ASan/UBSan. No radio backend or AirDrop transfer tested.");
}
