# Candidate 0.3.14: real panel size and About This Mac icon refresh

Includes all 0.3.13 copy/timing and guarded brightness changes. NMXAccel remains 0.2.5, Metal plugin remains 0.3.12 and the GPU kernel remains 1.0.15. USB EFI is unchanged.

LatitudeMTLFB 0.2.7 now supplies the exact 128-byte BOE panel EDID recorded on this Dell under Linux, instead of leaving EDID unavailable on the physical-panel path. Its base block checksum is valid; its native mode is 1920x1080. The detailed timing descriptor reports a 294x165 mm active image, giving a diagonal of 13.274 inches, rounded to 13.3 inches. Missing EDID and physical dimensions previously left macOS to estimate the display size, consistent with the reported 30.5-inch value. The old driver provided zero physical image dimensions. This change is actual captured panel metadata, not a fabricated size/EDID or a GPU memory override. It is specific to this laptop's captured panel, not every Latitude 5350 panel option. Replacing the physical panel requires recapturing its EDID.

The framebuffer validates the EDID header, length, checksum, extension count, native resolution and physical dimensions before supplying it. It also populates mode imageWidth/imageHeight in millimeters. The existing fPanelTiming path takes precedence, preserving the firmware's validated live timing, resolution, refresh and scanout. DDC requests receive the captured data; this adds no live AUX transactions, modesetting or PWM changes. Existing 0.2.5, 0.2.6 and the new 0.2.7 framebuffer identities are accepted for the guarded installer transition.

The supplied custom ICNS bytes remain unchanged. The user confirms that it appears under General > About > Displays. Both active CoreTypes model icon files already matched the asset in the last report, so repeatedly rewriting those files does not address a stale IconServices lookup. The branding wrapper now runs a separate refresh even when its existing-install check succeeds. It preserves only the fixed system IconServices store and the invoking user's resolved IconServices cache directories, then recreates existing directories with their original ownership and permissions. It rejects symlinks/files and does not search or delete other caches. Absent paths are skipped. Backups and a per-revision audit marker live under the original LatitudeBranding backup. A reboot is required to discard in-memory icon references; no applications or system services are force-quit. This is a cache-refresh candidate, not proof of About This Mac's next visual result. The collector records its refresh audit status.

The existing Dell model name/year label is retained. Physical Built-in Display dimensions should now round to 13.3 inches. No SMBIOS/security/NVRAM reset or touchpad changes are added.

Local checks: x86 framebuffer compile/signature; actual EDID parser under ASan/UBSan including the 13.3-inch calculation and malformed/truncated/mismatched input rejection; universal native helper compilation with warnings as errors; native and x86_64 fixtures for cache preservation, permission retention, missing paths and symlink rejection, plus existing preference/APFS/icon-resolution fixtures; installer/rollback checks; package/frozen baseline/EFI hashes. No hardware suite is rerun. Slider, smoothness, Weather, displayed size and About This Mac icon still require target observation.

From the fallback Dell desktop, use the usual installer:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, reboot through USB, then open Apple menu > About This Mac and General > About > Displays. Check the icon and panel size. Collect using:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

The icon-cache backups do not replace the installer's installed-kext/AuxKC backup. The USB publication backup preserves the preceding candidate package and EFI; no Dell OS installation is performed from the build host.

