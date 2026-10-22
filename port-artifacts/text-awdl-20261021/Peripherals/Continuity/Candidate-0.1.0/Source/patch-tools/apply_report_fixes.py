"""Opt-in Latitude Wacom resource fallback and stepping-aware MA firmware trial."""
from pathlib import Path
import base64, hashlib, urllib.request

def once(text, old, new):
    assert text.count(old) == 1, (old[:100], text.count(old))
    return text.replace(old, new)

def apply(i2c, wifi):
    nub = i2c/'VoodooI2C/VoodooI2C/VoodooI2CDevice/VoodooI2CDeviceNub.cpp'
    s = nub.read_text()
    s = once(s, 'bool VoodooI2CDeviceNub::attach(IOService* provider, IOService* child) {', '''// This fallback is opt-in and limited to the captured Latitude Wacom device.
static bool latitudeWacomFallbackAllowed(IOACPIPlatformDevice *acpi, IOPCIDevice *pci) {
    if (!acpi || !pci) return false;
    OSObject *opt_in = pci->getProperty("LatitudeWacomResourceFallback");
    OSData *opt_data = OSDynamicCast(OSData, opt_in);
    OSNumber *opt_number = OSDynamicCast(OSNumber, opt_in);
    bool enabled = opt_in == kOSBooleanTrue ||
        (opt_data && opt_data->getLength() == 1 &&
            *static_cast<const UInt8 *>(opt_data->getBytesNoCopy()) == 1) ||
        (opt_number && opt_number->unsigned64BitValue() == 1);
    if (!enabled ||
        pci->configRead16(kIOPCIConfigVendorID) != 0x8086 ||
        pci->configRead16(kIOPCIConfigDeviceID) != 0x7e78) return false;
    OSData *name = OSDynamicCast(OSData, acpi->getProperty("name"));
    return name && name->getLength() == 9 &&
        memcmp(name->getBytesNoCopy(), "WACF3233", 9) == 0;
}

bool VoodooI2CDeviceNub::attach(IOService* provider, IOService* child) {''')
    s = once(s, '    params[0]->release();', '''    IOPCIDevice *latitude_pci = controller->nub->controller->physical_device.pci_device;
    if (ret != kIOReturnSuccess && latitudeWacomFallbackAllowed(acpi_device, latitude_pci) &&
        strcmp(uuid, I2C_DSM_HIDG) == 0) {
        OSSafeReleaseNULL(*result);
        if (index == DSM_SUPPORT_INDEX) {
            UInt8 functions = 0x03;
            *result = OSData::withBytes(&functions, sizeof(functions));
            ret = *result ? kIOReturnSuccess : kIOReturnNoMemory;
        } else if (index == HIDG_DESC_INDEX) {
            ret = acpi_device->evaluateObject("HID2", result);
            OSNumber *address = OSDynamicCast(OSNumber, *result);
            if (ret == kIOReturnSuccess && address && address->unsigned64BitValue() > 0 &&
                address->unsigned64BitValue() <= 0xffff) {
                acpi_device->setProperty("LatitudeWacomHIDRegister", address);
                acpi_device->setProperty("LatitudeWacomHIDFallback", true);
            } else {
                OSSafeReleaseNULL(*result);
                ret = kIOReturnNotFound;
            }
        }
    }
    params[0]->release();''')
    s = once(s, '    if (!crs_parser.found_i2c && !dsm_parser.found_i2c) {', '''    bool latitude_wacom_resources = false;
    IOPCIDevice *latitude_pci = controller->nub->controller->physical_device.pci_device;
    if (!crs_parser.found_i2c && !dsm_parser.found_i2c &&
        latitudeWacomFallbackAllowed(acpi_device, latitude_pci)) {
        OSObject *serial_object = nullptr, *gpio_object = nullptr;
        IOReturn serial_status = acpi_device->evaluateObject("SBFB", &serial_object);
        IOReturn gpio_status = acpi_device->evaluateObject("SBFG", &gpio_object);
        OSData *serial = OSDynamicCast(OSData, serial_object);
        OSData *gpio = OSDynamicCast(OSData, gpio_object);
        // Validate the complete descriptor layouts before passing to the upstream parser.
        // Values and resource paths come from this laptop's live report, not a donor EFI.
        if (serial_status == kIOReturnSuccess && gpio_status == kIOReturnSuccess &&
            serial && gpio && serial->getLength() == 35 && gpio->getLength() == 37) {
            const UInt8 *a = static_cast<const UInt8 *>(serial->getBytesNoCopy());
            const UInt8 *b = static_cast<const UInt8 *>(gpio->getBytesNoCopy());
            if (a[0] == 0x8e && a[1] == 0x1e && a[2] == 0 && a[5] == 1 &&
                a[16] == 0x0a && a[17] == 0 && a[33] == 0x79 &&
                memcmp(a + 18, "\\\\_SB.PC00.I2C0", 15) == 0 &&
                b[0] == 0x8c && b[1] == 0x20 && b[2] == 0 && b[4] == 0 &&
                b[14] == 23 && b[15] == 0 && b[17] == 25 && b[18] == 0 &&
                b[19] == 35 && b[20] == 0 && b[23] == 0x75 && b[24] == 1 &&
                memcmp(b + 25, "\\\\_SB.GPI0", 10) == 0 && b[35] == 0x79) {
                dsm_parser.parseACPIResources(a, 0, serial->getLength());
                dsm_parser.parseACPIResources(b, 0, gpio->getLength());
                latitude_wacom_resources = dsm_parser.found_i2c && dsm_parser.found_gpio_interrupts;
            }
        }
        acpi_device->setProperty("LatitudeWacomResourceFallback", latitude_wacom_resources);
        OSSafeReleaseNULL(serial_object);
        OSSafeReleaseNULL(gpio_object);
    }
    if (!crs_parser.found_i2c && !dsm_parser.found_i2c) {''')
    s = once(s, '    if (validateAPICInterrupt() == kIOReturnSuccess)',
        '    if (!latitude_wacom_resources && validateAPICInterrupt() == kIOReturnSuccess)')
    nub.write_text(s)

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
