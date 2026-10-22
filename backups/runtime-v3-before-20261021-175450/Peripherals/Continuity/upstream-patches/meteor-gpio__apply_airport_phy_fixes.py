from apply_report_fixes import once


def apply(wifi):
    p = wifi / 'itlwm/hal_iwx/ItlIwx.cpp'
    s = p.read_text()
    s = once(s, 'if (iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_PHY_CONTEXT_CMD) == 3)',
             'if (iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_PHY_CONTEXT_CMD) == 3 ||\n        iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_PHY_CONTEXT_CMD) == 4)')
    start = s.index('iwx_phy_ctxt_cmd_v3(struct iwx_softc *sc')
    end = s.index('\nint ItlIwx::', start)
    b = s[start:end]
    b = once(b, '    struct iwx_phy_context_cmd cmd;', '''    struct iwx_phy_context_cmd cmd;
    static_assert(sizeof(struct iwx_phy_context_cmd) == 32, "PHY context v3/v4 ABI");
    struct latitude_rlc_config_v2 {
        uint32_t phy_id;
        uint32_t rx_chain_info;
        uint32_t rlc_reserved;
        uint32_t chain_a_sad_mode;
        uint32_t chain_b_sad_mode;
        uint32_t mac_id;
        uint32_t sad_reserved;
        uint8_t flags;
        uint8_t reserved[3];
    } __packed;
    static_assert(sizeof(struct latitude_rlc_config_v2) == 32, "RLC v2 ABI");
    uint8_t phy_ver = iwx_lookup_cmd_ver(sc, IWX_LONG_GROUP, IWX_PHY_CONTEXT_CMD);
    uint8_t rlc_ver = iwx_lookup_cmd_ver(sc, IWX_DATA_PATH_GROUP, 0x08);
    IOService *parent = getController() ? getController()->getProvider() : nullptr;''')
    b = once(b, '    return iwx_send_cmd_pdu(sc, IWX_PHY_CONTEXT_CMD, 0, sizeof(cmd), &cmd);', '''    // Linux v6.5: PHY_CONTEXT v3/v4 share the 32-byte layout. Version 4
    // reserves rxchain_info; RLC_CONFIG v2 carries receive-chain settings.
    uint32_t rx_chain = cmd.rxchain_info;
    if (phy_ver == 4 || rlc_ver == 2) cmd.rxchain_info = 0;
    if (parent) {
        parent->setProperty("LatitudeWiFiPhyVersion", phy_ver, 8);
        parent->setProperty("LatitudeWiFiPhyLayout", "linux-v3-v4-32byte");
        parent->setProperty("LatitudeWiFiRLCVersion", rlc_ver, 8);
        parent->setProperty("LatitudeWiFiPhyRxChainField", le32toh(cmd.rxchain_info), 32);
    }
    int err = iwx_send_cmd_pdu(sc, IWX_PHY_CONTEXT_CMD, 0, sizeof(cmd), &cmd);
    if (parent) parent->setProperty("LatitudeWiFiPhyResult", (UInt32)err, 32);
    if (err || action == IWX_FW_CTXT_ACTION_REMOVE || rlc_ver != 2) return err;
    struct latitude_rlc_config_v2 rlc;
    memset(&rlc, 0, sizeof(rlc));
    rlc.phy_id = htole32(ctxt->id);
    rlc.rx_chain_info = rx_chain;
    err = iwx_send_cmd_pdu(sc, iwx_cmd_id(0x08, IWX_DATA_PATH_GROUP, 2), 0, sizeof(rlc), &rlc);
    if (parent) parent->setProperty("LatitudeWiFiRLCResult", (UInt32)err, 32);
    return err;''')
    s = s[:start] + b + s[end:]
    s = once(s, '''            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP, IWX_STA_HE_CTXT_CMD):''', '''            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP, 0x08): {
                // RLC_CONFIG v2 returns an ACK. Without this dispatch case,
                // handled=0 suppresses iwx_cmd_done and the waiter times out.
                IOService *parent = getController() ? getController()->getProvider() : nullptr;
                if (parent) {
                    parent->setProperty("LatitudeWiFiRLCAckReceived", true);
                    parent->setProperty("LatitudeWiFiRLCAckQueue", qid, 8);
                    parent->setProperty("LatitudeWiFiRLCAckIndex", idx, 8);
                    parent->setProperty("LatitudeWiFiRLCAckPayloadLength", iwx_rx_packet_payload_len(pkt), 32);
                }
                break; // Existing queue/index/pending checks complete the ACK below.
            }
            case IWX_WIDE_ID(IWX_DATA_PATH_GROUP, IWX_STA_HE_CTXT_CMD):''')
    p.write_text(s)
