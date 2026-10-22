"""MA/API83 fragmented PNVM transport, following Linux v6.5's address table."""
def once(s,a,b):
    assert s.count(a)==1,(a[:100],s.count(a))
    return s.replace(a,b)

def apply(wifi):
    p=wifi/'itlwm/hal_iwx/ItlIwx.cpp';s=p.read_text()
    start=s.index('iwx_pnvm_handle_section(struct iwx_softc')
    end=s.index('\nint ItlIwx::',start)
    b=s[start:end]
    b=once(b,'    uint32_t size = 0;','''    uint32_t size = 0;
    uint32_t chunk_lengths[64] = {};
    uint32_t chunks = 0;
    uint32_t allocation_size = 0;
    bool fragmented = sc->sc_pcitag->configRead16(kIOPCIConfigDeviceID) == 0x7e40 &&
        isset(sc->sc_enabled_capa, 32); // IWL_UCODE_TLV_CAPA_FRAGMENTED_PNVM_IMG
    IOService *latitude_parent = getController() ? getController()->getProvider() : nullptr;''')
    b=once(b,'                u32 data_len = tlv_len - sizeof(*section);','''                if (tlv_len < sizeof(*section) || chunks >= 64) {
                    err = EINVAL;
                    goto out;
                }
                u32 data_len = tlv_len - sizeof(*section);
                if (data_len > 0xffffffffU - size) {
                    err = EINVAL;
                    goto out;
                }''')
    b=once(b,'                size += data_len;','''                size += data_len;
                chunk_lengths[chunks++] = data_len;''')
    b=once(b,'    err = iwx_dma_contig_alloc(sc->sc_dmat, &sc->pnvm_dram, size, 0);','''    allocation_size = size;
    if (fragmented) {
        allocation_size = 64 * sizeof(__le64);
        for (uint32_t j = 0; j < chunks; j++) {
            if (chunk_lengths[j] > 0xffffffc0U ||
                roundup(chunk_lengths[j], 64) > 0xffffffffU - allocation_size) {
                err = EINVAL;
                goto out;
            }
            allocation_size += roundup(chunk_lengths[j], 64);
        }
    }
    err = iwx_dma_contig_alloc(sc->sc_dmat, &sc->pnvm_dram, allocation_size, 0);''')
    b=once(b,'''    memcpy(sc->pnvm_dram.vaddr, pnvm_data, size);
    
    prph_sc_ctrl->pnvm_cfg.pnvm_base_addr =
        htole64(sc->pnvm_dram.paddr);
    prph_sc_ctrl->pnvm_cfg.pnvm_size =
        htole32(sc->pnvm_dram.size);''','''    if (fragmented) {
        // Each pointer addresses an original PNVM payload. One coherent DMA
        // allocation owns both the 64-entry descriptor table and all payloads.
        memset(sc->pnvm_dram.vaddr, 0, allocation_size);
        __le64 *addresses = (__le64 *)sc->pnvm_dram.vaddr;
        uint32_t offset = 64 * sizeof(__le64), source = 0;
        for (uint32_t j = 0; j < chunks; j++) {
            addresses[j] = htole64(sc->pnvm_dram.paddr + offset);
            memcpy((uint8_t *)sc->pnvm_dram.vaddr + offset, pnvm_data + source, chunk_lengths[j]);
            source += chunk_lengths[j];
            offset += roundup(chunk_lengths[j], 64);
        }
    } else {
        memcpy(sc->pnvm_dram.vaddr, pnvm_data, size);
    }
    prph_sc_ctrl->pnvm_cfg.pnvm_base_addr = htole64(sc->pnvm_dram.paddr);
    // Firmware expects the sum of payload bytes, excluding table and alignment.
    prph_sc_ctrl->pnvm_cfg.pnvm_size = htole32(size);
    OSSynchronizeIO();
    if (latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFiPNVMFragmented", fragmented);
        latitude_parent->setProperty("LatitudeWiFiPNVMChunks", chunks, 32);
        latitude_parent->setProperty("LatitudeWiFiPNVMPayloadBytes", size, 32);
        latitude_parent->setProperty("LatitudeWiFiPNVMDMALength", allocation_size, 32);
        latitude_parent->setProperty("LatitudeWiFiPNVMHardwareMatch", hw_match);
    }''')
    b=once(b,'out:\n    ::free(pnvm_data);','''out:
    if (latitude_parent) latitude_parent->setProperty("LatitudeWiFiPNVMSectionResult", (UInt32)err, 32);
    ::free(pnvm_data);''')
    s=s[:start]+b+s[end:]
    start=s.index('iwx_read_pnvm(struct iwx_softc')
    end=s.index('\nint ItlIwx::',start)
    b=s[start:end]
    b=once(b,'    int err = 0;', '    int err = ENOENT;')
    b=once(b,'''        if (tlv_type == IWX_UCODE_TLV_PNVM_SKU) {''','''        if (tlv_type == IWX_UCODE_TLV_PNVM_SKU) {
            if (tlv_len < sizeof(struct iwx_sku_id)) {
                err = EINVAL;
                goto out;
            }''')
    s=s[:start]+b+s[end:]
    start=s.index('iwx_load_pnvm(struct iwx_softc')
    end=s.index('\nstatic uint32_t',start)
    b=s[start:end]
    b=once(b,'''    if (!iwx_read_pnvm(sc)) {
        goto out;
    };
    
out:''','''    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) {
            parent->setProperty("LatitudeWiFiPNVMSKU0", sc->sku_id[0], 32);
            parent->setProperty("LatitudeWiFiPNVMSKU1", sc->sku_id[1], 32);
            parent->setProperty("LatitudeWiFiPNVMSKU2", sc->sku_id[2], 32);
        }
    }
    err = iwx_read_pnvm(sc);
    {
        IOService *parent = getController() ? getController()->getProvider() : nullptr;
        if (parent) parent->setProperty("LatitudeWiFiPNVMReadResult", (UInt32)err, 32);
    }
    if (err) {
        iwx_pnvm_free(&sc->sc_fw);
        return err;
    }''')
    b=once(b,'''        iwx_nic_unlock(sc);
    }''','''        iwx_nic_unlock(sc);
        {
            IOService *parent = getController() ? getController()->getProvider() : nullptr;
            if (parent) parent->setProperty("LatitudeWiFiPNVMDoorbellSubmitted", true);
        }
    } else {
        iwx_pnvm_free(&sc->sc_fw);
        return EBUSY;
    }''')
    s=s[:start]+b+s[end:]
    p.write_text(s)
