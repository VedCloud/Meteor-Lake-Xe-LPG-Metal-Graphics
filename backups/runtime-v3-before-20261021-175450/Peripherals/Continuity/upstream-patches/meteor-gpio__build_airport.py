from pathlib import Path
import subprocess,shutil,re
from apply_airport_firmware import apply_wifi
from apply_pnvm_fixes import apply as apply_pnvm
from apply_report_fixes import once
from apply_airport_scan_fixes import apply as apply_scan
from apply_airport_aux_fixes import apply as apply_aux
from apply_airport_phy_fixes import apply as apply_phy
from apply_airport_join_fixes import apply as apply_join
from apply_airport_mac_fixes import apply as apply_mac
from apply_airport_connection_fixes import apply as apply_connection
from apply_airport_connection_fixes import validate_rate_encoding
from apply_airport_assert_fixes import apply as apply_assert
from apply_airport_baid_fixes import apply as apply_baid, validate as validate_baid

def patch(wifi):
    apply_wifi(wifi)
    apply_pnvm(wifi)
    p=wifi/'AirportItlwm/AirportItlwm.cpp';s=p.read_text()
    s=once(s,'    if (!fHalService->attach(pciNub)) {','''    provider->setProperty("LatitudeAirportStage", "hardware-attach");
    if (!fHalService->attach(pciNub)) {
        provider->setProperty("LatitudeAirportStage", "hardware-attach-failed");
        const char *firmware = fHalService->getDriverInfo()->getFirmwareName();
        if (firmware) provider->setProperty("LatitudeWiFiFirmware", firmware);''')
    s=once(s,'    if (!attachInterface((IONetworkInterface **)&fNetIf, true)) {','''    provider->setProperty("LatitudeAirportStage", "interface-attach");
    if (!attachInterface((IONetworkInterface **)&fNetIf, true)) {
        provider->setProperty("LatitudeAirportStage", "interface-attach-failed");''')
    p.write_text(s)

    p=wifi/'AirportItlwm/AirportItlwmV2.cpp';s=p.read_text()
    s=once(s,'    if (!fHalService->attach(pciNub)) {','''    provider->setProperty("LatitudeAirportStage", "hardware-attach");
    if (!fHalService->attach(pciNub)) {
        provider->setProperty("LatitudeAirportStage", "hardware-attach-failed");
        const char *firmware = fHalService->getDriverInfo()->getFirmwareName();
        if (firmware) provider->setProperty("LatitudeWiFiFirmware", firmware);''')
    start=s.index('bool AirportItlwm::start(IOService *provider)');end=s.index('\nvoid AirportItlwm::stop',start)
    b=s[start:end]
    b=once(b,'    if (!attachInterface(fNetIf, this)) {','''    provider->setProperty("LatitudeAirportStage", "native-interface-attach");
    if (!attachInterface(fNetIf, this)) {
        provider->setProperty("LatitudeAirportStage", "native-interface-attach-failed");''')
    b=once(b,'    return true;','''    provider->setProperty("LatitudeAirportStage", "start-success");
    return true;''')
    s=s[:start]+b+s[end:];p.write_text(s)
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    s=once(s,'    fwData = getFWDescByName(sc->sc_fwname);','''    fwData = getFWDescByName(sc->sc_fwname);
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiFirmwareLookup", sc->sc_fwname);
            parent->setProperty("LatitudeWiFiFirmwareFound", fwData != nullptr);
        }
    }''')
    s=once(s,'                XYLog("PNVM complete notification received with status 0x%0x\\n", le32toh(pnvm_ntf->status));','''                {
                    IOService *parent = getController() ? getController()->getProvider() : nullptr;
                    if (parent) {
                        parent->setProperty("LatitudeWiFiPNVMComplete", true);
                        parent->setProperty("LatitudeWiFiPNVMStatus", le32toh(pnvm_ntf->status), 32);
                        parent->setProperty("LatitudeWiFiPNVMStatusPolicy", "fork-upstream-notification-completion");
                    }
                }
                XYLog("PNVM complete notification received with status 0x%0x\\n", le32toh(pnvm_ntf->status));''')
    for fn in ['iwx_preinit','iwx_run_init_mvm_ucode','iwx_load_ucode_wait_alive','iwx_load_pnvm','iwx_load_firmware']:
        start=s.index(fn+'(struct iwx_softc *sc'); begin=s.index('{',start);end=begin+1;depth=1
        while depth:
            if s[end]=='{':depth+=1
            elif s[end]=='}':depth-=1
            end+=1
        b=s[begin:end]
        def trace(m):
            call=m.group(1);label=fn+'/'+call.split('(',1)[0]
            return m.group(0)+'''
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiInitStage", "%s");
            parent->setProperty("LatitudeWiFiInitResult", (UInt32)err, 32);
            parent->setProperty("LatitudeWiFiAliveReceived", sc->sc_uc.uc_intr != 0);
            parent->setProperty("LatitudeWiFiAliveValid", sc->sc_uc.uc_ok != 0);
            parent->setProperty("LatitudeWiFiInitCompleteFlags", sc->sc_init_complete, 32);
            if (err && !parent->getProperty("LatitudeWiFiFailureStage")) {
                parent->setProperty("LatitudeWiFiFailureStage", "%s");
                parent->setProperty("LatitudeWiFiFailureCode", (UInt32)err, 32);
            }
        }
    }'''%(label,label)
        b=re.sub(r'(?m)^    err = ((?:iwx_\w+)\([\s\S]*?\);)',trace,b)
        s=s[:begin]+b+s[end:]
    p.write_text(s)

if __name__=='__main__':
    r=Path.cwd();wifi=r/'.deps/airport';out=r/'ci-airport';out.mkdir(exist_ok=True)
    patch(wifi)
    apply_scan(wifi)
    apply_aux(wifi)
    apply_phy(wifi)
    apply_join(wifi)
    apply_mac(wifi)
    apply_connection(wifi)
    apply_assert(wifi)
    apply_baid(wifi)
    validate_rate_encoding(wifi,out)
    validate_baid(wifi,out)
    (out/'instrumented-ItlIwx.cpp').write_text((wifi/'itlwm/hal_iwx/ItlIwx.cpp').read_text())
    (out/'instrumented-AirportItlwm.cpp').write_text((wifi/'AirportItlwm/AirportItlwm.cpp').read_text())
    (out/'instrumented-AirportItlwmV2.cpp').write_text((wifi/'AirportItlwm/AirportItlwmV2.cpp').read_text())
    subprocess.run(['xcodebuild','-project','itlwm.xcodeproj','-scheme','AirportItlwm-Sequoia','-configuration','Debug','-jobs','3','ARCHS=x86_64','ONLY_ACTIVE_ARCH=NO','SYMROOT='+str(r/'airport-build/products'),'OBJROOT='+str(r/'airport-build/obj'),'CODE_SIGNING_ALLOWED=NO','CODE_SIGNING_REQUIRED=NO','build'],cwd=wifi,check=True)
    b=r/'airport-build/products/Debug/Sequoia/AirportItlwm.kext'
    assert b.exists()
    shutil.copytree(b,out/'AirportItlwm.kext')
    subprocess.run(['file',str(out/'AirportItlwm.kext/Contents/MacOS/AirportItlwm')],check=True)
