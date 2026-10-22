# System brightness helper 0.2.0 (Sequoia)
The earlier helper physically adjusted linear brightness. This candidate uses DisplayServicesGetBrightness/SetBrightness for system state and calls the native Sequoia OSDManager brightness HUD. It derives the unique display ID from the Latitude panel registry vendor/product IDs. Missing APIs, ambiguous displays and failed writes are reported rather than treated as successful changes. No MMIO, keyboard content logging, TCC editing or graphics changes.

On the Dell, run without sudo:
`/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Brightness/install-brightness-keys.sh`

If Accessibility needs reapproval after the binary update, enable the installed latitude-brightness binary under Privacy & Security → Accessibility and rerun the installer. Test F6/F7, compare the HUD and Control Centre slider. Physical dimming worked in 0.1.0; system slider and OSD in 0.2.0 require this hardware test. Copilot is independent and requires a USB reboot.

The existing collect-desktop-failure.sh now records the installed helper's read-only diagnose JSON plus recent helper errors. Diagnose never changes brightness. The working 0.1.0 helper and previous collector are backed up under iGPU-Port/backups/input-system-sync-20261021.

References: https://github.com/nriley/brightness/blob/master/brightness.c and https://github.com/MonitorControl/MonitorControl/blob/main/MonitorControl/Support/OSDUtils.swift . These private APIs are for the current Sequoia installation; Tahoe needs separate validation.
