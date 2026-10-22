#pragma once
// Exact x86_64 IO80211Family UUID 13D3592A-A6BE-35E7-984C-0ACD90A76356.
// Generated from 24H32 vtables and matching concrete native override signatures.
// Native object allocation and traffic capability are not enabled by this header.
#include <Airport/Apple80211.h>
class IO80211VirtualInterface : public IO80211SkywalkInterface {
 OSDeclareAbstractStructors(IO80211VirtualInterface)
public:
 virtual void free() override; // 0x90
 virtual IOReturn configureReport(IOReportChannelList*, unsigned int, void*, void*) override; // 0x428
 virtual IOReturn updateReport(IOReportChannelList*, unsigned int, void*, void*) override; // 0x430
 virtual bool start(IOService*) override; // 0x5c0
 virtual void stop(IOService*) override; // 0x5c8
 virtual bool terminate(IOOptionBits) override; // 0x600
 virtual bool attach(IOService *) override; // 0x670
 virtual void detach(IOService *) override; // 0x678
 virtual IOReturn newUserClient(task*, void*, unsigned int, OSDictionary*, IOUserClient**) override; // 0x770
 virtual const char* stringFromReturn(int) override; // 0x780
 virtual IOReturn enable(unsigned int) override; // 0x850
 virtual IOReturn prepareBSDInterface(struct __ifnet*, unsigned int) override; // 0x8e8
 virtual IOReturn processBSDCommand(struct __ifnet*, unsigned int, void*) override; // 0x910
 virtual SInt32 setInterfaceEnable(bool) override; // 0x928
 virtual UInt32 getFeatureFlags() override; // 0x9b0
 virtual bool bpfTap(unsigned int, unsigned int) override; // 0x9d8
 virtual bool setMulticastAddresses(ether_addr const*, unsigned int) override; // 0xa80
 virtual void * createPeerManager() override; // 0xb00
 virtual void postMessage(unsigned int, void*, unsigned long, bool) override; // 0xb10
 virtual void logTxPacket(IO80211NetworkPacket*, PacketSkywalkScratch*, apple80211_wme_ac, bool) override; // 0xb30
 virtual void logTxCompletionPacket(IO80211NetworkPacket*, PacketSkywalkScratch*, unsigned char*, apple80211_wme_ac, int, unsigned int, bool, bool) override; // 0xb38
 virtual IOReturn inputPacket(IO80211NetworkPacket*, packet_info_tag*, ether_header*, bool*, bool) override; // 0xb48
 virtual void logSkywalkTxReqPacket(IO80211NetworkPacket*, PacketSkywalkScratch*, unsigned char*, apple80211_wme_ac, bool) override; // 0xb58
 virtual SInt64 pendingPackets(unsigned char) override; // 0xb60
 virtual SInt64 packetSpace(unsigned char) override; // 0xb68
 virtual IO80211LinkState linkState() override; // 0xb88
 virtual void setScanningState(unsigned int, bool, apple80211_scan_data*, int) override; // 0xb90
 virtual void updateLinkParameters(apple80211_interface_availability*) override; // 0xbb0
 virtual void updateInterfaceCoexRiskPct(unsigned long long) override; // 0xbb8
 virtual void setInterfaceNF(apple80211_channel, long long) override; // 0xbe8
 virtual void setDebugFlags(unsigned long long, unsigned int) override; // 0xc00
 virtual SInt64 debugFlags() override; // 0xc08
 virtual void setInterfaceChipCounters(apple80211_stat_report*, apple80211_chip_counters_tx*, apple80211_chip_error_counters_tx*, apple80211_chip_counters_rx*) override; // 0xc10
 virtual void setInterfaceMIBdot11(apple80211_stat_report*, apple80211_ManagementInformationBasedot11_counters*) override; // 0xc18
 virtual void setFrameStats(apple80211_stat_report*, apple80211_frame_counters*) override; // 0xc20
 virtual void setInfraSpecificFrameStats(apple80211_stat_report*, apple80211_infra_specific_stats*) override; // 0xc28
 virtual SInt64 getWmeTxCounters(unsigned long long*) override; // 0xc30
 virtual void setWoWEnabled(bool) override; // 0xc40
 virtual bool wowEnabled() override; // 0xc48
 virtual void printDataPath(userPrintCtx*) override; // 0xc50
private: virtual void _abi_0c68() __asm__("__ZN23IO80211VirtualInterface21findOrCreateFlowQueueE20IO80211FlowQueueHash") override; public:
private: virtual void _abi_0c70() __asm__("__ZN23IO80211VirtualInterface30findOrCreateFlowQueueWithCacheE20IO80211FlowQueueHashPb") override; public:
private: virtual void _abi_0c78() __asm__("__ZN23IO80211VirtualInterface21findExistingFlowQueueE20IO80211FlowQueueHash") override; public:
private: virtual void _abi_0c80() __asm__("__ZN23IO80211VirtualInterface17removePacketQueueEPK20IO80211FlowQueueHash") override; public:
 virtual void flushPacketQueues() override; // 0xc80
 virtual void cachePeer(ether_addr*, unsigned int*) override; // 0xc88
 virtual bool shouldLog(unsigned long long) override; // 0xc90
 virtual void vlogDebug(unsigned long long, char const*, va_list) override; // 0xc98
 virtual void vlogDebugBPF(unsigned long long, char const*, va_list) override; // 0xca0
 virtual void * getP2PSkywalkPeerMgr() override; // 0xcb8
private: virtual void _abi_0cd0() __asm__("__ZN23IO80211VirtualInterface8findPeerER10ether_addr") override; public:
 virtual int getEventPipeSize() override; // 0xdb8
 virtual UInt64 createEventPipe(IO80211APIUserClient*) override; // 0xdc0
 virtual bool init(IO80211Controller *,ether_addr *,unsigned int,const char *); // 0xe60
 virtual bool setLinkState(IO80211LinkState,unsigned int); // 0xe68
private: virtual void _native_e70() __asm__("__ZN23IO80211VirtualInterface13forwardPacketEP20IO80211NetworkPacket"); public:
private: virtual void _native_e78() __asm__("__ZN23IO80211VirtualInterface11handleIoctlEP7__ifnetmPv"); public:
private: virtual void _native_e80() __asm__("__ZN23IO80211VirtualInterface14addPeerToCacheEPh"); public:
private: virtual void _native_e88() __asm__("__ZN23IO80211VirtualInterface19deletePeerFromCacheEPh"); public:
private: virtual void _native_e90() __asm__("__ZN23IO80211VirtualInterface14handleDebugCmdEP24apple80211_debug_command"); public:
private: virtual void _native_e98() __asm__("__ZN23IO80211VirtualInterface20getPeerSlotDataStatsEP10ether_addrbP17peerSlotDataStats"); public:
private: virtual void _native_ea0() __asm__("__ZN23IO80211VirtualInterface16getPeerDataStatsEP10ether_addrbP16packetCounters_t"); public:
private: virtual void _native_ea8() __asm__("__ZN23IO80211VirtualInterface18clearPeerDataStatsEP10ether_addrb"); public:
private: virtual void _native_eb0() __asm__("__ZN23IO80211VirtualInterface34setAwdlCurrentChannelSequenceIndexEj"); public:
private: virtual void _native_eb8() __asm__("__ZN23IO80211VirtualInterface22isOutputFlowControlledEv"); public:
private: virtual void _native_ec0() __asm__("__ZN23IO80211VirtualInterface23setOutputFlowControlledEv"); public:
private: virtual void _native_ec8() __asm__("__ZN23IO80211VirtualInterface25clearOutputFlowControlledEv"); public:
private: virtual void _native_ed0() __asm__("__ZN23IO80211VirtualInterface18configureAQMOutputEv"); public:
private: virtual void _native_ed8() __asm__("__ZN23IO80211VirtualInterface11attachToBpfEv"); public:
private: virtual void _native_ee0() __asm__("__ZN23IO80211VirtualInterface14configureIfnetEv"); public:
private: virtual void _native_ee8() __asm__("__ZN23IO80211VirtualInterface5resetEv"); public:
private: virtual void _native_ef0() __asm__("__ZN23IO80211VirtualInterface25dupAndTransmitMcastPacketEP20IO80211NetworkPacketPhS2_"); public:
private: virtual void _native_ef8() __asm__("__ZN23IO80211VirtualInterface19freeMulticastPacketEP20IO80211NetworkPacket"); public:
private: virtual void _native_f00() __asm__("__ZN23IO80211VirtualInterface22triggerAWDLMulticastTxEjh"); public:
private: virtual void _native_f08() __asm__("__ZN23IO80211VirtualInterface14p2pMulticastTxEPP20IO80211NetworkPacketj"); public:
private: virtual void _native_f10() __asm__("__ZN23IO80211VirtualInterface25sendPendingPacketsToStackEPP20IO80211NetworkPacketj"); public:
private: virtual void _native_f18() __asm__("__ZN23IO80211VirtualInterface18freePendingPacketsEPP20IO80211NetworkPacketj"); public:
private: virtual void _native_f20() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface0Ev"); public:
private: virtual void _native_f28() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface1Ev"); public:
private: virtual void _native_f30() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface2Ev"); public:
private: virtual void _native_f38() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface3Ev"); public:
private: virtual void _native_f40() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface4Ev"); public:
private: virtual void _native_f48() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface5Ev"); public:
private: virtual void _native_f50() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface6Ev"); public:
private: virtual void _native_f58() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface7Ev"); public:
private: virtual void _native_f60() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface8Ev"); public:
private: virtual void _native_f68() __asm__("__ZN23IO80211VirtualInterface33_RESERVEDIO80211VirtualInterface9Ev"); public:
private: virtual void _native_f70() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface10Ev"); public:
private: virtual void _native_f78() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface11Ev"); public:
private: virtual void _native_f80() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface12Ev"); public:
private: virtual void _native_f88() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface13Ev"); public:
private: virtual void _native_f90() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface14Ev"); public:
private: virtual void _native_f98() __asm__("__ZN23IO80211VirtualInterface34_RESERVEDIO80211VirtualInterface15Ev"); public:
 int dequeueTxPackets(TxPacketRequest *);
private: uint8_t _nativeLayout[0x120-sizeof(IO80211SkywalkInterface)];
};
static_assert(sizeof(IO80211VirtualInterface)==0x120,"24H32 virtual interface ABI");
struct apple80211_awdl_af_sec_payload_request;
struct apple80211_awdl_af_tx_mode;
struct apple80211_awdl_bssid;
struct apple80211_awdl_cap;
struct apple80211_awdl_cca;
struct apple80211_awdl_config_request;
struct apple80211_awdl_dev_stats;
struct apple80211_awdl_device_capabilities;
struct apple80211_awdl_dfsp_config;
struct apple80211_awdl_dfsp_ucsa_config;
struct apple80211_awdl_election_id;
struct apple80211_awdl_election_metric;
struct apple80211_awdl_election_params;
struct apple80211_awdl_election_rssi_thresholds;
struct apple80211_awdl_extension_state_machine_parameter;
struct apple80211_awdl_master_channel;
struct apple80211_awdl_min_rate;
struct apple80211_awdl_oob_request;
struct apple80211_awdl_peer_traffic_registration;
struct apple80211_awdl_presence_mode;
struct apple80211_awdl_psf_dwell_params;
struct apple80211_awdl_secondary_master_channel;
struct apple80211_awdl_sync_channel_sequence;
struct apple80211_awdl_sync_enabled;
struct apple80211_awdl_sync_frame_template;
struct apple80211_awdl_sync_params;
struct apple80211_awdl_sync_state;
struct apple80211_awdl_txcal_period;
struct apple80211_bssid_data;
struct apple80211_channel_data;
struct apple80211_companion_skywalk_if_create;
struct apple80211_encrypt;
struct apple80211_key;
struct apple80211_llw_params;
struct apple80211_p2p_coex_protection;
struct apple80211_peer_cache_control;
struct apple80211_peer_cache_maximum_size;
struct apple80211_rtg_peer_stats_cfg;
struct apple80211_ssid_data;
struct apple80211_state_data;
struct apple80211_tx_rate;
struct awdl_peer_advert_add;
class IO80211AWDLProtocol : public IO80211VirtualInterface {
 OSDeclareAbstractStructors(IO80211AWDLProtocol)
public:
 virtual IOReturn getSSID(apple80211_ssid_data *) = 0; // 0xfa0
 virtual IOReturn getCHANNEL(apple80211_channel_data *) = 0; // 0xfa8
 virtual IOReturn getBSSID(apple80211_bssid_data *) = 0; // 0xfb0
 virtual IOReturn getSTATE(apple80211_state_data *) = 0; // 0xfb8
 virtual IOReturn getAWDL_SYNC_PARAMS(apple80211_awdl_sync_params *) = 0; // 0xfc0
 virtual IOReturn getAWDL_SYNC_ENABLED(apple80211_awdl_sync_enabled *) = 0; // 0xfc8
 virtual IOReturn getAWDL_EXTENSION_STATE_MACHINE_PARAMETERS(apple80211_awdl_extension_state_machine_parameter *) = 0; // 0xfd0
 virtual IOReturn getAWDL_ELECTION_ID(apple80211_awdl_election_id *) = 0; // 0xfd8
 virtual IOReturn getAWDL_ELECTION_METRIC(apple80211_awdl_election_metric *) = 0; // 0xfe0
 virtual IOReturn getAWDL_SYNCHRONIZATION_CHANNEL_SEQUENCE(apple80211_awdl_sync_channel_sequence *) = 0; // 0xfe8
 virtual IOReturn getPEER_CACHE_MAXIMUM_SIZE(apple80211_peer_cache_maximum_size *) = 0; // 0xff0
 virtual IOReturn getAWDL_MASTER_CHANNEL(apple80211_awdl_master_channel *) = 0; // 0xff8
 virtual IOReturn getAWDL_SYNC_STATE(apple80211_awdl_sync_state *) = 0; // 0x1000
 virtual IOReturn getAWDL_ELECTION_RSSI_THRESHOLDS(apple80211_awdl_election_rssi_thresholds *) = 0; // 0x1008
 virtual IOReturn getAWDL_PRESENCE_MODE(apple80211_awdl_presence_mode *) = 0; // 0x1010
 virtual IOReturn getAWDL_ELECTION_PARAMS(apple80211_awdl_election_params *) = 0; // 0x1018
 virtual IOReturn getAWDL_DEVICE_CAPABILITIES(apple80211_awdl_device_capabilities *) = 0; // 0x1020
 virtual IOReturn getAWDL_SECONDARY_MASTER_CHANNEL(apple80211_awdl_secondary_master_channel *) = 0; // 0x1028
 virtual IOReturn getAWDL_AF_TX_MODE(apple80211_awdl_af_tx_mode *) = 0; // 0x1030
 virtual IOReturn getAWDL_MIN_RATE(apple80211_awdl_min_rate *) = 0; // 0x1038
 virtual IOReturn getAWDL_ENCRYPTION_TYPE(apple80211_encrypt *) = 0; // 0x1040
 virtual IOReturn getAWDL_TXCAL_PERIOD(apple80211_awdl_txcal_period *) = 0; // 0x1048
 virtual IOReturn getAWDL_DEV_STATS(apple80211_awdl_dev_stats *) = 0; // 0x1050
 virtual IOReturn getAWDL_DFSP_CONFIG(apple80211_awdl_dfsp_config *) = 0; // 0x1058
 virtual IOReturn getAWDL_CCA(apple80211_awdl_cca *) = 0; // 0x1060
 virtual IOReturn getAWDL_AF_SECONDARY_PAYLOAD_CAPABILITIES(apple80211_awdl_cap *) = 0; // 0x1068
 virtual IOReturn getLLW_PARAMS(apple80211_llw_params *) = 0; // 0x1070
 virtual IOReturn setSSID(apple80211_ssid_data *) = 0; // 0x1078
 virtual IOReturn setCHANNEL(apple80211_channel_data *) = 0; // 0x1080
 virtual IOReturn setAWDL_SYNC_PARAMS(apple80211_awdl_sync_params *) = 0; // 0x1088
 virtual IOReturn setAWDL_SYNC_ENABLED(apple80211_awdl_sync_enabled *) = 0; // 0x1090
 virtual IOReturn setAWDL_EXTENSION_STATE_MACHINE_PARAMETERS(apple80211_awdl_extension_state_machine_parameter *) = 0; // 0x1098
 virtual IOReturn setAWDL_ELECTION_ID(apple80211_awdl_election_id *) = 0; // 0x10a0
 virtual IOReturn setAWDL_BSSID(apple80211_awdl_bssid *) = 0; // 0x10a8
 virtual IOReturn setAWDL_ELECTION_METRIC(apple80211_awdl_election_metric *) = 0; // 0x10b0
 virtual IOReturn setAWDL_SYNCHRONIZATION_CHANNEL_SEQUENCE(apple80211_awdl_sync_channel_sequence *) = 0; // 0x10b8
 virtual IOReturn setAWDL_MASTER_CHANNEL(apple80211_awdl_master_channel *) = 0; // 0x10c0
 virtual IOReturn setAWDL_SYNC_STATE(apple80211_awdl_sync_state *) = 0; // 0x10c8
 virtual IOReturn setAWDL_ELECTION_RSSI_THRESHOLDS(apple80211_awdl_election_rssi_thresholds *) = 0; // 0x10d0
 virtual IOReturn setAWDL_PRESENCE_MODE(apple80211_awdl_presence_mode *) = 0; // 0x10d8
 virtual IOReturn setPEER_CACHE_CONTROL(apple80211_peer_cache_control *) = 0; // 0x10e0
 virtual IOReturn setAWDL_OOB_REQUEST(apple80211_awdl_oob_request *) = 0; // 0x10e8
 virtual IOReturn setAWDL_SYNC_FRAME_TEMPLATE(apple80211_awdl_sync_frame_template *) = 0; // 0x10f0
 virtual IOReturn setAWDL_ELECTION_PARAMS(apple80211_awdl_election_params *) = 0; // 0x10f8
 virtual IOReturn setAWDL_PEER_TRAFFIC_REGISTRATION(apple80211_awdl_peer_traffic_registration *) = 0; // 0x1100
 virtual IOReturn setAWDL_SECONDARY_MASTER_CHANNEL(apple80211_awdl_secondary_master_channel *) = 0; // 0x1108
 virtual IOReturn setAWDL_AF_TX_MODE(apple80211_awdl_af_tx_mode *) = 0; // 0x1110
 virtual IOReturn setAWDL_MIN_RATE(apple80211_awdl_min_rate *) = 0; // 0x1118
 virtual IOReturn setAWDL_ENCRYPTION_KEYS(apple80211_key *) = 0; // 0x1120
 virtual IOReturn setAWDL_ENCRYPTION_TYPE(apple80211_encrypt *) = 0; // 0x1128
 virtual IOReturn setAWDL_OOB_AUTO_REQUEST(apple80211_awdl_oob_request *) = 0; // 0x1130
 virtual IOReturn setAWDL_TXCAL_PERIOD(apple80211_awdl_txcal_period *) = 0; // 0x1138
 virtual IOReturn setAWDL_AWDL_ADVERTISERS(awdl_peer_advert_add *) = 0; // 0x1140
 virtual IOReturn setAWDL_DFSP_CONFIG(apple80211_awdl_dfsp_config *) = 0; // 0x1148
 virtual IOReturn setAWDL_DFSP_UCSA_CONFIG(apple80211_awdl_dfsp_ucsa_config *) = 0; // 0x1150
 virtual IOReturn setCOMPANION_SKYWALK_IF_CREATE(apple80211_companion_skywalk_if_create *) = 0; // 0x1158
 virtual IOReturn setAWDL_OOB_SEC_PAYLOAD_REQUEST(apple80211_awdl_af_sec_payload_request *) = 0; // 0x1160
 virtual IOReturn setLLW_PARAMS(apple80211_llw_params *) = 0; // 0x1168
 virtual IOReturn setTX_RATE(apple80211_tx_rate *) = 0; // 0x1170
 virtual IOReturn setP2P_COEX_PROTECTION(apple80211_p2p_coex_protection *) = 0; // 0x1178
 virtual IOReturn setPSF_DWELL(apple80211_awdl_psf_dwell_params *) = 0; // 0x1180
 virtual IOReturn setAWDL_CONFIG(apple80211_awdl_config_request *) = 0; // 0x1188
 virtual IOReturn setAWDL_RTG_PEER_STATS_CONFIG(apple80211_rtg_peer_stats_cfg *) = 0; // 0x1190
};
static_assert(sizeof(IO80211AWDLProtocol)==0x120,"24H32 AWDL protocol ABI");
