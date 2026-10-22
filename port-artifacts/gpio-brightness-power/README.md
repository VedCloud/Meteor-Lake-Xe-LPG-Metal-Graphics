# GPIO, brightness shortcuts and normal-performance power candidate

Dell Latitude5350, Sequoia15.8.1. This is a hardware-test candidate, not a verified fix.

Touchpad: Dell runtime SBFG reports GPIO387. Linux INTC1083 Meteor Lake-P table maps gpio_base384+3 to physical pad232/GPP_D_3, community5 (memory resource4). New LatitudeGPIO0.1.0 subclasses the installed VoodooGPIO base, supports only that pin and active-low level mode, maps bounded firmware resources, checks revision/PADBAR, pad ownership/lock/enable state, and uses Meteor Lake status0x204/enable0x214/host-own0x144. It alters only the target pin's bits, refuses another enabled owner and restores its pin configuration on stop. It never initializes every GPIO community. A filtered interrupt schedules work on a dedicated workloop. Current upstream TigerLakeLP is a reference, not a copied register layout.

DSDT changes only TPD0 _DSM/_CRS bodies and checksum, preserves package lengths, returns firmware SBFB+SBFG. Removes only the I2C3 force-polling property, retains keyboard, and disables competing PS2 pointer plugins for the trial. HID1.0.3 keeps the read/status/length fixes; this ELAN refuses silent polling fallback if IRQ registration fails. Touchscreen remains on its existing setup. No claim of interrupt delivery, gestures, sleep/wake or keyboard stability until Dell test.

Brightness: install a user-session shortcut helper through the existing IODisplay linear-brightness API. No raw MMIO access, second PWM writer, key-content logging or GPU changes. F6/F7, Fn+F6/F7 where delivered, or Control+Option+F6/F7 on an external keyboard step brightness; NX brightness events are recognized too. Events that fail to adjust are passed through. Needs Accessibility permission for the installed helper, then rerun installer. A frozen keyboard cannot generate shortcuts. Manual + and - commands need no Accessibility permission. No native login-window key support is claimed.

Power: NORMAL performance settings preserved. No Low Power Mode, CPU caps, altered sleep/hibernation policy, CPUFriend profile or forced frequency. ELAN IRQ avoids its high-frequency polling when functional; the untouched touchscreen still uses polling and remains a future power investigation. Collector now captures signed battery current/voltage/discharge watts, pmset settings/assertions, top CPU users, bounded CPU power sampler, GPIO/IRQ mode/counters and input failures. Unsupported readings are errors, not zero watts; external-power/charging samples do not assert battery discharge power. These are diagnostic snapshots, not a measured Windows-runtime comparison. Matching Windows requires matched workload/brightness/radio conditions and actual discharge tests.

Use on Dell:
1. Run the usual install-windowserver.sh only if an accelerated one-shot boot needs arming, and confirm exit0.
2. /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
3. Reboot through USB. Test typing/volume, pointer and gestures. Keep external input available.
4. /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Brightness/install-brightness-keys.sh
   Allow ~/Library/Application Support/LatitudeInput/latitude-brightness in Privacy & Security > Accessibility, then rerun step4. Without permission you may directly run that binary with + or -.
5. sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
Restore input with touchpad-trial.sh restore then USB reboot. The default USB remains the restored PS2 setup until apply. No power-settings command is needed or included. Remove shortcut helper using launchctl bootout gui/$(id -u)/com.ved.latitude-brightness and remove its named LaunchAgent if desired.

Validation: GPIO firmware/Linux mapping and bounds/owner/lock tests; actual driver ASan/UBSan mocked MMIO/start failures/restore; exact AML method-body execution; brightness65536 keycode/modifier and system-event mapping tests; HID65536 wire-length tests/negative control; collector fixture; revised actual switcher fixture; x86 builds/signatures, dependency symbols, OpenCore validator and frozen graphics baseline. Mocked tests cannot prove physical IRQ delivery or hardware power savings. No full GPU test suite rerun.

Sources:
- https://github.com/torvalds/linux/blob/master/drivers/pinctrl/intel/pinctrl-meteorlake.c
- https://github.com/VoodooI2C/VoodooGPIO
- https://github.com/JulioNectar/Hackintosh-XPS13Plus-IrisXe-Tahoe/blob/main/CHANGELOG.md (reports polling-related input freezes fixed with interrupts on Raptor Lake)
- https://github.com/AppleBreak1/Dell-Latitude-5300-2-in-1-Hackintosh (older working Dell VoodooI2C setup; not a Meteor Lake register map)
- https://github.com/MokkaSchnalle/macOS-Dell-5411-5511 (older Latitude interrupt/CPU-load experience)

Backup: iGPU-Port/backups/gpio-brightness-power-before; includes previous EFI/peripherals/collector. Graphics, Bluetooth, audio, security and host/target power settings unchanged.
