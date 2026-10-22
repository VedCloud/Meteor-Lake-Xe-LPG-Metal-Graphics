"""Private opt-in current-channel action path; not full AWDL or a radio pass."""
from pathlib import Path
import shutil
root=Path(__file__).resolve().parent.parent;w=root/'desktop-build/airport-awdl'
def replace(file,old,new):
 p=w/file;s=p.read_text();assert s.count(old)==1,(file,old[:80]);p.write_text(s.replace(old,new))
shutil.copy2(root/'wireless-sources/LatitudeAWDLFrame.h',w/'include/LatitudeAWDLFrame.h')
replace('AirportItlwm/AirportItlwmV2.hpp','    virtual UInt32 outputPacket(mbuf_t, void * param) override;','''    virtual UInt32 outputPacket(mbuf_t, void * param) override;
    int bpfOutputPacket(OSObject *, UInt32, mbuf_t) override;
    static IOReturn latitudeAWDLRawGated(OSObject *,void *,void *,void *,void *);''')
replace('itlwm/hal_iwx/ItlIwx.hpp','    void releaseAll();','''    int latitudeAWDLCurrentChannel(const void *,size_t);
    void releaseAll();''')
p=w/'AirportItlwm/AirportItlwmV2.cpp';s=p.read_text();s+='''
// Experimental native Apple BPF callback -> Intel TX path. Normal boots keep
// superclass behavior. This is NOT peer data, scheduling or AirDrop support.
int AirportItlwm::bpfOutputPacket(OSObject *object,UInt32 dlt,mbuf_t m)
{
    int opt=0;
    if (!PE_parse_boot_argn("latitude-awdl-current-channel",&opt,sizeof(opt)) || opt!=1 || dlt!=127)
        return super::bpfOutputPacket(object,dlt,m);
    if (!m) return EINVAL;
    size_t length=mbuf_pkthdr_len(m);
    if(length<8||length>4096){mbuf_freem(m);return EINVAL;}
    void *copy=IOMalloc(length);
    if(!copy){mbuf_freem(m);return ENOMEM;}
    int error=mbuf_copydata(m,0,length,copy);mbuf_freem(m);
    if(!error)error=_fCommandGate?(int)_fCommandGate->runAction(latitudeAWDLRawGated,copy,(void*)length):ENXIO;
    IOFree(copy,length);return error;
}
IOReturn AirportItlwm::latitudeAWDLRawGated(OSObject *owner,void *bytes,void *length,void *,void *)
{
    auto self=OSDynamicCast(AirportItlwm,owner);if(!self)return EINVAL;
    auto hal=OSDynamicCast(ItlIwx,self->fHalService);
    int result=hal?hal->latitudeAWDLCurrentChannel(bytes,(size_t)length):ENOTSUP;
    self->setProperty("LatitudeAWDLRawLastSubmitErrno",(uint64_t)(uint32_t)result,32);
    self->setProperty("LatitudeAWDLRawMode","opt-in current-associated-channel action only; no peer-data path");
    return (IOReturn)result;
}
''';p.write_text(s)
p=w/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text();s='#include <LatitudeAWDLFrame.h>\n'+s;s+='''
// Called only while the existing controller command gate is held. No mode
// switch, channel change, second MAC, firmware station or queue is invented.
int ItlIwx::latitudeAWDLCurrentChannel(const void *bytes,size_t length)
{
    auto sc=&com;auto ic=&sc->sc_ic;auto ifp=&ic->ic_if;
    if(!bytes||sc->sc_pcitag->configRead16(kIOPCIConfigDeviceID)!=0x7e40 ||
       strcmp(sc->sc_fwname,"iwlwifi-ma-b0-gf-a0-83.ucode"))return ENOTSUP;
    if(!(ifp->if_flags&IFF_RUNNING)||!(ifp->if_flags&IFF_UP)||
       ic->ic_state!=IEEE80211_S_RUN||ic->ic_opmode!=IEEE80211_M_STA||
       sc->ns_nstate!=IEEE80211_S_RUN||!ic->ic_bss||!ic->ic_bss->ni_chan||
       (sc->sc_flags&(IWX_FLAG_SHUTDOWN|IWX_FLAG_TXFLUSH|IWX_FLAG_SCANNING|IWX_FLAG_BGSCAN))||
       (ic->ic_xflags&IEEE80211_F_TX_MGMT_ONLY))return EBUSY;
    auto channel=ic->ic_bss->ni_chan;
    if(channel==IEEE80211_CHAN_ANYC||(channel->ic_flags&(IEEE80211_CHAN_PASSIVE|IEEE80211_CHAN_DFS)))return EBUSY;
    LatitudeAWDL::Frame frame;
    if(!LatitudeAWDL::radiotap((const uint8_t*)bytes,length,channel->ic_freq,ic->ic_myaddr,frame))return EINVAL;
    int qid=sc->first_data_qid;
    if(qid<0||qid>=(int)nitems(sc->txq)||!sc->qfullmsk.empty())return ENOBUFS;
    auto ring=&sc->txq[qid];
    if(!ring->ring_count||!ring->desc||ring->queued>=ring->ring_count-1)return ENOBUFS;
    mbuf_t packet=nullptr;
    int err=mbuf_allocpacket(MBUF_DONTWAIT,frame.length,nullptr,&packet);
    if(err)return err;
    err=mbuf_copyback(packet,0,frame.length,(const uint8_t*)bytes+frame.offset,MBUF_DONTWAIT);
    if(err){mbuf_freem(packet);return err;}
    if(mbuf_len(packet)<40&&mbuf_pullup(&packet,40)){if(packet)mbuf_freem(packet);return ENOBUFS;}
    auto node=ieee80211_ref_node(ic->ic_bss);
    err=iwx_tx(sc,packet,node,EDCA_AC_BE); // takes packet ownership on all paths
    if(err)ieee80211_release_node(ic,node);
    // Zero means submission accepted; it does NOT mean RF transmission or ACK.
    return err;
}
''';p.write_text(s)
print('Private Sequoia bpfOutputPacket -> gated IWX current-channel action path prepared; default off, not installed.')
