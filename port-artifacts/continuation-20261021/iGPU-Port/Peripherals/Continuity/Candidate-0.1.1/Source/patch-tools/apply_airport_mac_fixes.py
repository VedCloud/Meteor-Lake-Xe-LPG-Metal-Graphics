from apply_report_fixes import once


def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    start=s.index('iwx_mac_ctxt_cmd(struct iwx_softc *sc')
    pos=s.index('    memset(&cmd, 0, sizeof(cmd));',start)
    s=s[:pos]+'''    // Exact MA/API83 compatibility stage. Linux v6.5 MAC_CONFIG v2.
    if (sc->sc_pcitag->configRead16(kIOPCIConfigDeviceID) == 0x7e40 &&
        strcmp(sc->sc_fwname, "iwlwifi-ma-b0-gf-a0-83.ucode") == 0 &&
        isset(sc->sc_enabled_capa, 110) &&
        iwx_lookup_cmd_ver(sc, IWX_MAC_CONF_GROUP, 0x08) == 2) {
        struct latitude_mac_config_v2 {
            uint32_t id_and_color, action, mac_type;
            uint8_t local_addr[6];
            uint16_t reserved_addr;
            uint32_t filter_flags;
            uint16_t he_support, he_ap_support;
            uint32_t eht_support, nic_not_ack_enabled;
            uint8_t is_assoc, esr_transition_timeout;
            uint16_t medium_sync_delay, assoc_id, reserved1, data_policy, reserved2;
            uint32_t ctwin;
        } __packed;
        static_assert(sizeof(latitude_mac_config_v2) == 52, "MAC_CONFIG v2 ABI");
        struct latitude_mac_config_v2 modern;
        memset(&modern, 0, sizeof(modern));
        modern.id_and_color = htole32(in->in_id);
        modern.action = htole32(action);
        modern.mac_type = htole32(ic->ic_opmode == IEEE80211_M_MONITOR ?
            IWX_FW_MAC_TYPE_LISTENER : IWX_FW_MAC_TYPE_BSS_STA);
        IEEE80211_ADDR_COPY(modern.local_addr, ic->ic_myaddr);
        if (action != IWX_FW_CTXT_ACTION_REMOVE) {
            modern.filter_flags = htole32(1U << 2); // multicast
            modern.is_assoc = assoc && ni->ni_associd;
            if (!modern.is_assoc) modern.filter_flags |= htole32(1U << 3); // beacon
            modern.assoc_id = htole16(ni->ni_associd & 0x3fff);
            if (ni->ni_flags & IEEE80211_NODE_HE) modern.he_support = htole16(1);
        }
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiMacLayout", "linux-MAC_CONFIG-v2-52byte");
            parent->setProperty("LatitudeWiFiMacAction", action, 32);
        }
        int result = iwx_send_cmd_pdu(sc, iwx_cmd_id(0x08, IWX_MAC_CONF_GROUP, 0),
                                      0, sizeof(modern), &modern);
        if (parent) {
            parent->setProperty("LatitudeWiFiMacResult", (UInt32)result, 32);
            if (action == IWX_FW_CTXT_ACTION_ADD)
                parent->setProperty("LatitudeWiFiMacAddResult", (UInt32)result, 32);
        }
        return result;
    }

'''+s[pos:]
    # MAC creation can be tested without knowingly sending the next incompatible
    # legacy binding command. This is an explicit staged port, not connectivity.
    start=s.index('iwx_binding_cmd(struct iwx_softc *sc')
    pos=s.index('    struct iwx_binding_cmd cmd;',start)
    s=s[:pos]+'''    if (sc->sc_pcitag->configRead16(kIOPCIConfigDeviceID) == 0x7e40 &&
        strcmp(sc->sc_fwname, "iwlwifi-ma-b0-gf-a0-83.ucode") == 0 &&
        isset(sc->sc_enabled_capa, 110)) {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) parent->setProperty("LatitudeWiFiPortBoundary", "MLD-link-station-queue-port-required");
        return EOPNOTSUPP;
    }
'''+s[pos:]
    s=once(s,'            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP, 0x08): {', '''            case IWX_WIDE_ID(IWX_MAC_CONF_GROUP, 0x08): {
                IOService *parent = getController() ? getController()->getProvider() : nullptr;
                if (parent) parent->setProperty("LatitudeWiFiMacAckReceived", true);
                break;
            }
            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP, 0x08): {''')
    p.write_text(s)
