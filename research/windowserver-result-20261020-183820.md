# Desktop result 2026-10-20 18:38:20

Source: USB Desktop/reports/failure-detail-20261020-183820. Collection boot 18:33:04; query also includes previous install boot. No driver or EFI changes during this review.

- Loaded GPU 1.0.15, accelerator 0.2.7 UUID DFF5DB6F-12C3-3A69-ACDC-7C511C2298FE, framebuffer 0.2.8 UUID 6D5205EC-5FCC-3EBD-AC98-24D05AAA6292.
- WindowServer sample proves Metal 0.3.15 UUID A7C28057-E4D8-302F-A87A-199C57C2CAC6 loaded. Installed SHA256 matches packaged 9d7f675e10c30f77ab4ca26cf8898297dcc29f70992eafe524b41d58626bc88e. candidate0.3.12 trace is a stale source string, not evidence of old binary loading.
- CoreAudio enumerates default Built-in Output/Internal Speakers and Built-in Input/Internal Microphone at 48 kHz. User confirms sound; enumeration alone does not validate recording.
- Backlight handler ready, period 192000, initial duty 48188, current duty 192000, value 65535. User confirms slider works.
- BrightnessKeys 1.0.4 and PS2Keyboard 2.3.8 loaded. USB DSDT and SSDT-LAT-BRT hashes captured. No delivered physical-key event evidence; driver presence does not validate routing.
- Night Shift support and toggles logged by corebrightnessd; BlueReductionEnabled transitions 1/0 and reduction factor calls exist. Final logged transition is disabled at 18:37:38. Framebuffer advertises gamma capability but has no LatitudeGammaActive/Generation/Channels/Count/Width setter telemetry. Gamma copies/refused both zero. No evidence the implemented gamma setter/output path executed. CoreDisplay reports AGDC Gamma successful, which does not prove panel LUT application.
- All front retry/late publication counters zero during 18:38:31..39; no publication0.3.15/glyph-layout0.3.15 lines. Rendering is present in readback logs and current WindowServer sample. Investigate actual surface/present path before claiming new staging/retry/gamma mechanisms exercised.
- Three CA timed-out-fence messages in collection boot (18:35:29..30); two older messages precede boot. No GPU HANG/device-lost/wedged signatures in examined query. Counts across different windows are not a controlled performance comparison.
- Frame observer skipped because Screen Recording permission absent; no measured whole-frame latency or FPS.

Next investigation: colour-management/AGDC-to-framebuffer gamma dispatch and actual presentation path; physical Dell firmware-key event routing. Do not infer a pitch, triple-buffering or hardware-hang root cause from these logs alone.

Follow-up source audit: release logging filtered publication0.3.15/glyph-layout0.3.15. Their absence in this report is an instrumentation defect, not evidence that those methods did not run. 0.3.17 retains bounded records and separates monitor wait. Night Shift and timeout root causes remain unverified.
