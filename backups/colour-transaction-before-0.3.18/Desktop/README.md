# Candidate 0.3.17: native brightness notifications and retained publication evidence

## Latest target result
Report failure-detail-20261020-183820 loaded the correct 0.3.16 kernel components and Metal 0.3.15. Audio output and slider brightness are confirmed by the user. There were three CoreAnimation timed-out-fence records after the 18:33:04 boot; two earlier records belong to the preceding installation boot. Different report windows prevent a controlled performance comparison.

## Corrected defects
The release logger omitted publication0.3.15 and glyph-layout0.3.15 despite their bounded call sites. Their absence therefore did not establish that publication/layout code was bypassed. Metal 0.3.17 retains these bounded messages, adds surface-monitor wait time separately from preparation, IOSurface lock, copy and unlock time, and corrects the constructor candidate label. Existing log throttling remains enabled. Fresh command pools, fencing, completion scheduling, shared heap, rendering and shader logic are unchanged.

The Dell brightness wrapper now emits standard panel notifications (LCD 0x86 up / 0x87 down), consumed by the already-loaded BrightnessKeys listener, instead of synthetic PS/2 scan notifications. The firmware's LCD has the standard 0x0400 address and BrightnessKeys successfully registered on the previous boot. Darwin SMEE event routing and non-Darwin original methods remain intact. Physical key operation is a candidate correction requiring target confirmation; firmware event arrival is not proven.

## Night Shift remains unresolved
CoreBrightness received toggles, but there is no recorded framebuffer gamma setter state and zero gamma copies. The final logged toggle was off. The AGDC bridge advertises plane colour capabilities, while NVDisplayPipe::performTransactionCopy only handles plane IOSurface/resource payloads and acknowledges empty transactions without parsing colour options. This is a specific integration gap, not proof that any particular transaction carried the Night Shift transform. Advertising support or seeing AGDC Gamma successful does not prove colour is applied. Do not guess undocumented transaction offsets or enable hardware PWM/LUT writes to address this. No Night Shift fix is claimed in this candidate.

Publication stalls exceed 100 ms and one submission/wait phase reached about 250 ms. The newly retained per-phase diagnostics are needed to attribute those stalls before altering synchronization. This candidate does not claim to remove the remaining fence timeouts or Weather corruption. No full rendering suite needs rerunning; use ordinary desktop/Weather interaction and the existing collector.

## Collector
Retains publication/glyph records, gamma registry, gamma-copy counters, CoreBrightness/CoreAudio logs, audio devices and identities, native BrightnessKeys registry state, and read-only AGDC/display-pipe counters. BrightnessKeys adds BrightnessPanel and BrightnessKeyRouted after receiving a handled panel notification; the existing input snapshot captures those. Frame observation still skips without Screen Recording permission and does not measure input-to-scanout latency.

## Validation and scope
x86_64 release compilation; actual release logger under ASan/UBSan; actual surface publication and event completion fixtures; native ACPI route compilation/checksum and upstream listener checks; collector retention fixtures; guarded installer regression checks. Target installation/boot/key operation and remaining graphics fixes are unverified. USB EFI change is limited to SSDT-LAT-BRT.aml. Audio, restored touchpad, framebuffer/accelerator/GPU kernels and security policy are preserved. Backup contains previous EFI and replaced package files, not an OS/AuxKC snapshot.

Primary references:
- https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp
- https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebuffer.cpp
- https://github.com/ericek111/vibe-nullmoth-xe3-qemu (virtual display reference; not a native Meteor Lake panel solution)