References: [Apple mode physical width is in millimeters](https://developer.apple.com/documentation/iokit/iodisplaymodeinformation/1506007-imagewidth), [Apple framebuffer mode-information API](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOKit/graphics/IOFramebuffer.h). The exact EDID is from the user's Linux hardware report, not an online sample. The previously supplied branding guide is treated as reference material, not permission to change security settings.

---

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

---

# Candidate 0.3.12 — texture-view correctness, bulk copy and stall attribution

Latest target result: report135231 confirms plugin0.3.11 and optimized=1. Across eight aggregates/79 readbacks, recorded stages total5.018886ms (prep0.289468, record0.050861, wait3.168114, post-cache/copy1.510380, cleanup0.000063). The prior report recorded22.428036ms across56readbacks. These are different startup captures, not a controlled frame-rate benchmark. Four CoreAnimation fence timeout messages cluster at13:51:19–20 during login check-in. No corresponding GPU HANG/device-lost signature and no new dated crash/panic report were found. Weather was not running during the collection; its glyph problem remains unlocalized.

## Changes

1. Explicitly swizzled Metal texture views now retain their identity attachment/storage view and store the swizzle in the separate sampled view. Previously this method replaced `_ownView` with the nonidentity view, contrary to the separation already used by descriptor-created textures. Vulkan framebuffer attachments require identity swizzles (VUID-VkFramebufferCreateInfo-pAttachments-00884). Failure to create a requested swizzle now returns nil rather than silently substituting an identity-mapped view. This is a concrete resource-view correction; whether Weather exercises the faulty path needs target evidence. No claim that its glyph problem is fixed yet.
2. Tightly packed readbacks use one `memcpy` for the complete validated byte range instead of one call per scanline (1080 calls at full HD). Padded layouts retain row copies and untouched padding. Both CPU cache-maintenance boundaries, Vulkan dependencies and successful-fence requirements are unchanged. This aims to reduce CPU copy cost; no target speedup below5ms is established. The3.17ms wait stage remains a substantial part of the total.
3. Bounded diagnostics: first eight Weather fragment-texture bindings record pixel format, dimensions, swizzle and alias/pitch data; slow submission parts record time before the part, GPU wait and surface publication when elapsed since commit is at least50ms (maximum32 reports/process); callbacks over50ms record up to16 reports. An unsupported cross-process shared-event-handle request is explicitly recorded if called. None of these diagnostics signal a fence early, skip work, lengthen timeouts, or establish that Metal shared events caused the CoreAnimation timeouts.
4. Collector adds a25-second-bounded read-only branding diagnostic: active root snapshot identity, previous snapshot identity, current CoreTypes model/inherited icon paths and hashes, equality to supplied ICNS, ImageIO frame decoding/dimensions, backup/volume match and current user model preferences. It does not infer icon-cache correctness from files, modify the snapshot or clear caches. The supplied ICNS itself extracts successfully, includes square16–1024px variants, and the largest decoded image was visually checked. It is unchanged.

Fresh command pools remain mandatory. Kernel1.0.15, framebuffer0.2.5, accelerator0.2.4, ANV4GiB backend, transport and restored touchpad EFI remain unchanged. 0.3.11 and the EFI are backed up before publication. The About This Mac name remains Dell Latitude5350 2-in-1 (13-inch,2024); existing successful branding is reused. Its missing image is not declared fixed before the new checks run.

## Validation and limits

Three plugin variants compile. Actual swizzled-view method fixture passes under ASan/UBSan with mock Vulkan view creation: identity/storage preserved, separate sampled view, replacement cleanup and nil on failure. Actual readback fixture covers tight/padded bytes, dependencies, cache boundaries, failed submission leaving the destination untouched, quarantined leases and cleanup. Event-publication, installer/rollback/one-shot, loader, release policy/logging and collector fixtures pass, including failed branding diagnostics that do not discard graphics evidence. Native branding helper builds with warnings as errors; inherited-icon/APFS/preference fixtures pass natively and under x86_64 Rosetta. Plugin signature/import/export checks pass.

These checks cannot reproduce Weather/CoreAnimation or benchmark a Dell GPU on the Apple Silicon build host. The candidate does not yet establish a fix for the four timeout messages. No full hardware rendering checklist was rerun, no Dell installation/arming/reboot was performed from the build host, and no new kernel/cache-coherency or guessed glyph-blend patches were added.

## Dell trial

From the fallback desktop:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0, reboot through the USB. Open Weather and leave its affected text visible, open About This Mac, and move/resize a window for about30seconds. Then collect using the same command:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

This records the visible desktop state without running the previous hardware suite. Cosmetic failure exit4 explicitly means graphics are installed/armed but branding needs separate attention. Retain its report rather than retrying blindly.

## Primary sources consulted

- [Khronos framebuffer attachment identity requirement](https://docs.vulkan.org/refpages/latest/refpages/source/VkFramebufferCreateInfo.html), VUID00884.
- [Apple Metal scheduling and handler semantics](https://developer.apple.com/documentation/metal/mtlcommandbuffer/waituntilscheduled()). Existing publication-before-scheduling workaround is preserved; it is not bypassed to improve measurements.
- [Apple shared-event API](https://developer.apple.com/documentation/metal/mtlsharedevent): cross-process handles are an explicit API, not equivalent to a process-local counter. No fabricated cross-process support is advertised.
- [Khronos synchronization and memory visibility](https://docs.vulkan.org/spec/latest/chapters/synchronization.html). Existing transfer-to-host visibility and fence conditions are preserved.
- [QEMU reference](https://github.com/ericek111/vibe-nullmoth-xe3-qemu): its zero-copy blob scanout and virtual cursor differ from the Dell's physical-panel copy path. The reference also documents CPU/GPU coherency failures, but that does not prove Weather has the same cause.
- [WhateverXe](https://github.com/b00t0x/WhateverXe) and [original driver reference](https://github.com/VedCloud2/First-Custom-iGPU-Driver-for-Intel-Core-5-120U-on-macOS) were checked; no independently demonstrated Weather/four-timeout fix was established from them in this review.

---

# About This Mac added to 0.3.11

Same graphics installer now also stages Dell Latitude 5350 2-in-1 (13-inch, 2024) and the supplied transparent icon. Separate System-volume snapshot transaction and restore script; graphics/EFI unchanged. Target layout/icon/graphics performance remain unverified. See Desktop/AboutThisMac/README.md. Cosmetic failure returns4 with graphics still installed/armed.

---

# Candidate0.3.11 — batched cache visibility

Latest report131729 confirms0.3.10 active. User reports transparency and earlier artifacts seem corrected;heavy pointer and Weather text corruption remain. Recorded readback phases total about22ms again,mainly serial cache-line maintenance.7 CoreAnimation timeout messages remain. These are captured startup windows,not frame-rate or controlled CPU benchmarks.

## Change

Use CLFLUSHOPT for a batch of cache lines when CPU CPUID7EBX23 advertises support. Preserve MFENCE before/after the batch and all pre-GPU-overwrite/post-successful-fence cache-maintenance boundaries from0.3.10. Intel documents CLFLUSHOPT as not ordered against other CLFLUSHOPT operations,with fence instructions providing ordering. Original Dell Linux cpuinfo lists CLFLUSHOPT;runtime detection remains. If absent,the prior CLFLUSH implementation is used unchanged. No removal of cache visibility workaround,fences,barriers,or fresh command pools. Staging limits/exclusive leases and quarantine remain.

This targets the measured cost without intentionally returning to0.3.9's visual regression. Target performance and continued visual correctness need hardware validation. Build-host Rosetta exposes CLFLUSH but not CLFLUSHOPT:real legacy instructions ran;the optimized path's dispatch,address coverage and fence boundaries were tested with injected callbacks,and x86 disassembly confirms CLFLUSHOPT/MFENCE emission. No claim of a build-host hardware benchmark for the fast instruction.

Weather remains a separate unresolved issue. The new collector samples the running Weather process for3seconds(25second limit) and includes Weather log events and CPU feature metadata. Prior logs show an IOKit DRM connection and a kernel report of~5648wakeups/sec;they do not establish the cause of glyph corruption. No guessed font,texture-swizzle or blend-state changes in this candidate.

Strict signatures/import/export checks,three plugin builds,actual readback/cache-helper ASan/UBSan fixtures,loader/installer rollback and Weather present/absent collector fixtures pass. Kernel1.0.15,framebuffer0.2.5,accelerator0.2.4,ANV4GiB backend and restored touchpad EFI preserved.

Source:[Intel CLFLUSHOPT instruction and CPUID/fence rules](https://cdrdv2-public.intel.com/812383/253666-sdm-vol-2a.pdf),pages273–274.

## Dell trial

From fallback:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0,restart USB once without ResetNVRAM. Check pointer/normal desktop movement and earlier visual faults. Open Weather with the affected text visible and leave it running while collecting:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

The collector automatically captures Weather if running;no new test command or full22-case suite. Next logs must confirm cache-visibility0.3.11 optimized=1 and new phase costs. The target installer backs up prior components;one-shot fallback remains. No target install,arming or reboot from this Mac. Weather glyph fix and smoothness remain unverified.

---

# Candidate0.3.10 — retained staging CPU cache visibility

0.3.9 target result125537 confirmed substantially faster recorded readbacks(33.877ms before,4.318ms now), and5 CoreAnimation timeout messages instead of43 in the respective startup streams. This is not an FPS measurement or controlled benchmark. User reports/photos also show new coloured cursor trails,partial background repaint,black menu bar instead of translucency,text artifacts and window-movement corruption. The candidate is not validated as visually correct.

## Targeted workaround

This candidate supplements the retained-staging path with explicit x86 CPU cache maintenance: flush/invalidate cache lines and MFENCE before a retained buffer is overwritten by the GPU, and again after the successful GPU fence before copying its bytes into an IOSurface. Pre-flushing prevents dirty CPU aliases being written back over later device data; post-flushing avoids reusing old CPU cache lines. Actual architecture support and cache-line size are queried through CPUID. If unsupported, retention is disabled and the prior fresh allocation path remains.

This is a hypothesis-driven workaround for possible stale retained IOKit mappings, not proof of a diagnosed kernel cache-coherency defect. Under the Vulkan HOST_COHERENT contract extra invalidation should not be necessary; the local ANV implementation skips vkInvalidateMappedMemoryRanges for coherent types, so simply adding that Vulkan call would be a no-op. Image content preservation,damage tracking and physical scanout remain alternative explanations if artifacts persist.

Flush addresses stay within the requested allocation range,including unaligned first/last bytes; zero,overflow,unsupported geometry and capacity violations are rejected. The Vulkan transfer-to-host barrier and successful fence are still mandatory. On timeout there is no CPU cache read/copy or premature release; quarantine remains. A bounded cache of4exclusive16MiB-payload slots remains; fresh command pools are unchanged. No kernel/framebuffer/backend/EFI/touchpad changes. CPU cache maintenance adds work and may reduce some of0.3.9's speed benefit; target timing is needed.

## Local checks

Three plugin variants compile; strict signature verifies; no new external imports or missing prior exports. x86_64 ASan/UBSan tests execute actual CLFLUSH/MFENCE under Rosetta and cover guard pages,unaligned ranges,overflow,capacity and geometry. They verify byte preservation and code order,not Dell GPU visibility. Actual readback fixtures assert pre-submit/post-fence ordering,1000cached padded copies with one allocation,timeout retention/no destination writes,invalid memory type discard,known unsubmitted failure recovery,and20000forced-fresh cleanup checks. Loader/installer guards and rollback checks pass. No Dell hardware rendering suite rerun.

Sources: [Vulkan coherent memory contract](https://docs.vulkan.org/spec/latest/chapters/memory.html),[GPU-to-CPU synchronization](https://docs.vulkan.org/guide/latest/synchronization_examples.html),[Intel instruction reference](https://cdrdv2-public.intel.com/812383/253666-sdm-vol-2a.pdf). Local ANV anv_device.c InvalidateMappedMemoryRanges reviewed; no Mesa changes in this candidate.

## Dell trial

From fallback:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0,restart through USB once without ResetNVRAM. No extension-update alert is required for the plugin/loader-only update. Check background redraw at rest,cursor trails,menu transparency,text and window dragging,then collect before reboot:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

The collector should show candidate0.3.10/cache-visibility first-submit records and present-perf0.3.10 timing. No repeated22-case suite or motion script is required for this correctness trial. If still corrupt,report which symptoms remain; do not treat passing readbacks or lower timeout counts as proof of correct pixels. The target installer preserves prior plugin/loader backups and one-shot fallback recovery. No target install,arming or reboot from the build Mac. All observed artifact fixes remain hardware-unverified.

---

# Smoothness candidate0.3.9 — bounded staging leases

Latest evidence: failure-detail-20261020-123840, same boot as motion123544. Graphics reports Intel(R) Graphics(MTL),4096MBshared,Metal3. Plugin0.3.8 remained sluggish and logged43 CoreAnimation timeouts. The reduced log capture worked(1767 vs199637live lines), but this did not resolve presentation stalls.

Across72 sampled successful full-HD readbacks, weighted mean phase times were preparation/allocation24.126ms,record0.096ms,submit/wait3.888ms,CPUcopy3.712ms,cleanup2.055ms,total33.877ms. Preparation includes upload-barrier work, allocation/map and coherence checks; it is not an isolated allocation benchmark. Motion worker stacks independently show repeated staging mapping through IOKit. Main-thread scheduling/publication waits account for365/640 samples in the motion trace.

## Change

Plugin0.3.9 retains up to4 dedicated host-coherent readback staging allocations,16MiB requested payload per slot. A slot is exclusively leased until the successful GPU fence and CPU copy finish. Try-lock acquisition never waits for another lease; if all slots are busy, requests exceed16MiB, or NVMTL_READBACK_STAGE_FRESH is explicitly set, the previous fresh allocation path is used. No app/non-target staging policy changes.

Known never-submitted commands can return a slot idle. Uncertain submissions quarantine the slot and retain GPU memory; its buffer is never reused. Invalid maps/memory types discard known-idle storage. Growing a slot destroys its previous idle buffer before replacement. Normal retained requested payload is bounded to64MiB total plus driver allocation alignment/metadata. Transient fresh-fallback allocations remain possible; this is not a new global memory quota. Retained buffers are an intentional process-lifetime cache, not a per-frame leak. Process exit reclaims the process's resources.

Command pools are still fresh for every submission and retired using the existing completion rules. This is staging-memory reuse, not command-pool/command-buffer reuse. All host coherence checks, dependency barriers, layout restore, serial GPU submission and schedule-after-publication rules remain. No zero-copy or framebuffer kernel changes. See Vulkan's GPU-to-CPU synchronization example: https://docs.vulkan.org/guide/latest/synchronization_examples.html

The expected benefit is avoiding repeated full-HD buffer allocation/mapping and cleanup after warm-up. This is a hardware-unverified candidate; measured smoothness,CPU and timeout reduction are not claimed. First allocations,large buffers or contention can still take the old path. Shader startup cost and CPU copying remain separate work.

## Validation and trial

Three plugin variants compile; strict signature verifies; no external imports added or old exports missing. Actual lease helper passes8000 concurrent ASan/UBSan cycles and allocation/error/quarantine/fallback tests. Actual readback fixture performs1000 cached padded copies with one allocation, plus cached failure recovery/quarantine and the previous20000 forced-fresh allocation/reclaim checks. These are mocked CPU fixtures, not Dell GPU tests. Installer rollback,loader and predicates pass. Touchpad remains rolled back; EFI is preserved.

From the Dell fallback desktop:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit0, restart through USB once without ResetNVRAM. No extension-update alert is required for this plugin-only change. Let desktop settle and assess normal movement, then collect:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-motion.sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

Motion requests eight seconds of window dragging/Dock interaction after a five-second lead-in. Collect before rebooting to preserve current accelerated state. This is a short comparison of the changed display path, not the22-case rendering suite. If it fails, next USB boot returns fallback; collect retained failure logs before another install. Target installer retains prior plugin/loader/backend backups. No target install,arming or reboot has been performed from the build Mac.

---

# Smoothness candidate0.3.8 — October20

Focus: WindowServer startup fence timeouts and CPU/animation cost. Touchpad rollback remains in place.

The prior live startup capture contains199637 lines in about120seconds. Four repeated multitouch debug message classes account for160893 lines. Broad debug streaming was active at the same time as a second startup stream. Both filters now retain graphics/plugin diagnostics and process errors/faults without broad HID debug capture. This removes an avoidable diagnostic load; hardware impact on CPU, timeouts and smoothness is unverified. Captured line volume does not quantify CPU cost or prove that all fence timeouts were caused by logging.

Plugin0.3.8 records bounded successful-readback phase averages (upload-preparation/allocation/coherence checks, command recording, serialized submit/wait, CPU copy, staging cleanup), maximum total and bytes. Two-second minimum report windows,eight windows per calling thread; normal release logger also throttles the common format. After the per-thread cap, timing clocks are bypassed. It does not change rendering behavior, remove fences, recycle command pools, cache staging buffers or implement zero-copy. Failed submissions retain the prior error handling. The device-lifetime pipeline cache from0.3.7 remains; it does not persist across boots. Vulkan describes pipeline creation as potentially costly and pipeline caches as a way to reuse compilation: https://docs.vulkan.org/guide/latest/pipeline_cache.html

Early shader compilation remains a possible startup contributor. The prior three-second sample13minutes after boot was mostly idle and showed no compiler frames; it cannot measure choppiness during animation. The physical display path still includes readback/copy for non-host-backed IOSurfaces. Its actual phase costs need measurement. High ps CPU percentages in previous snapshots are lifetime averages, not proof of continuous high CPU at collection time. The motion capture also records cumulative CPU time so a delta can be calculated.

Late startup sample deadline is25seconds(previous10seconds killed symbol processing). Early/late samples remain three seconds. Errors/faults, CoreAnimation fences, plugin diagnostics and kernel GPU diagnostics are retained by the actual tested predicates. No new panic/GPU hang was identified in the reviewed October20 steady-state report.

## Dell trial

From fallback, run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

If exit=0, restart through USB once without ResetNVRAM. This arms one accelerated boot. The installer backs up previous runtime components. Existing GPU/backend/framebuffer remain the verified versions; no extension-update alert is required for a plugin/loader-only change. The restored input EFI remains untouched.

Once the desktop appears and settles, run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-motion.sh
```

It announces a five-second lead-in, then asks for eight seconds of continuous window dragging/Dock interaction. This samples normal desktop work; it runs no GPU validation suite and does not measure FPS directly. Then collect retained startup timings/errors:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

Collect before rebooting so motion sample represents the active accelerated desktop. Return USB for analysis. No22-case rendering suite rerun. Smoothness and timeout reduction remain unverified until the target trial.

---

# Confirmed4GiB; touchpad inline ACPI correction pending trial

11:58 report: queried shared heap4294967296bytes and4096MB in System Information; plugin pipeline cache active, but47 CoreAnimation timeouts and CPU spikes remain. I2C3 controller/D0 works; ELAN HID nub is unregistered because separate SSDT scope fails during load. USB now inlines those methods in TPD0 and removes the external SSDT dependency. Local interpreter/rollback checks pass; gestures need next-boot confirmation. Graphics binaries unchanged. Updated collector can capture current desktop stack with25s watchdog. See windowserver-result-20261020-115838.md.

---

# Current candidate: backend4GiB + pipeline cache0.3.7 + Dell I2C3 touchpad

Based on verified October19 checkpoint. Backend enforces4GiB shared heap on Darwin integrated8086:7d45; existing reporter reads its actual limit. Device-lifetime graphics pipeline cache targets repeated compilation; fresh command pools remain. Dell7e7b controller power/matching and scoped TPD0 ACPI fixes prepare multitouch attachment. Local checks pass; target smoothness/gestures/cap need trial confirmation. GPU1.0.15/FB0.2.5/accelerator/transport/HID guard retained. See backend4g-touchpad-20261020.md.

Boot USB fallback; run install-windowserver.sh; exit0 then restart USB once without ResetNVRAM. Let desktop settle about1minute, check gestures, collect-desktop-failure.sh. No22-case suite. restore-touchpad.sh reverts only USB input changes. Latest verified checkpoint archive preserved. Historical instructions below are superseded.

---

# Confirmed: real queried GPU name and 12288MB shared heap

Dell23:14 boot/report231630 confirms Intel(R) Graphics (MTL),12288MB shared memory in System Information, and successful PCI publication/readback. GPU1.0.15/FB0.2.5/plugin0.3.6 active. Choppiness remains:42 CoreAnimation timeout lines, WindowServer CPU128–206% in three snapshots, early pipeline compilation and private-image/copy surface path. No new GPU hang or current-boot crash/panic captured. See windowserver-result-231630.md. No executable/EFI change or test-suite rerun from this review.

---

# Current: queried GPU name and shared capacity in System Information

LatitudeMTLFB0.2.5 and the new boot helper publish the actual ANV deviceName/shared heap to the matched PCI model/VRAM,totalMB registry fields. No fixed GPU name/2048MB injection; physical scanout mapping and GPU1.0.15 unchanged. Local build, sanitizer, installer/rollback and loader checks pass. Target label confirmation pending. Saved working checkpoint and EFI unchanged. See windowserver-registry-report-0.2.5.md.

Dell fallback: install-windowserver.sh; after exit0 restart through USB once without ResetNVRAM. Collect on usable trial, or fallback after failure, before reinstalling. No full GPU suite rerun. Historical notes below are superseded by this candidate.

---

# Current: automatic Metal identity and shared-memory reporting0.3.6

GPU name and heap capacity come from Vulkan queries. No fixed model or 2048MB injection; EFI unchanged. Unified-memory reporting corrected and unsupported host imports use existing copy fallback. System Information Display7MB registry reporting remains unresolved. GPU1.0.15/framebuffer0.2.4/transport retained and working checkpoint saved. Local builds/checks pass; this candidate requires target confirmation. No full GPU suite rerun. See windowserver-memory-report-0.3.6.md.

---

# Target checkpoint:1.0.15 usable desktop + WindowServer GPU completions

Report224146 confirms same-boot usable desktop, applied firmware-scanout reservation, successful WindowServer Vulkan initialization,24/24logged submission fence completions and32/32logged readback completions (bounded traces, not total workload counts). No previous GuC startup crash or new WindowServer crash captured. This is real target improvement. Remaining: host import uses private-image/copy fallback,35Core Animation timeout messages, high CPU snapshot; efficient/stable complete presentation not yet certified. Preserve1.0.15 as the current usable checkpoint. See windowserver-result-224146.md.

---

# Current: NMXGPU1.0.15 GPU startup / firmware scanout reservation

Corrected diagnosis: GuC crashed and i915 wedged during GPU initialization at22:19:41, BEFORE WindowServer zero-device queries.1.0.15 replaces whole stolen-region exclusion with bounds-checked/aligned actual GOP prefix exclusion and restores remaining stolen allocator. Adds init-phase errors and collector initialization summary. Local ASan/UBSan,438-object build, signature and installer/KC rollback checks pass. Hardware recovery and full accelerated desktop remain UNVERIFIED.

Dell fallback: install-windowserver.sh; after exit0 restart USB once without ResetNVRAM. On failure poweroff/USBfallback and collect-desktop-failure.sh before reinstalling. No22-case rerun. See windowserver-scanout-reservation-1.0.15.md.

---

# Current: WindowServer IOKit transport correction

22:07 trial: three CoreDisplay assertion crashes; Vulkan zero devices; sandbox denies writable render-node opens and ioctls. Reproduced mixed transport identity gap corrected: denied writable renderD128 opens use IOKit token directly; existing IOKit real-fd identity uses its DRM file. Added bounded startup ioctl results. Local sanitizer/build/installer rollback checks pass; Dell fix and full acceleration remain UNVERIFIED. GPU1.0.14, framebuffer0.2.4, plugin0.3.5 and HID1.0.1 retained.

Run install-windowserver.sh on Dell fallback; exit0 then restart USB once without ResetNVRAM. On failure fallback and collect-desktop-failure.sh before reinstalling. No22-case rerun. See windowserver-iokit-transport-220839.md.

---

# Current: HID null-property panic guard + early capture

Latest trial panicked in VoodooI2CHID forwardReport+0x4f at9.1seconds. Original USB binary matches panic UUID; missing IOFBTransform cast dereference reproduced. USB HID1.0.1 guards it and preserves input reports;200000 local cases pass. GPU1.0.14 unchanged. Full accelerated desktop remains UNVERIFIED. See windowserver-hid-nullguard-1.0.1.md.

Boot Dell through USB fallback; run install-windowserver.sh; after exit0 restart through USB once without ResetNVRAM. On failure boot fallback and collect-desktop-failure.sh before reinstall. No22-case rerun. Original USB HID backed up; restore-usb-hid.sh restores it.

---

# Current: NMXGPU1.0.14 completion context lifetime

1.0.13 panicked in GuC retirement at8seconds.1.0.14 adds explicit context/parent ownership to queued breadcrumb requests and safe successor iteration. Old UAF reproduced with actual functions in sanitizer fixture;90000 corrected lifetimes pass. Kernel build/signature and installer/KC rollback checks pass. Target panic cure and accelerated desktop remain UNVERIFIED.

Dell fallback: install-windowserver.sh; after exit0 restart USB once without ResetNVRAM. On failure poweroff/USBfallback then collect-desktop-failure.sh before reinstall. Collector includes kernel panics. No full GPU suite rerun or automatic reboot. See windowserver-completion-context-1.0.14.md.

---

# Installer retry correction for1.0.14

The18:30collection shows the driver parked because nullmoth-gpu-attaching survived the earlier1.0.13panic.1.0.14loaded but did not attach, so that attempt did not validate the new completion fix. The installer now saves/clears/verifies only that stale marker before explicit one-shot arming; kernel boot guard stays enabled. If deletion fails, no trial is armed. No new driver binaries or full suite rerun. Research: ../windowserver-1.0.14-attach-guard.md.

---

# Current: NMXGPU1.0.14 completion context lifetime

1.0.13 panicked in GuC retirement at8seconds.1.0.14 adds explicit context/parent ownership to queued breadcrumb requests and safe successor iteration. Old UAF reproduced with actual functions in sanitizer fixture;90000 corrected lifetimes pass. Kernel build/signature and installer/KC rollback checks pass. Target panic cure and accelerated desktop remain UNVERIFIED.

Dell fallback: install-windowserver.sh; after exit0 restart USB once without ResetNVRAM. On failure poweroff/USBfallback then collect-desktop-failure.sh before reinstall. Collector includes kernel panics. No full GPU suite rerun or automatic reboot. See windowserver-completion-context-1.0.14.md.

---

# Current: NMXGPU1.0.13 firmware-owned DSM exclusion

Source inspection found missing initial-plane ownership protection: i915 could allocate stolen objects while LatitudeMTLFB retains the panel there.1.0.13 withholds stolen regions on7d45, allowing standard system-memory fallback. Previous1.0.12 still corrupted its ring. The specific failed-ring overlap is unproven; hardware desktop success remains unverified.

Locally built/signed, sanitizer regression and installer/KC rollback checks pass. Original1.0.8/22-case baseline retained. No full GPU suite rerun. Dell: install-windowserver.sh on fallback; after exit0 restart USB once without ResetNVRAM; collect-desktop-failure.sh on trial if usable, otherwise poweroff/USBfallback then collect before reinstall. No automatic reboot. See windowserver-firmware-stolen-1.0.13.md.

---

# Current: raw physical allocation fix candidate

NMXGPU1.0.12 + LatitudeMTLFB0.2.4 correct a host-versus-IOMMU contiguity assumption verified against Apple's Sequoia source. The old allocator could expose a whole surface from only its first raw physical segment. New allocations require host-physical contiguity and verify the actual span before export. This can explain pixel writes reaching unrelated memory, but the target cause and full desktop acceleration still need hardware validation.

Fallback Dell: `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh`. After exit0 reboot USB once without ResetNVRAM. If it fails, poweroff/USBfallback and run collect-desktop-failure.sh. No full hardware-suite rerun. GPU kernel and framebuffer are updated together with guarded KC validation and rollback. Plugin0.3.5 and firmware/render policies remain unchanged.

Research: windowserver-raw-physical-1.0.12.md.

---

# Current: NMXGPU1.0.11 mapping-lifetime fix candidate

The latest1.0.10 capture shows valid trailing ring commands overwritten with0xff000000 before the next submission. A source audit found that static i915 CPU mappings lacked the lifetime page pin required by this port's empty revocation shim.1.0.11 adds that pin and releases it after unmapping, and refuses unsupported legacy GTT mmap. Local lifetime/error/rollback tests pass. Hardware hang resolution and full desktop acceleration remain unverified.

Fallback Dell: `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh`. After exit0, reboot USB once without ResetNVRAM. If it fails, poweroff/USBfallback and run collect-desktop-failure.sh before reinstalling. No full hardware suite. Plugin0.3.5, firmware and render policies are unchanged.

Research: windowserver-mmap-lifetime-1.0.11.md.

---

# Current: WindowServer boundary diagnostic1.0.10

Candidate0.3.5 still hangs on submission8 despite noccs. Installation and driver loading succeeded. The new kernel1.0.10 captures bounded command-ring words before submission and at the hang; it is a diagnostic build, not a verified acceleration fix. Plugin0.3.5 and firmware/scheduling/pool policies remain unchanged. The full hardware suite is not rerun.

Fallback Dell: `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh`. Only after exit0, reboot USB once without ResetNVRAM. If it fails, poweroff/USBfallback and run collect-desktop-failure.sh before reinstalling. This attempt may black-screen again; it is intended to collect the missing request-boundary evidence.

Research and limits: windowserver-boundary-diagnostic-1.0.10.md.

---

# Installer cache-path repair ready

The13:36 boot did not load NMXGPU: its AuxKC referenced a deleted temporary bundle. WindowServer waited for GPU publication. It did not test the compression workaround. The installer now rebuilds the final cache from the permanent installed bundle and verifies path/UUID before arming. It supports the exact installed1.0.9-but-not-loaded repair state.

On fallback Dell: run install-windowserver.sh again. Afterexit0, reboot USB once without ResetNVRAM; then collect-desktop-failure.sh. No full hardware-suite run. Driver binaries remain0.3.5/kernel1.0.9; their desktop behavior is still unverified.

[Evidence and installer correction](../windowserver-0.3.5-cache-repair-133725.md).

---

# Current:0.3.5 compression-isolation and kernel-capture candidate

The0.3.4 active desktop still hung on submission8/third readback with dedicated staging. The next candidate sets noccs only for this WindowServer/device and installs diagnostic NMXGPU1.0.9. Kernel capture now reports referenced request state and parsed GuC registers; zero legacy ring fields were not a complete hardware snapshot. No exact root cause or complete desktop fix is claimed.

Local builds, signatures, sanitizer fixtures, kernel link/import comparison, compression scope, installer/KC rollback and regressions pass. Saved22case/26,000frame evidence and original1.0.8 bundle are retained; no suite rerun requested. Firmware, accelerator, framebuffer and transport remain unchanged. The1.0.9 kernel changes diagnostics, not scheduling/reset/timeouts.

Next: Dell fallback install-windowserver.sh; this also prebuilds/backs up/updates AuxKC and kernel. Afterexit0 restart USB once without ResetNVRAM. Then collect-desktop-failure.sh on visible desktop, or poweroff/USBfallback then collect if it fails. No automatic reboot. See windowserver-0.3.5-research.md.

---

# Current:0.3.4 dedicated readback candidate ready; desktop success unverified

Latest active0.3.3 attempt completed eight initial submissions/three readbacks, then readback4 timed out and the kernel reported a GPU hang. New WindowServer crashes followed backend loss. Root logs survived; plugin file logging was sandbox-denied and sample falsely exited0 without a usable stack.

Candidate0.3.4 scopes dedicated fresh staging to8086:7d45 WindowServer, cleans known pre-submit allocation failures, and fixes those logging/sampler defects. Local compile, signatures, sanitizer fixtures,20,000balanced mocked allocation lifetimes, release policy,80failed-buffer, installer rollback and loader checks pass. No claim of a complete/future-proof desktop fix. See windowserver-0.3.4-research.md for primary references, hypotheses and unresolved risks.

No kernel/firmware/accelerator/framebuffer/transport change; saved22case/26,000frame baseline remains. Next: Dell fallback install-windowserver.sh; afterexit0 reboot USB once without ResetNVRAM. Then collect-desktop-failure.sh on visible desktop, or after poweroff/USBfallback if it fails. No full-suite rerun or auto-reboot.

---

# Current: candidate0.3.3 ready for one targeted WindowServer boot

The0.3.2 black-screen boot lacks retained plugin/fence evidence; no exact cause is established. Candidate0.3.3 bounds the previously unbounded desktop submission-lock acquisition and adds a pre-created startup stage journal, early loader capture and an attempted WindowServer sample. Failed lock acquisition disables the backend and quarantines resources safely. Local build, signature, sanitizer, policy, error-path, installer rollback, loader and collector checks pass. Hardware desktop validation remains pending.

The verified22case/26,000frame baseline remains intact. No GPU suite rerun is requested. Kernel, accelerator, framebuffer/claim, transport and firmware remain byte-identical. On Dell fallback: install-windowserver.sh, then after exit0 reboot through USB once. After failure: poweroff/USBfallback, collect-desktop-failure.sh before reinstalling. See windowserver-0.3.3-research.md for evidence and limits.

---

# Current:0.3.2 WindowServer-only candidate

## Candidate 0.3.2 prepared — WindowServer-only trial

The22case/26,000frame application baseline is retained. Latest physical desktop failed: second readback and another surface upload timed out, then CoreDisplay threw while creating a texture after backend shutdown. Candidate0.3.2 serializes normal WindowServer submissions, disables submit-ahead and defers IOSurface-writer scheduling until successful copy-out. Scoped to8086:7d45 WindowServer; applications keep normal concurrency. No kernel, framebuffer, firmware or security changes. Local compile/sanitizer/order/error/installer regressions pass; target desktop success pending.

Next on fallback Dell: install-windowserver.sh, then after exit0 reboot through USB once. Installer changes only the Metal plugin and arms one desktop boot using the saved hardware baseline; no GPU suite rerun or auto-reboot. After failure, poweroff/USBfallback then collect-desktop-failure.sh; full crash reports and latest attempt window included.

---

# Current:0.3.1 GuC wait compatibility candidate

## Candidate0.3.1 prepared: NMXGPU1.0.8 GuC wait compatibility fix

The verified0.3.0 test passed4,000frames with zero live command pools. A GuC TLB-invalidation timeout preceded the fifth process, which later hung on upload#1285 at frame428. Fresh pools remain enabled but do not establish complete stability.

A deterministic test of actual scheduler/i915 wait code reproduced a pre-enrollment lost wake in the LinuxKPI woken callback. Kernel1.0.8 persists the task wake state under the sleep-channel lock.100,000sanitizer scenarios and438-object kernel link pass; no new undefined symbols. This fixes a verified software race; target hang causation and hardware validation remain pending.

Next: install-readback.sh; after exit=0 reboot USB once; test-and-arm-desktop.sh. Full22case gate now requires the loaded1.0.8UUID and rejects timestamped kernel TLB errors/hangs/resets. Log-filter/command metadata are excluded. Driver/plugin/firmware/display policies otherwise unchanged; no automatic reboot. Research: fresh-pool-guc-research.md.

---

# Current: 0.3.0 fresh-pool enforced candidate

## Candidate 0.3.0 fresh-pool enforced prepared

The master plugin now creates fresh command pools everywhere and destroys completed pools immediately after fence/witness completion. Pool cache/pop/push paths are removed; queue initialization no longer leaks an unused pool, and dummy initialization uses the common tracked allocator. Unsubmitted allocations clean up promptly. Uncertain GPU completion is quarantined and blocks the hardware gate.

Local compilation, sanitizer lifetime/completion checks, release constructor policy checks and mocked full 22-case/arming guards pass. USB signatures and hashes are verified. These are local checks; 0.3.0 has not yet passed the Dell hardware checklist or accelerated WindowServer/panel test.

Next on Dell: install-readback.sh; after exit=0 reboot USB once; then test-and-arm-desktop.sh. The script runs all 22 cases (1000 iterations each worker) and arms one desktop boot only after the current-run gate passes. No automatic reboot. Keep using the one-shot fallback after a failed panel boot.

---

# Current: 0.2.9 controlled GPU-hang comparison ready

Earlier release-environment resource-switch comparisons are also unverified because of the same constructor scrub. Their exact-pixel passes and IOKit counters remain valid; those do not prove distinct resource policies were exercised.

## Candidate 0.2.9 effective-policy repair prepared

The latest 0.2.8 test (readback-20261018-231611) hit GPU HANG 12:1:00b79797 at frame 169. It does not test fresh pools: release initialization stripped NVMTL_* settings, including the experimental flags and direct diagnostics. The policy file only recorded intent. No conclusion about pool reuse is justified.

The fix narrowly preserves explicit controls for the named test process and adds an in-driver effective-policy query. Each workload refuses to run unless the actual post-initialization policy matches the requested profile. Tests against the real native release plugin constructor passed for all three profiles, unrelated controls and another process. Desktop activation remains disabled; target validation remains pending.

Next: install-readback.sh, reboot USB, then diagnose-readback.sh fresh-pools. No GPU-hang cure is claimed.

---

# Current: 0.2.8 controlled GPU-hang comparison ready

## Candidate 0.2.8 diagnostic comparison prepared

Recovered 0.2.7 evidence confirms GPU HANG 12:1:00b79797 and readback VK_ERROR_DEVICE_LOST after 13,072 ms, followed by a context reset. IOKit access passed; correct WindowServer remains unverified.

The USB now contains plugin 0.2.8 with direct per-test diagnostics and a command-pool-only fresh-allocation switch. The same three warmups precede a single retained/fresh-pool/fresh-staging comparison. Witnesses and address-page caching stay on in the fresh-command-pool case. The runner checks boot UUIDs and refuses another comparison in the same boot. Kernel events are captured live and historical lookup starts at the current run. Desktop activation is disabled in this candidate; no GPU-hang cure is claimed.

Next target step: install-readback.sh, reboot USB once, then `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/diagnose-readback.sh fresh-pools`. Return the first report before selecting the next clean-boot variant. Further variants require prepare-readback-boot.sh and a USB reboot. Full instructions: Desktop/README.md.

---

# Current: 0.2.7 IOKit discovery passes; long-run readback blocked

# Candidate 0.2.7 target result: IOKit works, long-run readback fails

Report: Desktop/reports/readback-20261018-203542. Install succeeded. IOKit enumeration/open/token identity/nested-pointer ioctl probe passed. Small raw, small render and full-HD render each passed 1,000 frames through verified IOKit transport. Padded full-HD readback failed at zero-based frame 745: channel 0 of pixel (0,0) remained 165 (0xA5, the initial destination fill), expected 33. It made 2,007,898 IOKit ioctl calls, zero direct calls. The ioctl count alone does not establish a fault cause. The test stopped before pool/staging A/B or IOSurface tests. No desktop gate or panel opt-in was set by the failed suite.

The untouched destination is consistent with an early readback error, but the existing public getBytes API is void. It is not proof of a stride arithmetic defect, GPU data corruption or a specific fence failure. The historical log search timed out while still reading earlier 20:18 events, before the current 20:35 boot. Its absence of a current GPU error is not useful evidence.

A read-only collector now queries a narrow failure-time interval derived from the report timestamp, captures kernel/NVMTL/IOKit messages, and saves current boot/registry/dmesg without issuing GPU tests or changing boot variables. Run `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-readback-failure.sh` on the Dell before another install or reboot. Save the USB afterward for inspection.

Local diagnostic build 0.2.8 is prepared but is NOT the USB runtime: explicit test-only stderr logging survives syslog throttling; readback and submit errors identify their phase; a revised runner captures kernel events live, bounds historical search to the current run and provides one-variable retained/fresh-pool/fresh-staging profiles. No GPU execution fix is claimed from this evidence. USB runtime remains 0.2.7 pending recovery of its original error.

---

# Current: candidate 0.2.7 IOKit and desktop test prepared


## Candidate 0.2.7 prepared — target validation pending

0.2.6 passed all 10 direct cases on the Dell (1,200 frames, report 20261018-201754). Forced IOKit failed during enumeration before any ioctl. The new candidate fixes inconsistent forced-route enumeration and token node classification, adds automatic IOKit discovery fallback, and corrects prior-access synchronization during IOSurface reuploads. It also fixes the release filter prefix so image/staging traces are retained.

The revised suite tests IOKit first, probes metadata and nested-pointer ioctls, and checks actual RGBA/BGRA IOSurface backing pixels immediately after GPU completion. `test-and-arm-desktop.sh` runs 1,000 iterations across 22 cases and enables ONE desktop boot only if all cases pass. It does not reboot automatically. Use install-readback.sh, reboot USB once, then test-and-arm-desktop.sh. Details: Desktop/README.md.

Local ASan/UBSan routing/discovery/barrier/failure tests and native Apple M4 harness checks pass. They do not establish Intel IOKit correctness or WindowServer success. Both still need the target test. Shared-image recording concurrency and broader display features remain unverified.


## Candidate 0.2.6 prepared — Intel tests pending

The readback correctness patch and isolated test matrix are on the USB. Local ASan/UBSan recording/error tests pass. Native Apple M4 exact-pixel tests validate the harness only. Default installation is now readback-only; physical panel ownership is disabled for the next GPU application test. Automatic WindowServer `INTEL_DEBUG=stall` was removed after its failed target result.

Run `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-readback.sh` on the Dell. After exit=0, reboot USB once and run `sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/test-readback.sh`. Reports go to `Desktop/reports/readback-TIMESTAMP`. The 100-iteration smoke test comes before the 1,000-iteration gate and another desktop attempt.

Implemented: validated full/region readback, transfer-write/host-read dependency, copied-subresource layout restoration, coherent-memory verification, timeout retention, image/stage identities, dedicated-staging option, deterministic raw/render/IOSurface tests, controlled two-thread cases, verified direct/IOKit route counters.

Pending: target results, broader render/compute/blit image-state concurrency, synchronization validation, conditional kernel-request/GEM correlation, desktop stability and later display features. Artifacting and full acceleration are not confirmed fixed.

---

# Latitude 5350 port status — 18 October 2026, after desktop 0.2.5

The current plan is [ROADMAP.md](ROADMAP.md). It replaces the earlier status page's obsolete statement that the runtime is untested.

Application GPU output was demonstrated in earlier Dell reports: Vulkan compute, Metal compute and the small Metal pixel-output test passed. This is limited test coverage, not broad application compatibility or stability certification.

Physical-panel desktop acceleration is still failing. The latest experimental boot, preserved in Desktop/reports/20261018-194932, verifies framebuffer attachment, existing display-pipe identity, phase-2 power notifications and an active pipe. The first four traced GPU submissions complete; submission #5 (image readback) and the following upload time out. The render engine subsequently hangs. Presentation samples remain zero and the user sees artifacts.

The 0.2.5 command-buffer change removed the previous stack-overflow crash signature from this test, but WindowServer still aborts when the backend is unavailable and restarts repeatedly. INTEL_DEBUG=stall was enabled and did not cure the hang. Neither is a successful desktop result.

Current desktop package: NMXGPU 1.0.7, NMXAccel 0.2.4, LatitudeMTLFB/LatitudePanelClaim 0.2.3 and NVMTLDriver 0.2.5. The separate Acceleration package remains the earlier application-testing package. Reports collected on the fallback boot with the one-shot GPU inactive must not be confused with tests of the active experimental boot.

Next work: prepare an isolated full-size readback reproducer and resource-correlated trace; audit readback host visibility, layout/subresource ownership and resource reuse; compare direct versus verified IOKit transport. Investigate the first GPU error using that evidence. The roadmap defines promotion criteria before another physical-desktop candidate.

No reboot or reinstall is requested for this documentation update. Do not repeat the unchanged 0.2.5 installation as a new experiment. The existing one-boot opt-ins, target preflight, backups and rollback remain in place. If recovery is needed after an opted-in failed boot, boot through the USB again without reinstalling, then collect-desktop.sh from the fallback desktop.

Security/EFI history: the earlier USB setup used csr-active-config Data 430A0000, SecureBootModel Disabled, Lilu and AMFIPass. This roadmap update changes none of those settings. Their original backups remain under iGPU-Port/backups and on the build Mac.

Detailed evidence: [0.2.5 result](desktop-0.2.5-result.md), [0.2.5 implementation](desktop-0.2.5-implementation.md), and [parsed timeline/crashes](desktop-roadmap-evidence.json). Hardware video decode, sleep/wake, brightness, modesetting, external displays and complete desktop support remain unverified.
