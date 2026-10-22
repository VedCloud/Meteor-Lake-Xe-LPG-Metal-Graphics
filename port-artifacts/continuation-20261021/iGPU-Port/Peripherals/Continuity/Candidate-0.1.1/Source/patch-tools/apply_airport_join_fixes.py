import re
from apply_report_fixes import once


def apply(wifi):
    p=wifi/'AirportItlwm/WCL/WCL.cpp';s=p.read_text()
    start=s.index('IOReturn AirportItlwmSkywalkInterface::startWCLJoin(')
    end=s.index('\nvoid AirportItlwmSkywalkInterface::resumeWCLJoin',start)
    b=s[start:end]
    b=once(b,'    ieee80211com *ic = fHalService->get80211Controller();','''    ieee80211com *ic = fHalService->get80211Controller();
    auto latitude_join_trace = [&](const char *stage, IOReturn result) {
        setProperty("LatitudeJoinStage", stage);
        setProperty("LatitudeJoinResult", (UInt32)result, 32);
        setProperty("LatitudeJoinState", (UInt32)ic->ic_state, 32);
        setProperty("LatitudeJoinScanPending", scanPending);
        setProperty("LatitudeJoinAuthSupported", supported);
    };
    latitude_join_trace("request", kIOReturnSuccess);
    // Keep one supported request while a native scan is active. The existing
    // deferred request timer, abort and credential erasure paths still apply.
    if (supported && !joinPending && scanPending &&
        (ic->ic_if.if_flags & IFF_RUNNING) && !ic->ic_wcl_mac_reconfig) {
        deferredJoin = join;
        memcpy(joinBSSID, join.bssid, sizeof(joinBSSID));
        explicit_bzero(&join, sizeof(join));
        joinDeferred = joinPending = true;
        joinTimer->setTimeoutMS(30000);
        latitude_join_trace("deferred-until-scan-complete", kIOReturnSuccess);
        return kIOReturnSuccess;
    }''')
    b=once(b,'        return !supported ? kIOReturnUnsupported : kIOReturnBusy;', '''        IOReturn result = !supported ? kIOReturnUnsupported : kIOReturnBusy;
        latitude_join_trace("request-rejected", result);
        return result;''')
    b=once(b,'    ic->ic_des_esslen = join.ssidLength;', '''    latitude_join_trace("authentication-configured", kIOReturnSuccess);
    ic->ic_des_esslen = join.ssidLength;''')
    pos=b.rfind('    return kIOReturnSuccess;')
    b=b[:pos]+'    latitude_join_trace("join-submitted", kIOReturnSuccess);\n'+b[pos:]
    s=s[:start]+b+s[end:]
    s=once(s,'    instance->postMessage(this, AirportWCL::ScanDone, &status, sizeof(status), true);', '''    instance->postMessage(this, AirportWCL::ScanDone, &status, sizeof(status), true);
    if (joinDeferred && !fHalService->get80211Controller()->ic_wcl_mac_reconfig)
        resumeWCLJoin();''')
    s=once(s,'    if (!AirportWCL::decodeJoin(request, 0x2cc, join)) {', '''    setProperty("LatitudeJoinDecodeCalled", true);
    if (!AirportWCL::decodeJoin(request, 0x2cc, join)) {
        setProperty("LatitudeJoinDecodeSuccess", false);''')
    s=once(s,'    return startWCLJoin(join);', '''    setProperty("LatitudeJoinDecodeSuccess", true);
    return startWCLJoin(join);''')
    s=once(s,'    event.status = status;', '''    setProperty("LatitudeJoinCompletionStatus", status, 16);
    setProperty("LatitudeJoinCompletionReason", reason, 16);
    setProperty("LatitudeJoinCompletionDriverResult", (UInt32)driverStatus, 32);
    event.status = status;''')
    p.write_text(s)
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    for fn in ['iwx_auth','iwx_run','iwx_rs_init']:
        start=s.index(fn+'(struct iwx_softc *sc')
        begin=s.index('{',start);end=begin+1;depth=1
        while depth:
            if s[end]=='{':depth+=1
            elif s[end]=='}':depth-=1
            end+=1
        b=s[begin:end]
        def trace(m):
            call=m.group(1); label=fn+'/'+call.split('(',1)[0]
            return '''err = ([&]() -> int {
        int result = %s
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiAssociationStage", "%s");
            parent->setProperty("LatitudeWiFiAssociationResult", (UInt32)result, 32);
            if (result) {
                parent->setProperty("LatitudeWiFiAssociationFailure", "%s");
                parent->setProperty("LatitudeWiFiAssociationFailureCode", (UInt32)result, 32);
            }
        }
        return result;
    })();'''%(call,label,label)
        b=re.sub(r'err = ((?:iwx_\w+)\([\s\S]*?\);)',trace,b)
        s=s[:begin]+b+s[end:]
    p.write_text(s)
