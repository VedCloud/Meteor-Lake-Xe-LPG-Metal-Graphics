"""Exercise the actual Intel RX case; substitute IOKit/DMA, never open radio."""
from pathlib import Path
import subprocess,json,hashlib
r=Path(__file__).resolve().parent.parent;out=r/'desktop-build/native-awdl-abi'
p=r/'desktop-build/airport-awdl-peer/itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
a=s.index('                // API83 notification v2');b=s.index('                break;\n            }',a)
body=s[a:b]
harness=r'''
#include "LatitudeAWDLPeerTransport.h"
#include <cassert>
#include <vector>
#include <cstdio>
struct iwx_session_prot_notif {uint32_t a,b,c,d;};
struct iwx_node {uint32_t in_id=0;};
struct Packet {const uint8_t*data;size_t length;};
struct IOService {template<class...T>void setProperty(T...){};IOService*getProvider(){return this;}};
struct Softc {struct{iwx_node*ic_bss;}sc_ic;int sc_flags=1;void*sc_dmat=nullptr;};
struct DMA {void*map=nullptr;};
static unsigned syncs=0;
static void bus_dmamap_sync(void*,void*,size_t,size_t length,int){assert(length==16);++syncs;}
static size_t iwx_rx_packet_payload_len(Packet*p){return p->length;}
static void clock_get_uptime(uint64_t*p){*p=100;}
static void absolutetime_to_nanoseconds(uint64_t a,uint64_t*b){*b=a;}
#define BUS_DMASYNC_POSTREAD 1
#define IWX_FLAG_TE_ACTIVE 1
#define XYLog(...) do{}while(0)
struct Driver {
 LatitudeAWDLPeer::Scheduler latitudeAWDLScheduler;IOService provider;
 IOService*getController(){return &provider;}
 void handle(Softc*sc,Packet*pkt,DMA*data){do {
'''
tests=r'''
 }while(false);}
};
int main(){
 Driver d;iwx_node bss;Softc sc;sc.sc_ic.ic_bss=&bss;DMA dma;
 auto event=[&](unsigned mac,unsigned configuration,unsigned status,unsigned start,size_t len=16){
  std::vector<uint8_t> bytes(len);uint32_t fields[]={mac,status,start,configuration};
  for(size_t i=0;i<len&&i<16;++i)bytes[i]=uint8_t(fields[i/4]>>(8*(i%4)));
  Packet p{bytes.data(),len};d.handle(&sc,&p,&dma);
 };
 for(size_t len=0;len<16;++len){sc.sc_flags=1;event(0,0,1,0,len);assert(sc.sc_flags==1);}
 assert(syncs==0);event(0,0,1,0,17);assert(syncs==0&&sc.sc_flags==1);
 event(1,2,0,0);assert(sc.sc_flags==1&&d.latitudeAWDLScheduler.foreign==1);
 event(1,3,1,0);assert(sc.sc_flags==1&&d.latitudeAWDLScheduler.foreign==2);
 event(1,0,1,0);assert(sc.sc_flags==1);
 event(0,1,0,0);assert(sc.sc_flags==1);
 event(0,0,2,0);assert(sc.sc_flags==1);
 event(0,0,1,2);assert(sc.sc_flags==1);
 event(0,0,1,1);assert(sc.sc_flags==1);
 event(0,0,1,0);assert(!sc.sc_flags);
 sc.sc_flags=1;event(0,0,0,1);assert(!sc.sc_flags);
 sc.sc_ic.ic_bss=nullptr;sc.sc_flags=1;event(UINT32_MAX,0,1,0);assert(sc.sc_flags==1);
 puts("PASS actual Intel RX case: valid primary semantics, foreign P2P isolation, bounded malformed payloads, DMA synchronization, missing BSS.");
}
'''
source=out/'notification.cpp';source.write_text(harness+body+tests);binary=out/'test-notification'
subprocess.run(['xcrun','clang++','-std=c++14','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(r/'wireless-sources'),str(source),'-o',str(binary)],check=True)
subprocess.run([str(binary)],check=True)
(out/'NOTIFICATION-VALIDATION.json').write_text(json.dumps({'actualIntelRxCase':'passed ASan/UBSan','sourceSHA256':hashlib.sha256(p.read_bytes()).hexdigest(),'hardwareIO':False,'associationIsolation':True},indent=2)+'\n')
