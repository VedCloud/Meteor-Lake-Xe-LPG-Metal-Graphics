from apply_report_fixes import once
from apply_airport_connection_fixes import body
import subprocess


def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    anchor='int ItlIwx::\niwx_sta_rx_agg('
    types='''// Linux v6.5 datapath.h RX_BAID_ALLOCATION_CONFIG v2.
static bool latitude_mld(struct iwx_softc *sc);
struct latitude_baid_v2 {
    uint32_t action, sta_mask;
    uint8_t tid, reserved[3];
    uint16_t ssn, window;
} __packed;
static_assert(sizeof(latitude_baid_v2)==16,"RX BAID v2 ABI");
static latitude_baid_v2 latitude_baid_command(bool start, uint8_t tid,
    uint16_t ssn, uint16_t window) {
    latitude_baid_v2 cmd={};
    // BAID actions are0/1/2, NOT the MAC context actions1/2/3.
    cmd.action=htole32(start?0:2);
    cmd.sta_mask=htole32(1U<<IWX_STATION_ID);
    if(start) { cmd.tid=tid; cmd.ssn=htole16(ssn); cmd.window=htole16(window); }
    else {
        // REMOVE v2 carries le32 TID at union offset4 (wire offset8).
        uint32_t remove_tid=htole32(tid);
        memcpy(&cmd.tid,&remove_tid,sizeof(remove_tid));
    }
    return cmd;
}
static int latitude_baid_response(uint32_t reply, uint32_t *legacy_status) {
    // A raw BAID response is not ADD_STA's status word. Validate it before
    // adapting to the existing local reorder setup's checked representation.
    if(reply>=IWX_MAX_BAID) return EIO;
    *legacy_status=IWX_ADD_STA_SUCCESS|IWX_ADD_STA_BAID_VALID_MASK|
        (reply<<IWX_ADD_STA_BAID_SHIFT);
    return 0;
}

'''
    s=once(s,anchor,types+anchor)
    a,e=body(s,'iwx_sta_rx_agg');b=s[a:e]
    b=once(b,'    memset(&cmd, 0, sizeof(cmd));','''    bool modern_baid=latitude_mld(sc);
    if(modern_baid) {
        if(tid>=IEEE80211_NUM_TID || (start &&
            (!winsize || winsize>IEEE80211_BA_MAX_WINSZ || ssn>=4096))) {
            if(start && tid<IEEE80211_NUM_TID) ieee80211_addba_req_refuse(ic,ni,tid);
            splx(s); return EINVAL;
        }
        bool existing=false;
        for(unsigned i=0;i<nitems(sc->sc_rxba_data);i++) {
            struct iwx_rxba_data *entry=&sc->sc_rxba_data[i];
            if(entry->baid!=IWX_RX_REORDER_DATA_INVALID_BAID &&
                entry->sta_id==IWX_STATION_ID && entry->tid==tid) existing=true;
        }
        if(start && existing) {
            ieee80211_addba_req_refuse(ic,ni,tid); splx(s); return EBUSY;
        }
        // A duplicate stop must not decrement another session's counter.
        if(!start && !existing) { splx(s); return 0; }
    }
    memset(&cmd, 0, sizeof(cmd));''')
    old='''    err = iwx_send_cmd_pdu_status(sc, IWX_ADD_STA, sizeof(cmd), &cmd,
                                  &status);'''
    b=once(b,old,'''    if(modern_baid) {
        IOService *parent=getController()?getController()->getProvider():nullptr;
        uint8_t version=iwx_lookup_cmd_ver(sc,IWX_DATA_PATH_GROUP,0x16);
        latitude_baid_v2 modern=latitude_baid_command(start!=0,tid,ssn,winsize);
        uint32_t reply=UINT32_MAX;
        err=version==2 ? iwx_send_cmd_pdu_status(sc,
            iwx_cmd_id(0x16,IWX_DATA_PATH_GROUP,0),sizeof(modern),&modern,&reply) : EOPNOTSUPP;
        if(!err && generation!=sc->sc_generation) err=ENXIO;
        if(!err && start) {
            err=latitude_baid_response(reply,&status);
            if(!err && sc->sc_rxba_data[reply].baid!=IWX_RX_REORDER_DATA_INVALID_BAID)
                err=EEXIST;
            if(err && generation==sc->sc_generation) {
                // Release only the just-requested station/TID; never publish
                // an invalid/occupied BAID into the host reorder table.
                latitude_baid_v2 rollback=latitude_baid_command(false,tid,0,0);
                uint32_t ignored=UINT32_MAX;
                int cleanup=iwx_send_cmd_pdu_status(sc,iwx_cmd_id(0x16,IWX_DATA_PATH_GROUP,0),
                    sizeof(rollback),&rollback,&ignored);
                if(parent) parent->setProperty("LatitudeWiFiRxBACleanupResult",(UInt32)cleanup,32);
            }
        }
        if(parent) {
            parent->setProperty("LatitudeWiFiRxBALayout","linux-baid-v2-test16");
            parent->setProperty("LatitudeWiFiRxBAVersion",version,8);
            parent->setProperty("LatitudeWiFiRxBAStart",start!=0);
            parent->setProperty("LatitudeWiFiRxBATID",tid,8);
            parent->setProperty("LatitudeWiFiRxBAReply",reply,32);
            parent->setProperty("LatitudeWiFiRxBAResult",(UInt32)err,32);
        }
    } else {
        err = iwx_send_cmd_pdu_status(sc, IWX_ADD_STA, sizeof(cmd), &cmd,&status);
    }''')
    # Existing BAID bounds/ownership checks, timers, reorder initialization,
    # accept/refuse and session counts run only after the checked response.
    s=s[:a]+b+s[e:]
    s=once(s,'            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP,0x17):',
        '''            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP,0x16):
            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP,0x17):''')
    p.write_text(s)


