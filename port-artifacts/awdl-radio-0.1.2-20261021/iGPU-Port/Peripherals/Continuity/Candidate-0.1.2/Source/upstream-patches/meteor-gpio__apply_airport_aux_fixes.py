from apply_report_fixes import once

def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    start=s.index('iwx_add_aux_sta(struct iwx_softc *sc)');end=s.index('\nint ItlIwx::',start)
    b=s[start:end]
    b=once(b,'    uint8_t cmdver;', '''    uint8_t cmdver;
    int internal_trial = 0;
    bool internal_aux = false;
    IOService *latitude_parent = getController() ? getController()->getProvider() : nullptr;''')
    b=once(b,'''    if (cmdver != IWX_FW_CMD_VER_UNKNOWN && cmdver >= 12)
        return 0;''','''    // Explicit compatibility trial for the exact MA/API83 command table.
    // Unknown legacy command alone is not treated as proof of internal AUX support.
    internal_aux = PE_parse_boot_argn("itlwm_mtl_internal_aux", &internal_trial, sizeof(internal_trial)) &&
        internal_trial == 1 && sc->sc_pcitag->configRead16(kIOPCIConfigDeviceID) == 0x7e40 &&
        strcmp(sc->sc_fwname, "iwlwifi-ma-b0-gf-a0-83.ucode") == 0 &&
        cmdver == IWX_FW_CMD_VER_UNKNOWN &&
        iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_SCAN_REQ_UMAC) == 16 &&
        iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_SCAN_CFG_CMD) == 5;
    if (latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFiAuxCommandVersion", cmdver, 8);
        latitude_parent->setProperty("LatitudeWiFiAuxMode", internal_aux ? "ma-api83-internal-aux-trial" : "fork-original");
    }
    if (internal_aux || (cmdver != IWX_FW_CMD_VER_UNKNOWN && cmdver >= 12))
        return 0;''')
    b=once(b,'''    if (!err && (status & IWX_ADD_STA_STATUS_MASK) != IWX_ADD_STA_SUCCESS)
        return EIO;''','''    if (latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFiAuxAddResult", (UInt32)err, 32);
        latitude_parent->setProperty("LatitudeWiFiAuxAddStatus", status, 32);
    }
    if (err) return err;
    if ((status & IWX_ADD_STA_STATUS_MASK) != IWX_ADD_STA_SUCCESS) return EIO;''')
    b=once(b,'''    return iwx_enable_txq(sc, IWX_AUX_STA_ID, qid, IWX_MGMT_TID,
                          sc->txq[qid].ring_count);
    return 0;''','''    err = iwx_enable_txq(sc, IWX_AUX_STA_ID, qid, IWX_MGMT_TID, sc->txq[qid].ring_count);
    if (latitude_parent) latitude_parent->setProperty("LatitudeWiFiAuxQueueResult", (UInt32)err, 32);
    return err;''')
    s=s[:start]+b+s[end:]
    start=s.index('iwx_config_umac_scan(struct iwx_softc *sc)');end=s.index('\nint ItlIwx::',start)
    b=s[start:end]
    b=once(b,'''    cmdver = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_ADD_STA);
    if (cmdver == IWX_FW_CMD_VER_UNKNOWN || cmdver < 12)
        scan_config->bcast_sta_id = IWX_AUX_STA_ID;
    else if (cmdver == IWX_FW_CMD_VER_UNKNOWN || cmdver < 5)
        scan_config->bcast_sta_id = 0xFF;''','''    // SCAN_CFG API5 deprecates bcast_sta_id: leave that byte zero as reserved.
    // Earlier revisions still follow the station-version compatibility rules.
    cmdver = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_SCAN_CFG_CMD);
    if (cmdver == IWX_FW_CMD_VER_UNKNOWN || cmdver < 5) {
        uint8_t station_version = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_ADD_STA);
        scan_config->bcast_sta_id = station_version == IWX_FW_CMD_VER_UNKNOWN || station_version < 12 ? IWX_AUX_STA_ID : 0xff;
    }
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiScanConfigVersion", cmdver, 8);
            parent->setProperty("LatitudeWiFiScanBroadcastStation", scan_config->bcast_sta_id, 8);
        }
    }''')
    s=s[:start]+b+s[end:]
    p.write_text(s)
