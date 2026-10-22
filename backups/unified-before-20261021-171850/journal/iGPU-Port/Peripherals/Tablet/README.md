# Latitude 5350 convertible candidate — current status

See LINUX-RESULTS-20261021.md and Candidate/STATUS.json for the new measured
orientation/switch results, completed ELAN capture and exact implementation limits.
The previous investigation below is retained as historical context. It predates
the 50-sample Linux capture; it no longer describes the available Linux evidence.

The new candidate adds a passive macOS tablet-event observer, tested orientation/
input/keyboard decision policy, decoded sensor descriptors, updated diagnostics,
and a manually testable on-screen keyboard. Auto-rotation/input suppression and
automatic keyboard popup still need the live macOS sensor/state bridge.

# Latitude 5350 convertible support — investigation, not enabled

Requested behaviour: automatic panel rotation, matching touchscreen/pen coordinates, disable only built-in keyboard/trackpad when folded, restore on unfolding. External keyboard/mouse and touchscreen remain available. User confirmed disabling both built-in devices in tablet mode and restoring them in laptop mode. Current EFI is unchanged by this package.

## Confirmed from this Dell's existing reports
- Linux PCI 00:12.0: Meteor Lake-P Integrated Sensor Hub 8086:7e45; driver intel_ish_ipc.
- Four sensor HID devices enumerate through hid-ishtp 8087:0ac2 and bind hid-sensor-hub. Their report descriptors and actual motion axes were not captured. This does not yet establish which are accelerometers, hinge or orientation sensors.
- Intel HID events and 5-button array are on INTC1077. Saved input/devices.txt contains no SW_TABLET_MODE-capable switch; the lid switch only reports SW_LID. Absence from this snapshot does not prove no tablet signal exists.
- Firmware _SB.PC00.LPCB.ECDV.VGBS reads EC via ECRB(0x39), bits 0:1, synthesizes bit 0x40 and gates it with IUCE. Linux intel-hid defines bit 6 as NOT in tablet mode, but firmware's enable state and actual transitions must be measured before disabling input.
- VGBS is a method with EC reads AND VBDS writes. The diagnostics deliberately do not execute it as an assumed harmless property query. Do not infer tablet state just from current VGBS/EC bit definitions or enable switches speculatively.

## Implementation dependencies
1. Identify the sensor HID descriptors, physical axis mapping, sample scale and mount matrix in laptop and both portrait positions. Validate a stable tablet switch independently from ordinary lid closure.
2. Expose the hub/firmware signals to macOS. YogaSMC's experimental generic Intel HID support is a research lead, not evidence of Meteor Lake ISH sensor transport support or a drop-in Dell solution. No matching third-party kext is installed here.
3. Add debounced orientation decisions, hysteresis around flat/ambiguous positions, rotation lock, and a coordinated display plus Wacom touch/pen transform. Preserve external monitor orientation. Our custom framebuffer must be tested for all four rotations; its working landscape path does not establish rotated scanout support.
4. Implement device-specific input suppression below userspace, releasing held keys/modifiers and contacts before suppression; never shut down the shared PS/2 controller or I2C bus. Restore on unfolding, wake or signal loss. Keyboard/trackpad must not be disabled on unverified data, especially after the earlier input freezes.

## Diagnostics already on USB

See REPORT-20261021.md for the reviewed laptop/tablet results. The corrected collector enumerates the actual ACPI plane and includes the compiled read-only latitude-tablet-state tool; ordinary desktop failure reports include it too. No automatic tablet feature is enabled yet.
macOS: `/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/collect-tablet-macos.sh laptop`, then repeat with `tablet` while folded. These are registry snapshots only; no need to reinstall graphics or rerun rendering tests. They may show no active sensor driver.

Linux (only if required after macOS snapshots): `sudo python3 <USB-mount>/iGPU-Port/Peripherals/Tablet/collect-tablet-linux.py laptop`; repeat `tablet`, `portrait-left`, `portrait-right`. Requires Python 3; no package installs, sysfs writes, HID feature writes, event grabbing or keyboard capture. IIO interfaces that do not exist are reported as absent; this collector does not enable missing sensors. Hold each position for five seconds. HID descriptor reads can require root. Existing Linux report is from kernel enumeration, not these motion samples.

Sources:
- https://github.com/torvalds/linux/blob/master/drivers/platform/x86/intel/hid.c — INTC1077 and VGBS bit semantics, tablet events.
- https://github.com/torvalds/linux/blob/master/drivers/hid/intel-ish-hid/ipc/pci-ish.c — Meteor Lake ISH PCI transport; sensor transport is distinct from the tablet switch.
- https://github.com/zhen-zen/YogaSMC — experimental Intel HID firmware support; assess exact hardware matching before reuse.
