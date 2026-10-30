> **30 October 2026:** This directory retains the historical Sequoia package. For current Tahoe 0.3.21 payloads and corresponding sources, see [packages](../packages/). Do not install Sequoia binaries onto Tahoe.

# Candidate 0.3.18: display-transaction colour bridge and firmware-key counters

Target: Dell Latitude 5350 2-in-1, 8086:7d45, Sequoia15.8.1 build24H32. Source report failure-detail-20261020-190817. No target boot or hardware validation performed while preparing this candidate.

## Result and correction
The latest report loaded0.3.17, showed four CA fence timeout records, and confirmed active rendering. CoreBrightness set Night Shift factor1 then0; no framebuffer gamma setter state appeared. Firmware BRTKEYS revision2 loaded, but BrightnessKeys recorded no handled panel notification. User tried both brightness keys alone and Fn combinations, neither showing an indicator.

Apple's transaction-interface description distinguishes framebuffer gamma calls from colour data bound to display transactions. The recovery image's Sequoia15.6.1 IOAcceleratorFamily2 binary exports the corresponding transaction and GammaTable2 getters. The local bridge previously consumed only plane pixels/resources; it did not consume these colour getters. This is a code omission, although the exact Night Shift payload on24H32 still requires the target report.

Accelerator0.2.8 now reads the pipe/plane options, pre-/post-/plane gamma tables and colour matrix for the validated physical Latitude panel. It converts normalized float bit patterns into integer fixed-point data, composes LUTs and diagonal RGB gains/offsets into the existing256-entry output LUT, and sends that table to framebuffer0.2.9. The existing bounded CPU scanout path applies the table, with its identity fast path retained. Table objects are retained for a bounded three-object cache, unchanged transactions skip conversion, and pipe retirement releases the cache. Bypass flags reset to neutral. No GPU, Mesa, shader, PWM, mode or link changes.

Supported scope is SDR8-bit output, normalized LUTs and diagonal CSC. Non-diagonal CSC, invalid coefficients/tables, or unknown gamma-getter code are explicitly reported without applying a guessed transform. A zero/unbound matrix uses identity. This is a Night Shift candidate, not confirmed full colour-management support. Full matrix transforms and colour-profile fidelity are not claimed. Existing framebuffer gamma reporting is8-bit LUT output; this does not add HDR support.

The stock gamma-entry getter uses legacy MOVSS/XMM0. Before calling it, the candidate checks its complete47-byte relocation-free implementation against the inspected Sequoia reference. A mismatch refuses table sampling. The tiny getter runs inside a saved interrupt/legacy FP-state section with CR0.TS preservation; there is no allocation, wait or arbitrary method call inside that section. Fixed-point conversion runs outside it. The privileged section is compiled and statically inspected, not hardware-tested on the host. New family imports resolve during the guarded AuxKC preflight; failed preflight preserves installed components.

Framebuffer registry evidence: LatitudeColourDispatch, LatitudeColourOptions/PlaneOptions, table-presence flags, LatitudeGammaSource, LatitudeGammaActive/Generation, and existing gamma-copy/refusal counters. These distinguish request dispatch, actual LUT state and pixel-copy activity.

## Brightness keys
No additional remap is guessed. The previous native LCD0x86/0x87 route remains. A single definition-only DSDT rename _Q66->XQ66 preserves the complete firmware query body. The SSDT wraps it and records query entry, SMEE entry/flags, and BRT6 entry/argument. Its LDBG method returns only those five software counters. It neither reads EC operation regions nor injects keys during collection. Non-Darwin fallback and a single original event-flag read remain.

The USB collector's root-only latitude-key-state helper asks the validated framebuffer to snapshot LDBG from the ACPI LCD object. Only diagnostic registry metadata changes. No brightness setting, NVRAM, key injection or typed-key capture occurs. Snapshot status is retained even if firmware counters cannot be found. The framebuffer adds an explicit IOACPIFamily1.4 dependency, already loaded in the target report. These counters locate a missing query/SMEE/BRT6 stage; keyboard operation is not yet fixed or confirmed. Linux's Dell WMI driver also handles separate brightness events, so absence of the EC query must be established before selecting another event source.

## Remaining stalls and collector
Publication and glyph logs are bounded at their call sites (64/32), but the general eight-record site throttle still hid later slow records in0.3.17. Metal0.3.18 preserves all records within those existing caps. Ordinary per-draw logging stays throttled. This corrects observability; it does not claim to eliminate the four timeouts or the339ms submission/wait outlier. The scheduling, fencing and fresh command-pool workaround remain unchanged. Whole-frame/input latency is still unmeasured if Screen Recording permission is absent.

## Local validation
x86_64 framebuffer/accelerator/Metal/helper compilation and signatures; colour math and actual bridge fixtures under ASan/UBSan (identity, warm gains,2..4096-entry interpolation, cache lifetime, bypass/reset, invalid-data and unsupported-matrix refusal); reference getter fingerprint; actual gamma setter/front timer; root reporting/counter endpoint; actual AML under ACPICA with Darwin and fallback paths; guarded installer/AuxKC rollback fixtures; release logging and collector fixtures. No full hardware rendering suite rerun.

## Target check
Use the fallback desktop to run the existing install-windowserver.sh. After exit0, reboot through this USB. Press brightness keys alone and with Fn; enable Night Shift, move warmth to its warm end and leave it enabled. Use Weather and move windows normally, then run collect-desktop-failure.sh before rebooting. No colour/stress benchmark is required.

Previous EFI and replaced files are saved in backups/colour-transaction-before-0.3.18 (USB and local). The only EFI changes are DSDT's single query-definition rename/checksum and the counter SSDT. Sound, restored touchpad, SMBIOS, branding, GPU1.0.15, ANV4GiB, security and one-shot fallback policy are preserved.

## Primary evidence
- https://patents.google.com/patent/WO2018231523A1/en — Apple transaction colour interface, engineering reference rather than an ABI guarantee.
- https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebuffer.cpp — separate framebuffer gamma callback.
- https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp — panel notification listener and handled-notification registry markers.
- https://raw.githubusercontent.com/torvalds/linux/master/drivers/platform/x86/dell/dell-wmi-base.c — Dell WMI brightness event handling.
- https://www.dell.com/support/manuals/en-kh/latitude-13-5350-2-in-1-laptop/latitude-5350-owners-manual/keyboard-shortcuts?guid=guid-703f5965-dd57-4650-8547-e31b9d6db280&lang=en-us — F6/F7 brightness and Fn-lock behavior.

Recovery image mounted read-only for inspection. Its15.6.1 binary is research evidence, not a replacement for any system component. No Apple binary is added to the candidate.
