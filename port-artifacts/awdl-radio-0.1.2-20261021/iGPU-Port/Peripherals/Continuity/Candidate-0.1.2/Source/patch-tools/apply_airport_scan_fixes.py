import re
from apply_report_fixes import once

def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    assert s.count('else if (scan_ver == 14)')==2
    s=s.replace('else if (scan_ver == 14)', 'else if (scan_ver == 14 || scan_ver == 16)')
    s=once(s,'int ItlIwx::\niwx_umac_scan_size', '''static_assert(sizeof(struct iwx_scan_general_params_v10) == 36, "Shared scan general ABI");
static_assert(sizeof(struct iwx_scan_channel_params_v6) == 540, "Shared scan channel ABI");

int ItlIwx::
iwx_umac_scan_size''')
    # Linux v6.5 explicitly shares SCAN_REQUEST_* ABI across versions14â€“17.
    # The fork's v14 payload fields match that ABI; never relabel firmware version.
    marker='iwx_umac_scan(struct iwx_softc *sc, int bgscan)'
    start=s.index(marker);end=s.index('\nint ItlIwx::',start)
    b=s[start:end]
    b=once(b,'    uint8_t scan_ver = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_SCAN_REQ_UMAC);','''    uint8_t scan_ver = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_SCAN_REQ_UMAC);
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiScanVersion", scan_ver, 8);
            parent->setProperty("LatitudeWiFiScanLayout", scan_ver == 16 ? "api16-shared-layout14-17" : "fork-original");
            parent->setProperty("LatitudeWiFiRuntimeFlags", sc->sc_flags, 32);
            parent->setProperty("LatitudeWiFiNetworkState", ic->ic_state, 32);
        }
    }''')
    s=s[:start]+b+s[end:]
    # Persistent stages cover radio enable, per-command setup and scan submission.
    for fn in ['iwx_init_hw','iwx_config_umac_scan','iwx_config_legacy_umac_scan','iwx_umac_scan','iwx_umac_scan_v12','iwx_umac_scan_v14']:
        start=s.index(fn+'(struct iwx_softc *sc');begin=s.index('{',start);end=begin+1;depth=1
        while depth:
            if s[end]=='{':depth+=1
            elif s[end]=='}':depth-=1
            end+=1
        b=s[begin:end]
        b=b[:1]+'''
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) parent->setProperty("LatitudeWiFiRuntimeStage", "%s-entered");
    }
'''%fn+b[1:]
        def trace(m):
            call=m.group(2);label=fn+'/'+call.split('(',1)[0]
            return m.group(1)+'err = [&]() -> int {\n        int result = '+call[:-1]+''';
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiRuntimeStage", "%s");
            parent->setProperty("LatitudeWiFiRuntimeResult", (UInt32)result, 32);
            if (result) {
                parent->setProperty("LatitudeWiFiRuntimeFailure", "%s");
                parent->setProperty("LatitudeWiFiRuntimeFailureCode", (UInt32)result, 32);
            }
        }
    }
        return result;
    }();'''%(label,label)
        if fn=='iwx_umac_scan_v14':
            b=once(b,'    err = iwx_send_cmd(sc, &hcmd);','''    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiScanChannelCount", cp->count, 8);
            parent->setProperty("LatitudeWiFiScanRequestBytes", req_len, 32);
        }
    }
    err = iwx_send_cmd(sc, &hcmd);''')
        b=re.sub(r'(?m)^([ \t]*)err = ((?:iwx_\w+)\([\s\S]*?\);)',trace,b)
        s=s[:begin]+b+s[end:]
    # Track the runtime enable entry even if hardware setup fails before scanning.
    s=once(s,'\n    generation = ++sc->sc_generation;\n', '''
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiEnableCalled", true);
            parent->setProperty("LatitudeWiFiRuntimeFlags", sc->sc_flags, 32);
            parent->setProperty("LatitudeWiFiIfFlags", ifp->if_flags, 32);
            parent->setProperty("LatitudeWiFiBand24", sc->sc_nvm.sku_cap_band_24GHz_enable != 0);
            parent->setProperty("LatitudeWiFiBand5", sc->sc_nvm.sku_cap_band_52GHz_enable != 0);
        }
    }
    generation = ++sc->sc_generation;
''')
    p.write_text(s)
