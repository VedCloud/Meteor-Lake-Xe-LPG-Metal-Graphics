# Unified Dell candidate — user scope retained, 21 October 2026

All requested work belongs to one candidate. Do not silently drop unfinished
items or mark the entire candidate complete from a single subsystem test.

- HIDD visibility: scoped OpenCore HIDD method-rename patch plus SSDT compiled and mock-AML
  interpreted; published in the active USB EFI; Dell validation pending.
- Tablet state: live firmware reader/qualification kext 0.2.0 compiled and tested
  under ASan/UBSan with mocked firmware; requires actual HIDD enumeration and
  corroborated mode transitions. No keyboard/touchpad suppression enabled yet.
- Rotation: exact built-in/manual request+acknowledgement/rollback client compiled;
  automatic orientation still needs the macOS ISH transport and coordinated
  active-contact transition. Working graphics binaries must remain preserved.
- On-screen keyboard: 0.2.0 session app compiled, foreground text-field check and
  live-state contract connected; new login installer prepared; target runtime
  and Accessibility permission still need Dell validation.
- FreeType: full integration remains the goal, not a standalone demo. Filtered
  LCD RGB/RGB_V/BGR/BGR_V renderer, bounded atlas and gamma-correct component
  blending built/tested; CoreText-shaped-run frontend implemented and sanitizer-tested. System
  app interception and component-alpha compositor integration are not complete.
- Bluetooth/Continuity/full AirDrop/AWDL: source lineage and missing radio paths
  audited, diagnostic collector added; full AWDL data plane not yet implemented.
- OpenCore theme: requested GoldenGateExt archived, exact active 1.0.9 DEBUG
  driver build identified and hash-matched; published theme and matching
  OpenCanopy/AudioDxe without changing OpenCore core loader.
- Auxiliary entries: HideAuxiliary=true; ResetNvramEntry is auxiliary and remains
  available through Spacebar.
- Chime: official audio resource downloaded; correct HDA path 00:1f.3 selected,
  conservative gain and existing OS mute policy retained. Playback needs target
  validation; correct codec output routing may require the first UEFI log.

Preserve confirmed Copilot->Command, normal persistent accelerated boots,
working Wi-Fi/graphics/audio, frozen baseline manifests and historical backups.
Do not rerun the old graphics suite. Include new diagnostics in ordinary desktop
collection and record actual runtime identity separately from on-disk candidate.
