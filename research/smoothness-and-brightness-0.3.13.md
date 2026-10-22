# Candidate 0.3.13: front-buffer copy/timing and native PWM brightness

Prepared for Dell Latitude 5350 2-in-1, Intel 8086:7d45 / Dell 1028:0cb6. This candidate is compiled and checked locally; speed, remaining CoreAnimation timeouts, Weather glyphs and brightness require Dell validation.

## Evidence and changes

Report failure-detail-20261020-141853 loaded Metal plugin 0.3.12. Its 91 recorded readbacks averaged 5.279604 ms, versus 5.018886 ms in the preceding startup capture. This is not evidence of a speedup or a whole-frame measurement. Four CoreAnimation fence timeout messages remain. Slow-part diagnostics include 134.443 ms publication and 422.213 ms inside submission/wait. The latter includes CPU submission preparation and cleanup; it is not isolated GPU execution time. No GPU HANG/device-lost signature was found in the examined log. Weather was sampled during startup and its visual result is unconfirmed. A wall-clock-adjustment warning makes the old log's stated end boundary unreliable.

NMXAccel 0.2.5 replaces scalar front-buffer copies with x86 REP MOVSB and SFENCE, including the primary presentation copy and the existing retry copy. It keeps the validated source/destination geometry, fresh command pools, successful-fence publication and the existing retry policy. The retry timer now advances from its previous deadline rather than adding copy time to each interval. Late slots are skipped without burst replay; these slots are not measured dropped rendered frames. Read-only counters record retry-copy cumulative/max duration and skipped timer slots. REP MOVSB does not use kernel SIMD/FPU state. Target CPU reports ERMS and FSRM; actual framebuffer performance remains unmeasured.

LatitudeMTLFB 0.2.6 adds an IODisplayParameterHandler through the native display-parameter path. LatitudePanelClaim 0.2.3 keeps its existing early one-shot PCI arbitration. No new forced pipe takeover, modesetting, link training or register lock is added. The framebuffer first adopts the already validated firmware scanout; brightness is enabled only for active Pipe 0, plane 0 and a plausible enabled PWM controller 0.

The Dell's Linux VBT version 253 selects panel 2, native DDI PWM controller 0, 8-bit brightness and minimum level 15/255. Its MMIO trace writes PWM frequency 0xC8254=192000, duty 0xC8258=11294 and enable 0xC8250=0x80000000. The handler reads the live period and duty rather than assuming those captured values remain current. Initialization performs no PWM writes. Slider requests map monotonically to the live duty range with the VBT minimum, never change PWM enable/frequency/polarity/routing, and refuse writes if control, period or active pipe/plane checks fail. Slider zero means minimum dimming, not panel power-off. Stop restores the initial duty only if the state and last-written duty still match. Actual slider visibility, panel dimming and function keys are not verified; function-key mapping is unchanged.

Metal plugin stays 0.3.12; its identity attachment versus sampled-swizzle correction is retained. GPU kernel 1.0.15, ANV 4 GiB backend, transport and USB EFI/touchpad remain unchanged. No speculative glyph/blending change is added without visual confirmation. The branding files remain the supplied ICNS; both target CoreTypes paths in the latest report match it and decode successfully. This proves installation, not that About This Mac's image cache/UI uses the files.

## Collection and verification

The collector resolves its default log end after live sampling, records explicit boundaries and clock warnings, adds backlight registry snapshots and a bounded ScreenCaptureKit observer. The observer records delivered desktop update intervals, continuous-update p50/p95/max and intervals over the 60 Hz budget plus 0.5 ms. It discards pixels without inspection or saving. It runs as the logged-in user and skips without prompting when screen-recording permission is absent. Capture overhead, app-limited updates and idle periods affect these results. It cannot measure render-start-to-scanout or input latency; total_render_latency_ms is explicitly null. CPU/copy counters bracket a separate eight-second interval which can differ from the capture interval because capture setup takes time.

Local validation: x86 kernel builds and ad-hoc signatures; ASan/UBSan copy tests with tight/padded full-HD buffers, guard pages and overflow rejection; deadline overflow/skipping tests; all 65,536 brightness levels plus invalid/changed state rejection; native and x86_64 timestamp/statistics observer fixtures; installer staging/final-path validation/rollback including accelerator mismatch; collector crash/Weather/branding/explicit-end/frame-observer fixtures. New accelerator imports are the documented Mach conversion APIs absolutetime_to_nanoseconds and nanoseconds_to_absolutetime, with the existing com.apple.kpi.mach dependency retained. The Dell installer must still resolve them and validate its AuxiliaryKC.

The prior 22-case / 26,000-frame evidence is preserved. No full hardware suite, real host screen capture, Dell installation, arming or reboot was performed. The installer backs up all three installed kexts and restores the previous accelerator/framebuffer/GPU/AuxKC on a transaction failure. The one-shot fallback policy remains.

## Dell run

From the fallback desktop, run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, reboot through the USB. Check the brightness slider gently, Weather text and window movement. Then collect while the accelerated desktop is active:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

Keep Weather open; move/resize its window when FRAME PACING is announced. Permission-related skipping is recorded and does not prevent other logs. No GPU checklist rerun is needed. If the one-shot boot fails, return through the fallback boot and run the same collector for retained failure/crash logs.

## Primary references

- [Linux PWM register definitions](https://github.com/torvalds/linux/blob/v6.14/drivers/gpu/drm/i915/display/intel_backlight_regs.h)
- [Linux native backlight selection and setup](https://github.com/torvalds/linux/blob/v6.14/drivers/gpu/drm/i915/display/intel_backlight.c)
- [Apple IODisplay parameter handler](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOKit/graphics/IODisplay.h)
- [Apple framebuffer backlight discovery](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebuffer.cpp)
- [Apple display timestamps](https://developer.apple.com/documentation/screencapturekit/scstreamframeinfo/displaytime)
- [Apple kernel time conversion APIs](https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/KernelProgramming/services/services.html)