def validate(wifi,out):
    s=(wifi/'itlwm/hal_iwx/ItlIwx.cpp').read_text()
    helper=s[s.index('struct latitude_baid_v2 {'):s.index('int ItlIwx::\niwx_sta_rx_agg(')]
    test=out/'rx-baid-regression.cpp'
    test.write_text('''#include <cstdint>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <cerrno>
#define __packed __attribute__((packed))
#define htole32(x) uint32_t(x)
#define htole16(x) uint16_t(x)
enum { IWX_STATION_ID=0,IWX_MAX_BAID=32,IWX_ADD_STA_SUCCESS=1,
    IWX_ADD_STA_BAID_VALID_MASK=0x8000,IWX_ADD_STA_BAID_SHIFT=8 };
'''+helper+'''
int main() {
    const uint8_t add_wire[16]={0,0,0,0,1,0,0,0,5,0,0,0,0xbc,0x0a,64,0};
    auto add=latitude_baid_command(true,5,0xabc,64);
    assert(memcmp(&add,add_wire,16)==0);
    const uint8_t remove_wire[16]={2,0,0,0,1,0,0,0,15,0,0,0,0,0,0,0};
    auto remove=latitude_baid_command(false,15,0xffff,0xffff);
    assert(memcmp(&remove,remove_wire,16)==0);
    uint32_t status=0;
    assert(latitude_baid_response(0,&status)==0 && status==0x8001);
    assert(latitude_baid_response(31,&status)==0 && status==0x9f01);
    status=0xdeadbeef;
    assert(latitude_baid_response(32,&status)==EIO && status==0xdeadbeef);
    assert(latitude_baid_response(0xffffffff,&status)==EIO && status==0xdeadbeef);
    std::puts("RX BAID wire fixtures passed:16-byte ADD/REMOVEv2,raw BAID0/31,invalid32/negative rejected");
}
''')
    exe=out/'rx-baid-regression'
    subprocess.run(['clang++','-std=c++11','-Wall','-Wextra','-Werror',str(test),'-o',str(exe)],check=True)
    subprocess.run([str(exe.resolve())],check=True)
