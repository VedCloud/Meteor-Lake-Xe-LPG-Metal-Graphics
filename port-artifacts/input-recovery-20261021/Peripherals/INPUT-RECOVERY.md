# Input recovery candidate — 21 October 2026

Evidence: failure-detail-20261021-002150 and opencore-2026-10-20-231422.txt. OpenCore explicitly rejected LatitudeHotkeys0.1.0 injection as Invalid Parameter. Its binary had no _kmod_info/start/stop metadata. 0.1.1 supplies explicit matching module identity/version, kernel startup/shutdown and compiler markers, and OSBundleRequired Root. Build checks require defined metadata/entry-point symbols; signature verified. AML revision5 and WMI event routing unchanged. No claim of successful target injection until next boot.

ELAN attached but final read was kIOReturnTimeout0xe00002d6. The installed HID1.0.1 getInputReport ignored readI2C's result, parsed uninitialized/partial buffers, did not reject length1 before length-2 underflow, and handed the HID parser a buffer two bytes longer than its initialized payload. HID1.0.2 checks read status and allocations, validates both length bounds, preserves reset completion on successful length0, dispatches exactly length-2 initialized bytes, and never dispatches failed input. Repeated ELAN polling failures back off50ms then250ms; normal scheduling resumes on the next successful read. This fixes concrete parser defects, not a demonstrated root cause of the controller timeout or keyboard freeze. No speculative controller/MMIO/EC resets.

Tests: actual compiled report/poll functions under ASan/UBSan over all65536 wire lengths, failed reads/no false reset, allocation/short-write failures, exact payload/readiness/capacity, bounded backoff/recovery. Negative control reproduces previous length1 underflow. Existing framebuffer-transform null guard remains and its200000-case test passes. WMI mock tests pass. Actual trial switcher tested with apply/restore and changed-HID refusal. X86 kext builds/signatures pass. No hardware access during host tests.

Default USB retains restored PS2 pointer and original HID1.0.1. Only LatitudeHotkeys0.1.1 is replaced in active EFI. Revised touchpad trial includes HID1.0.2 and preserves keyboard/touchscreen support; gestures and built-in keyboard stability need a Dell boot. Apply with:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
Restore with the same command ending restore, then reboot via USB. Restore now restores the previous HID binary as well as config/DSDT/PS2 pointer plugins.

Use existing WindowServer installer only to arm an accelerated boot, then apply the touchpad trial before that reboot. No graphics binaries changed and no rendering suite needed. After testing keys/touchpad, run the normal collect-desktop-failure.sh. It now retains latest3 OpenCore logs and input kext info so an enabled entry cannot be mistaken for successful injection. HID registry counters expose failed/malformed/delivered reports. External-keyboard volume working supports an input-path problem; exact built-in keyboard freeze cause remains unproven.

Sources:
- Linux I2C-HID receive checks and payload stripping: https://github.com/torvalds/linux/blob/master/drivers/hid/i2c-hid/i2c-hid-core.c
- VoodooI2C HID code: https://github.com/VoodooI2C/VoodooI2CHID
- OpenCore prelinked kext implementation: https://github.com/acidanthera/OpenCorePkg/blob/master/Library/OcAppleKernelLib/PrelinkedKext.c
- Upstream similar symptom reports (not proof of this Dell's cause): https://github.com/VoodooI2C/VoodooI2C/issues/321 and https://github.com/VoodooI2C/VoodooI2C/issues/456

Backup: iGPU-Port/backups/input-recovery-before-20261021 (EFI, Peripherals, collector; not an installed OS snapshot). Bluetooth, GPU, audio, XOSI, NVRAM and security policy retained.
