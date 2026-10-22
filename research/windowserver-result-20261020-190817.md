# Desktop report 2026-10-20 19:08:17

Source: /Volumes/SEQOUIA/iGPU-Port/Desktop/reports/failure-detail-20261020-190817
Boot: 19:04:05; retained query 19:00:21..19:08:40 includes installation boot too. Review only; no package/EFI changes.

## Identity and outcome
WindowServer sample loaded Metal 0.3.17 UUID 9408E5B9-28AA-3906-BA56-BDF371E65068. Accelerator 0.2.7 and framebuffer 0.2.8 loaded. ACPI log confirms BRTKEYS revision 2, length 329, loaded this boot; USB SSDT SHA256 7d20b1f86478d5fcbb14d4e51629eb2e8fb6079deb2a915dcd85a3bad3c0b51c.
User reports subjectively faster desktop; brightness keys and Night Shift still ineffective.

## Brightness and colour
BrightnessKeys is active, but neither BrightnessPanel nor BrightnessKeyRouted appeared. Upstream sets these when a handled panel notification arrives. This does not show whether firmware failed to emit an event, the wrapper was bypassed, or notification targeting failed; do not assume another kext is missing.
CoreBrightness sets Night Shift factor=1 at 19:07:47, then factor=0 at 19:07:49. Framebuffer has advertised gamma capability but no setter-result properties. Gamma copies and refusals both zero. Thus colour output remains unverified/ineffective per user. AGDC gamma enabled and 9 capability answers. Display-pipe transactions accepted=2893, failed=0, empty=0. The previous theory of empty gamma-only transactions being silently acknowledged is not observed in this run; colour transaction contents are not captured.

## Performance and instrumentation
Eight publication lines now survive release logging. All staged=1, result=0, unlock=0; startup monitor wait .001..003 ms, IOSurface lock .013..026 ms, copy .862..1.881 ms. Preparation mostly 4.737..6.411 ms after initial 42.505 ms. These are early bounded observations, not whole-frame/input-to-scanout latency or measurements of later stalls. Logger per-site eight-record throttle still drops later publication records despite the call-site cap of64; slow phase details remain unavailable and need a bounded priority path, not unlimited logging.
Four CA timed-out-fence messages at19:07:12..13 in this boot. No GPU HANG/device-lost/wedged signatures in examined query. Counts from unequal windows cannot establish improvement over the previous three.
Later slow-part records include publication66.434 ms and submission/wait339.217 ms; the latter includes CPU preparation/serialization and is not isolated GPU time. WindowServer process CPUtime rises 2:12.01 to2:18.92 over~8sec (6.91 CPUsec, about86% of one core); front retry copies/late-refresh/skips stayzero. Those counters measure the retry observer, not all accepted primary copies.
Frame observer skipped due absent Screen Recording permission; no FPS or whole-frame measurement.
Glyph records show padded BGRA layouts (e.g44x24, pitch192); they do not establish a Weather font-atlas format/pitch fault. Weather sample captured; only WindowServer glyph records retained in examined logs.

Next work: trace physical key event entry before further routing edits; trace actual colour dispatch before interpreting successful AGDC capability answers as an applied LUT; retain bounded slow publication records beyond initial eight. Preserve successful renderer/audio/slider baseline.
