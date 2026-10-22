from apply_report_fixes import once


def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    for fn,kind in [('iwx_nic_error','LMAC'),('iwx_nic_umac_error','UMAC')]:
        start=s.index('\n'+fn+'(struct iwx_softc *sc)')
        end=s.index('\nvoid ItlIwx::',start+2) if '\nvoid ItlIwx::' in s[start+2:] else len(s)
        b=s[start:end]
        b=once(b,'    if (base < 0x400000) {', '''    // Linux v6.12 fw/dump.c validates the firmware-supplied LMAC pointer
    // for nonzero instead of the obsolete fixed 0x400000 lower limit.
    // Limit this compatibility change to the exact MA/API83 adapter; also
    // reject unaligned addresses and address+table-size overflow.
    bool modern_error_pointer = latitude_mld(sc);
    if ((!modern_error_pointer && base < 0x400000) || !base ||
        (base & 3) || base > UINT32_MAX - sizeof(table)) {''')
        b=once(b,'    if (iwx_read_mem(sc, base, &table, sizeof(table)/sizeof(uint32_t))) {', '''    IOService *latitude_parent=getController()?getController()->getProvider():nullptr;
    int latitude_read=iwx_read_mem(sc,base,&table,sizeof(table)/sizeof(uint32_t));
    if(latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFi%sErrorBase",base,32);
        latitude_parent->setProperty("LatitudeWiFi%sErrorReadResult",(UInt32)latitude_read,32);
    }
    if (latitude_read) {'''%(kind,kind))
        marker='    if (!table.valid) {' if kind=='LMAC' else '    if (ERROR_START_OFFSET <= table.valid * ERROR_ELEM_SIZE) {'
        data='''    if(latitude_parent && table.valid && table.valid!=UINT32_MAX &&
       table.error_id!=UINT32_MAX && !latitude_parent->getProperty("LatitudeWiFi%sFirstErrorID")) {
        latitude_parent->setProperty("LatitudeWiFi%sFirstErrorID",table.error_id,32);
        latitude_parent->setProperty("LatitudeWiFi%sFirstErrorData1",table.data1,32);
        latitude_parent->setProperty("LatitudeWiFi%sFirstErrorData2",table.data2,32);
        latitude_parent->setProperty("LatitudeWiFi%sFirstErrorData3",table.data3,32);
        latitude_parent->setProperty("LatitudeWiFi%sFirstErrorCommand",table.%s,32);
    }
'''%(kind,kind,kind,kind,kind,kind,'last_cmd_id' if kind=='LMAC' else 'cmd_header')
        b=once(b,marker,data+marker)
        s=s[:start]+b+s[end:]
    s=once(s,'            link_stage="activate-with-basic-rates";', '''            link_stage="activate-with-basic-rates";
            if(parent) {
                parent->setProperty("LatitudeWiFiActivationPayload",&link,sizeof(link));
                parent->setProperty("LatitudeWiFiActivationCCK",le32toh(link.cck_rates),32);
                parent->setProperty("LatitudeWiFiActivationOFDM",le32toh(link.ofdm_rates),32);
                parent->setProperty("LatitudeWiFiActivationLinkID",le32toh(link.link_id),32);
                parent->setProperty("LatitudeWiFiActivationMacID",le32toh(link.mac_id),32);
                parent->setProperty("LatitudeWiFiActivationPhyID",le32toh(link.phy_id),32);
            }''')
    p.write_text(s)
