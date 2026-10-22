// Included at the end of ItlIwx.cpp, after the API83 command declarations.
// All entry points run on the controller workloop. No commands run at attach.
struct latitude_mac_config_v2_native {
 uint32_t id_and_color,action,mac_type;uint8_t local_addr[6];uint16_t reserved;
 uint32_t filter_flags;uint16_t he_support,he_ap_support;uint32_t eht_support,nic_ack;
 uint8_t specific[16];
} __packed;
static_assert(sizeof(latitude_mac_config_v2_native)==52,"API83 MAC_CONFIG v2");
static uint64_t latitude_native_now(){uint64_t n;clock_get_uptime(&n);absolutetime_to_nanoseconds(n,&n);return n;}
int ItlIwx::latitudeNativeBind(OSObject*owner,LatitudeNativeComplete complete,
 LatitudeNativeReceive receive,const uint8_t*local,const uint8_t*peer,uint32_t frequency)
{
 auto sc=&com;auto ic=&sc->sc_ic;
 if(!getMainWorkLoop()->inGate()||!owner||!complete||!receive||!local||!peer)return EINVAL;
 int armed=0;if(!PE_parse_boot_argn("latitude-awdl-native",&armed,sizeof(armed))||armed!=1)return EPERM;
 if(!latitude_mld(sc)||iwx_lookup_cmd_ver(sc,IWX_MAC_CONF_GROUP,8)!=2||
    iwx_lookup_cmd_ver(sc,IWX_MAC_CONF_GROUP,IWX_SESSION_PROTECTION_CMD)!=1||
    iwx_lookup_cmd_ver(sc,IWX_DATA_PATH_GROUP,0x17)!=3||
    iwx_lookup_cmd_ver(sc,IWX_LONG_GROUP,IWX_PHY_CONTEXT_CMD)!=4||
    iwx_lookup_cmd_ver(sc,IWX_DATA_PATH_GROUP,8)!=2)return ENOTSUP;
 if(!dmaEpoch||dmaEpoch>UINT32_MAX)return ENOTSUP;
 if(latitudeNativeClosing||latitudeNativeOwner||latitudeNativePoisoned)return EBUSY;
 if(ic->ic_state!=IEEE80211_S_RUN||sc->ns_nstate!=IEEE80211_S_RUN||
    !(ic->ic_if.if_flags&IFF_RUNNING)||!ic->ic_bss||
    (sc->sc_flags&(IWX_FLAG_SHUTDOWN|IWX_FLAG_SCANNING|IWX_FLAG_BGSCAN|IWX_FLAG_TXFLUSH)))return EBUSY;
 if(!LatitudeAWDLPeer::unicast(local)||!LatitudeAWDLPeer::unicast(peer)||
    LatitudeAWDLPeer::equal(local,peer)||!LatitudeAWDLPeer::Scheduler::social(frequency))return EINVAL;
 // This driver owns MAC/link 0 and stations 0/1/2. Reserve MAC/link 1
 // for P2P ROC and MAC/link 2 for listener injection; P2P_DEVICE data TX
 // is not supported by the matching Linux API83 path. PHY 1 was created at
 // firmware initialization and must still be unreferenced by infrastructure.
 if(((iwx_node*)ic->ic_bss)->in_id!=0||IWX_NUM_PHY_CTX<2||sc->sc_phyctxt[1].ref)return EBUSY;
 ieee80211_channel*channel=nullptr;
 for(auto&c:ic->ic_channels)if(c.ic_freq==frequency&&c.ic_flags&&
    !(c.ic_flags&(IEEE80211_CHAN_PASSIVE|IEEE80211_CHAN_DFS))){channel=&c;break;}
 if(!channel)return EACCES;
 // Keep the prospective callback owner alive across synchronous commands,
 // which can sleep and allow a firmware reset on this workloop.
 owner->retain();uint64_t epoch=dmaEpoch;latitudeNativePoisoned=true;
 latitudeNativePreviousChannel=sc->sc_phyctxt[1].channel;
 sc->sc_phyctxt[1].ref=2;latitudeNativePhyReserved=true;
 auto node=(iwx_node*)::malloc(sizeof(iwx_node),0,M_NOWAIT|M_ZERO);
 if(!node){owner->release();latitudeNativePoisoned=false;sc->sc_phyctxt[1].ref=0;latitudeNativePhyReserved=false;latitudeNativePreviousChannel=nullptr;return ENOMEM;}
 node->in_ni.ni_refcnt=1;node->in_ni.ni_chan=channel;
 node->in_ni.ni_rates.rs_nrates=1;node->in_ni.ni_rates.rs_rates[0]=12|0x80;
 IEEE80211_ADDR_COPY(node->in_ni.ni_macaddr,peer);
 latitude_mac_config_v2_native mac{};mac.id_and_color=htole32(1);mac.action=htole32(1);
 mac.mac_type=htole32(6);memcpy(mac.local_addr,local,6);
 mac.filter_flags=htole32(1U<<5); // P2P discovery accepts probe requests
 auto send=[&](uint32_t id,size_t length,const void*p)->int{
  if(epoch!=dmaEpoch)return ENXIO;
  int result=iwx_send_cmd_pdu(sc,id,0,(uint16_t)length,p);
  return epoch==dmaEpoch?result:ENXIO;
 };
 int error=send(iwx_cmd_id(8,IWX_MAC_CONF_GROUP,0),sizeof(mac),&mac);
 latitude_link_v1 link{};link.link_id=htole32(1);link.mac_id=htole32(1);
 link.phy_id=htole32(IWX_FW_CTXT_INVALID);memcpy(link.addr,local,6);link.action=htole32(1);
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 // Do not reuse the infrastructure PHY helper: its channel-width decision
 // depends on AP state. The secondary context is explicitly 20 MHz.
 iwx_phy_context_cmd phy{};phy.id_and_color=htole32(1);phy.action=htole32(2);
 phy.lmac_id=htole32(iwx_lmac_id(sc,channel));
 phy.ci.band=IEEE80211_IS_CHAN_2GHZ(channel)?IWX_PHY_BAND_24:IWX_PHY_BAND_5;
 phy.ci.channel=htole32(ieee80211_chan2ieee(ic,channel));phy.ci.width=IWX_PHY_VHT_CHANNEL_MODE20;
 if(!error)error=send(IWX_PHY_CONTEXT_CMD,sizeof(phy),&phy);
 if(!error)sc->sc_phyctxt[1].channel=channel;
 link.action=htole32(2);link.phy_id=htole32(1);link.modify_mask=0;
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 link.modify_mask=htole32(3);link.active=htole32(1);link.cck_rates=htole32(1);link.ofdm_rates=htole32(0x15);
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 // Linux mld-sta.c's monitor path uses BCAST_MGMT with a listener link,
 // allowing raw 802.11 injection without AP association keys or a CAB queue.
 // P2P_DEVICE's mcast data station is NOT supported; do not invent one.
 mac.id_and_color=htole32(2);mac.mac_type=htole32(2);mac.action=htole32(1);
 mac.filter_flags=htole32(7); // listener accepts data, management/control, group
 if(!error)error=send(iwx_cmd_id(8,IWX_MAC_CONF_GROUP,0),sizeof(mac),&mac);
 link={};link.link_id=htole32(2);link.mac_id=htole32(2);link.action=htole32(1);
 link.phy_id=htole32(IWX_FW_CTXT_INVALID);memcpy(link.addr,local,6);
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 link.action=htole32(2);link.phy_id=htole32(1);
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 link.modify_mask=htole32(3);link.active=htole32(1);link.cck_rates=htole32(1);link.ofdm_rates=htole32(0x15);
 if(!error)error=send(iwx_cmd_id(9,IWX_MAC_CONF_GROUP,0),sizeof(link),&link);
 latitude_sta_v1 station{};station.sta_id=htole32(3);station.link_id=htole32(2);station.type=htole32(1);
 if(!error)error=send(iwx_cmd_id(0x0a,IWX_MAC_CONF_GROUP,0),sizeof(station),&station);
 int unicast=-1;
 if(!error){unicast=iwx_tvqm_enable_txq(sc,IWX_MGMT_TID,0,64,3);if(unicast<0)error=-unicast;}
 if(!error&&epoch!=dmaEpoch)error=ENXIO;
 if(error){
  // An ACK timeout can leave firmware owning DMA or a context. Never recycle
  // its identifiers or DMA on a guessed rollback. The normal reset path
  // quiesces DMA and clears this bounded failed transaction before retry.
  ::free(node);owner->release();return error;
 }
 LatitudeAWDLPeer::Lease lease{uint32_t(epoch),1,0,1,2,3,uint32_t(unicast)};
 if(!lease.epoch||!latitudeAWDLScheduler.bind(lease)){::free(node);owner->release();return EIO;}
 latitudeNativeOwner=owner;latitudeNativeComplete=complete;latitudeNativeReceive=receive;
 latitudeNativeNode=node;latitudeNativeQueue=unicast;latitudeNativeMulticastQueue=unicast;
 latitudeNativeEpoch=epoch;latitudeNativeData.configure(local);latitudeNativeData.observe(peer,latitude_native_now());
 latitudeNativePoisoned=false;return 0;
}
int ItlIwx::latitudeNativeWindow(uint32_t frequency,uint32_t durationTU)
{
 if(!getMainWorkLoop()->inGate()||latitudeNativeClosing||!latitudeNativeOwner||latitudeNativeEpoch!=dmaEpoch)return ENXIO;
 // A context is tied to its configured secondary PHY. Switching it while a
 // session is active is prohibited; other channels need a separate rebind.
 if(!latitudeNativeNode||latitudeNativeNode->in_ni.ni_chan->ic_freq!=frequency)return EINVAL;
 LatitudeAWDLPeer::SessionCommand cmd{};
 if(!latitudeAWDLScheduler.prepare(frequency,durationTU,latitude_native_now(),cmd))return EBUSY;
 static_assert(sizeof(cmd)==sizeof(iwx_session_prot_cmd),"API83 session ABI");
 int result=iwx_send_cmd_pdu(&com,iwx_cmd_id(IWX_SESSION_PROTECTION_CMD,IWX_MAC_CONF_GROUP,0),0,sizeof(cmd),&cmd);
 latitudeAWDLScheduler.submitted(result);return result;
}
int ItlIwx::latitudeNativeSubmit(mbuf_t original)
{
 // Zero transfers ownership of the original; errors leave it with the caller.
 if(!getMainWorkLoop()->inGate()||latitudeNativeClosing||!original||!latitudeNativeOwner||latitudeNativeEpoch!=dmaEpoch)return ENXIO;
 auto sc=&com;
 if(sc->sc_flags&(IWX_FLAG_SHUTDOWN|IWX_FLAG_TXFLUSH|IWX_FLAG_SCANNING|IWX_FLAG_BGSCAN))return EBUSY;
 size_t length=mbuf_pkthdr_len(original);
 if(length>LatitudeAWDLNative::MaxPrepared||length<70)return EMSGSIZE;
 uint8_t prepared[LatitudeAWDLNative::MaxPrepared],air[LatitudeAWDLPeer::MaxAir];
 if(mbuf_copydata(original,0,length,prepared))return EINVAL;
 auto now=latitude_native_now();
 if(!(prepared[0]&1)&&!latitudeNativeData.find(prepared,now))return EHOSTUNREACH;
 size_t n=LatitudeAWDLNative::airFrame(prepared,length,latitudeNativeData.local,
     latitudeNativeSequence++,air,sizeof(air));if(!n)return EINVAL;
 int qid=(prepared[0]&1)?latitudeNativeMulticastQueue:latitudeNativeQueue;
 if(qid<0||qid>=int(nitems(sc->txq)))return ENXIO;
 auto&ring=sc->txq[qid];if(!ring.desc||ring.queued>=ring.ring_count-1)return ENOBUFS;
 uint64_t credit=0;
 if(!latitudeAWDLScheduler.take(uint32_t(dmaEpoch),latitudeNativeNode->in_ni.ni_chan->ic_freq,now,credit))return EAGAIN;
 mbuf_t packet=nullptr;int error=mbuf_allocpacket(MBUF_DONTWAIT,n,nullptr,&packet);
 if(!error)error=mbuf_copyback(packet,0,n,air,MBUF_DONTWAIT);
 if(!error&&mbuf_len(packet)<24)error=mbuf_pullup(&packet,24);
 if(error){if(packet)mbuf_freem(packet);latitudeAWDLScheduler.complete(uint32_t(dmaEpoch),credit);return error;}
 uint64_t cookie=latitudeAWDLTx.issue();
 if(!latitudeNativeOriginals.insert(cookie,original,credit)){
  mbuf_freem(packet);latitudeAWDLScheduler.complete(uint32_t(dmaEpoch),credit);return ENOBUFS;
 }
 auto node=ieee80211_ref_node(&latitudeNativeNode->in_ni);
 error=iwx_tx(sc,packet,node,EDCA_AC_BE,cookie,qid);
 if(error){
  ieee80211_release_node(&sc->sc_ic,node);int unused=0;
  latitudeNativeOriginals.reclaim(cookie,0,unused);
  latitudeAWDLScheduler.complete(uint32_t(dmaEpoch),credit);return error;
 }
 ++latitudeAWDLTx.submitted;return 0;
}
void ItlIwx::latitudeNativeFinished(uint64_t cookie,bool reset)
{
 uint64_t credit=0;int status=0;
 auto packet=latitudeNativeOriginals.reclaim(cookie,reset?kIOReturnAborted:kIOReturnIOError,status,&credit);
 if(!packet)return;
 latitudeAWDLScheduler.complete(uint32_t(latitudeNativeEpoch),credit);
 auto owner=latitudeNativeOwner;auto complete=latitudeNativeComplete;
 if(owner&&complete){owner->retain();complete(owner,packet,reset?kIOReturnAborted:status);owner->release();}
 else mbuf_freem(packet);
}
bool ItlIwx::latitudeNativeInput(mbuf_t radio,int channel)
{
 if(latitudeNativeClosing||!latitudeNativeOwner||latitudeNativeEpoch!=dmaEpoch||!latitudeNativeNode||
    channel!=int(ieee80211_chan2ieee(&com.sc_ic,latitudeNativeNode->in_ni.ni_chan)))return false;
 size_t n=mbuf_pkthdr_len(radio);if(n>LatitudeAWDLPeer::MaxAir||n<80)return false;
 uint8_t bytes[LatitudeAWDLPeer::MaxAir],ethernet[LatitudeAWDLPeer::MaxEthernet];
 if(mbuf_copydata(radio,0,n,bytes))return false;
 auto length=latitudeNativeData.decode(bytes,n,ethernet,sizeof(ethernet),latitude_native_now());if(!length)return false;
 if(auto peer=latitudeNativeData.find(bytes+10,latitude_native_now()))peer->lastSeen=latitude_native_now();
 auto owner=latitudeNativeOwner;auto receive=latitudeNativeReceive;
 owner->retain();receive(owner,ethernet,length);owner->release();return true;
}
void ItlIwx::latitudeNativeResetAfterDMAQuiesced()
{
 if(latitudeNativeClosing)return;latitudeNativeClosing=true;
 latitudeNativeOriginals.abortAfterDMAQuiesced(kIOReturnAborted,[&](mbuf_t p,int status){
  if(latitudeNativeOwner&&latitudeNativeComplete)latitudeNativeComplete(latitudeNativeOwner,p,status);else mbuf_freem(p);
 });
 latitudeAWDLScheduler.resetAfterDMAQuiesced();
 if(latitudeNativeNode){::free(latitudeNativeNode);latitudeNativeNode=nullptr;}
 auto owner=latitudeNativeOwner;latitudeNativeOwner=nullptr;latitudeNativeComplete=nullptr;latitudeNativeReceive=nullptr;
 latitudeNativeQueue=latitudeNativeMulticastQueue=-1;latitudeNativeEpoch=0;latitudeNativePoisoned=false;
 if(latitudeNativePhyReserved){com.sc_phyctxt[1].ref=0;com.sc_phyctxt[1].channel=latitudeNativePreviousChannel;latitudeNativePreviousChannel=nullptr;latitudeNativePhyReserved=false;}
 latitudeNativeClosing=false;if(owner)owner->release();
}
