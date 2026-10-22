# Dell peripheral candidate — 21 October 2026

Bluetooth: exact Dell USB 8087:0033 match. IntelBluetoothFirmware and IntelBTPatcher 2.5.1 from lshbluesky release; BlueToolFixup 2.7.2 official release. Download SHA256 values match GitHub release asset digests. The reference README advertises 2.7.3, but the official available release is 2.7.2. Existing Lilu1.7.3 and USB map retained; physical port10 already internal255. Set the two Bluetooth NVRAM initial values via OpenCore Add/Delete. No host NVRAM writes. No IntelBluetoothInjector, Broadcom firmware, address-bypass debug option or Tahoe beta flag.

Brightness keys: LatitudeHotkeys0.1.0 handles only Dell WMI brightness events from the matching GUID on AMW0/D0. BRTKEYS revision5 supplies an ownership guard to prevent duplicate native routing. Refuses an already-owned WMI interface, releases ownership/enable state on stop/failure. Hardware event source remains unconfirmed: the last report had Q66=7, SMEE=0, BRT6=0. Host mocked-driver/AML tests pass; this is not a Dell hardware pass.

I2C touchpad is a SEPARATE opt-in trial. Dell ELAN04F3:328E and Meteor Lake8086:7e7b differ from the reference ELAN0A05/Raptor Lake51e8. Current Dell boot logs contain TPD0._DSM missing HIDG. Patch only the current DSDT's TPD0 _DSM/_CRS bodies using firmware TPDH/SBFB/SBFI, preserving all other methods and package lengths. Reuse the Dell-scoped controller subclass with verified D0 and Linux timings; polling only PCI15.3. Disable PS2Mouse/PS2Trackpad plugins while keeping PS2Keyboard and exactly one VoodooInput. Earlier ELAN attachment succeeded, but erratic motion was not diagnosed; competing pointer suppression is a testable hypothesis, not a proven fix. A USB mouse is recommended for this trial.

First reboot the Dell through this USB and check Bluetooth/brightness keys. No graphics installer or rendering suite is needed for these EFI additions. Then collect with the normal collect-desktop-failure.sh: it now captures Bluetooth services, USB enumeration, Intel firmware status, Dell WMI counters and I2C3/TPD0 registry state. Test F6/F7 with/without Fn before collection. Pair a Bluetooth device manually to establish real functionality.

To try ELAN gestures after the first check:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
Then reboot via USB. To restore PS2 pointer configuration:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh restore
Both operations refuse changed configs/DSDTs and preserve the new Bluetooth/brightness configuration. Scripts only modify the USB; no target system installs. Full pre-update EFI backup: iGPU-Port/backups/peripherals-before-20261021/EFI.

Sources:
- https://github.com/ductruongvu23/hackintosh-gigabyte-a16
- https://github.com/lshbluesky/IntelBluetoothFirmware/releases/tag/v2.5.1
- https://github.com/acidanthera/BrcmPatchRAM/releases/tag/2.7.2
- https://github.com/acidanthera/BrcmPatchRAM#bluetoolfixupkext
- https://github.com/torvalds/linux/blob/master/drivers/platform/x86/dell/dell-wmi-base.c
- https://github.com/VoodooI2C/VoodooI2C

Hardware support is pending the next Dell boot. No GPU binaries, Wi-Fi, audio, branding, security policy, XOSI or host boot configuration changed.
