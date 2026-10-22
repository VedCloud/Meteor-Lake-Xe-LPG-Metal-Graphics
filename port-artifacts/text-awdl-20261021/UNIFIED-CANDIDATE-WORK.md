# Unified Dell candidate — user scope retained, 21 October 2026

All requested work belongs to one candidate. Do not silently drop unfinished
items or mark the entire candidate complete from a single subsystem test.

- HIDD visibility: scoped OpenCore HIDD method-rename patch plus SSDT compiled and mock-AML
  interpreted; first hardware attempt failed: OpenCore replaced the patched OEM table with the unpatched supplied DSDT. V3 directly patches the supplied table and uses original recovery tables; Dell revalidation pending.
- Tablet state: live firmware reader/qualification kext 0.2.0 compiled and tested
  under ASan/UBSan with mocked firmware; requires actual HIDD enumeration and
  corroborated mode transitions. Device-local PS/2 gates are now compiled/linked/signed in separate staging, with held-state release, quarantine and stale-data restoration tests passing. V3 arms the compiled gates conditionally: only corroborated folded mode plus a fresh reader heartbeat can block input; missing/unknown/stale signals restore input. Target validation pending.
- Rotation: exact built-in/manual request+acknowledgement/rollback client compiled;
  HBM 1.0 negotiation, HID GUID enumeration and credit/reset state are implemented. A real x86_64 ISH PCI/IRQ/PIO/HID driver now compiles and its actual interrupt path passes firmware-emulation tests through live-property publication. Normal boots use read-only preflight; Dell sensor operation and coordinated
  active-contact transition. Working graphics binaries must remain preserved.
- On-screen keyboard: 0.2.0 session app compiled, foreground text-field check and
  live-state contract connected; new login installer prepared; target runtime
  and Accessibility permission still need Dell validation.
- FreeType: full integration remains the goal, not a standalone demo. Filtered
  LCD RGB/RGB_V/BGR/BGR_V renderer, bounded atlas and gamma-correct component
  blending built/tested; CoreText-shaped-run frontend implemented and sanitizer-tested. System
  app interception and component-alpha compositor integration are not complete. A process-local native CTLineDraw interposer now passes sanitized/release runtime tests through an ordinary caller, with clipping and context-lifetime validation. The native AppKit glyph route and deferred transparent CPU text-image composition also pass tests, including San Francisco variable fonts. System-app deployment and GPU-backed CA layers remain open; the attempted late dyld loader was rejected after source/runtime verification showed that API is a no-op. Immutable deferred glyph layers and late opaque-background composition pass sanitizer tests.
- Bluetooth/Continuity/full AirDrop/AWDL: source lineage and missing radio paths
  audited, diagnostic collector added; pinned OWL protocol engine now compiles for Intel and passes host sanitizer tests for sync windows, peer lifecycle and action frames; A new native callback -> gated current-channel IWX action-submission candidate now compiles/links/signs as a complete Intel kext. Actual function-body sanitizer tests use mocked radio/kernel APIs. It remains staged, with radio qualification, completion/RX, peer data and actual AirDrop incomplete.
- OpenCore theme: requested GoldenGateExt archived, exact active 1.0.9 DEBUG
  driver build identified and hash-matched; published theme and matching
  OpenCanopy/AudioDxe without changing OpenCore core loader.
- Auxiliary entries: HideAuxiliary=true; ResetNvramEntry is auxiliary and remains
  available through Spacebar.
- Chime: official audio resource downloaded; correct HDA path 00:1f.3 selected,
  conservative gain and existing OS mute policy retained. First UEFI log finds ALC295 and decodes the 44.1 kHz file, but SetupPlayback returns Unsupported. V3 uses 48 kHz MP3 and the observed speaker output mask 1; audible playback still needs target validation.

Preserve confirmed Copilot->Command, normal persistent accelerated boots,
working Wi-Fi/graphics/audio, frozen baseline manifests and historical backups.
Do not rerun the old graphics suite. Include new diagnostics in ordinary desktop
collection and record actual runtime identity separately from on-disk candidate.
