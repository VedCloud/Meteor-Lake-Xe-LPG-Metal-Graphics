from pathlib import Path
import base64,hashlib,urllib.request
from apply_report_fixes import once

def apply_wifi(wifi):
    fw = wifi/'itlwm/hal_iwx/ItlIwx.cpp'
    s = fw.read_text()
    s = once(s, '    sc->sc_fwname = sc->sc_cfg->fwname;', '''    sc->sc_fwname = sc->sc_cfg->fwname;
    {
    IOService *latitude_parent = getController() ? getController()->getProvider() : nullptr;
    if (latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFiHwRevision", sc->sc_hw_rev, 32);
        latitude_parent->setProperty("LatitudeWiFiRfId", sc->sc_hw_rf_id, 32);
        latitude_parent->setProperty("LatitudeWiFiMacStep", IWX_CSR_HW_REV_STEP(sc->sc_hw_rev), 32);
    }
    // The old table maps all MA steps to a nonexistent a0/API63 file.
    // Use an unmodified official b0/API83 payload only on verified b0 MA/GF hardware.
    if (device == 0x7e40 && CSR_HW_REV_TYPE(sc->sc_hw_rev) == IWL_CFG_MAC_TYPE_MA &&
        IWX_CSR_HW_REV_STEP(sc->sc_hw_rev) == IWX_SILICON_B_STEP &&
        sc->sc_cfg == &iwl_cfg_ma_a0_gf_a0) {
        sc->sc_fwname = "iwlwifi-ma-b0-gf-a0-83.ucode";
        if (latitude_parent) latitude_parent->setProperty("LatitudeWiFiFirmwareSelection", "ma-b0-api83-trial");
    } else if (device == 0x7e40 && latitude_parent) {
        latitude_parent->setProperty("LatitudeWiFiFirmwareSelection", "unsupported-stepping-or-rf-kept-original");
    }
    }''')
    # Save parser return before early startup logs roll out of Recovery's buffer.
    s = once(s, '    err = iwx_read_firmware(sc);', '''    err = iwx_read_firmware(sc);
    {
        IOService *latitude_fw_parent = getController() ? getController()->getProvider() : nullptr;
        if (latitude_fw_parent) latitude_fw_parent->setProperty("LatitudeWiFiFirmwareParseResult", (UInt32)err, 32);
    }''')
    s = once(s, '    fwData = getFWDescByName(pnvm_name);', '''    if (strcmp(sc->sc_fwname, "iwlwifi-ma-b0-gf-a0-83.ucode") == 0)
        snprintf(pnvm_name, sizeof(pnvm_name), "iwlwifi-ma-b0-gf-a0.pnvm");
    fwData = getFWDescByName(pnvm_name);
    {
        IOService *latitude_pnvm_parent = getController() ? getController()->getProvider() : nullptr;
        if (latitude_pnvm_parent) {
            latitude_pnvm_parent->setProperty("LatitudeWiFiPNVMLookup", pnvm_name);
            latitude_pnvm_parent->setProperty("LatitudeWiFiPNVMFound", fwData != nullptr);
        }
    }''')
    fw.write_text(s)
    base = 'https://kernel.googlesource.com/pub/scm/linux/kernel/git/firmware/linux-firmware.git/+/refs/tags/20251011/intel/iwlwifi/'
    for name, digest in [
        ('iwlwifi-ma-b0-gf-a0-83.ucode', '9386a8294457f527c6e0fa1eac440c50cbee801707fdb480a10889770c0d297b'),
        ('iwlwifi-ma-b0-gf-a0.pnvm', 'b21d3257a2a434ca4660353a7144c436eafaf8e60d0542e3159eaffee4de5629')]:
        data = base64.b64decode(urllib.request.urlopen(base + name + '?format=TEXT').read())
        assert hashlib.sha256(data).hexdigest() == digest
        (wifi/'itlwm/firmware'/name).write_bytes(data)
    generated = wifi/'include/FwBinary.cpp'
    if generated.exists(): generated.unlink()
