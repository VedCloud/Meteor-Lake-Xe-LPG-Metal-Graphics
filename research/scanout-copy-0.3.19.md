# Candidate 0.3.19: guarded publication lock and bounded colour copy

Target: Dell Latitude 5350 2-in-1, 8086:7d45, Sequoia 24H32. Prepared privately from report failure-detail-20261020-220831. No target install, reboot, or hardware validation performed while building this candidate.

## What the report establishes

The loaded identities match Metal 0.3.18, accelerator 0.2.8, and framebuffer 0.2.9. Night Shift is confirmed visually by the user and supported by LatitudeGammaActive=Yes, transaction colour dispatch, generation145, post-gamma table present, 454 successful nonidentity gamma copies, and zero refusals. The report shows four current-boot CoreAnimation fence timeouts; no GPU HANG/device-lost signatures were found in the retained live log. Absence of those signatures is not proof of correctness under all workloads.

Review of all 64 publication records changes the earlier partial interpretation: most lock acquisitions were short, but one took427.573ms, accounting for most of that transaction's443.973ms publication phase. Other outliers include160.214ms preparation and188.632ms pixel copy. Submission/wait reaches622.431ms in a separate transaction; that includes CPU imports and submission serialization, not just hardware execution. Wall times also include thread scheduling. These observations do not prove a single universal stall mechanism or a scanout ownership conflict.

WindowServer CPUtime rose4.54sec over approximately8sec (about57% of one core on average; the final instantaneous ps percentage was105.9). Nonidentity gamma copies rose139 over that window. Those counts are not FPS. Whole-frame/render-to-scanout latency remains unavailable because capture permission was absent. Weather was not running, so the report contains no Weather rendering stack or confirmed failing atlas layout. A global font-pitch rewrite is not supported by this evidence.

The firmware-counter request returned0xe00002f0, kIOReturnNotFound. ECDV exists in the ACPI snapshot; the old code searched for a registered LCD service. Failure to find that service cannot establish that firmware did not generate key events. BrightnessKeys was active, with no handled panel-notification marker. The beep establishes neither the scan code nor a correct brightness consumer event.

## Changes

Metal0.3.19 first tries kIOSurfaceLockAvoidSync only when a complete, fenced CPU staging snapshot exists. Apple's documented contract lets the call fail with CannotLock if costly paging/readback is required. That precise error falls back to the existing normal lock; other errors still fail without writing or publishing. Unlock uses the exact successful options. ReadOnly seed/tag handling now tests the ReadOnly bit rather than treating all nonzero options as ReadOnly. Staging saturation retains the original synchronous path. There is no destination replacement, frame dropping, raw-pointer lock bypass, or early completion signal. This is a candidate fast path, not a guarantee that implicit synchronization or all lock stalls disappear.

Accelerator0.2.9 uses a bounded128-pixel (512-byte) scratch tile for nonidentity gamma conversion. Source and destination transfers use integer REP MOVSB; LUT arithmetic runs on the cacheable tile. Numerical output, alpha, row padding, bounds and final SFENCE are unchanged. Identity still uses the existing bulk path. No SIMD/FPU context use is introduced; disassembly confirms integer-only code and a0x568-byte stack frame for the containing copy helper. Target performance is unmeasured; cached host-memory tests are not a benchmark of the Dell's scanout mapping. A cumulative gamma-copy nanosecond counter allows the collector to measure this work separately.

Framebuffer0.2.10 resolves the pure software-counter method by its absolute ACPI namespace path through registered ECDV, rather than requiring LCD to register as an IOService. Root, validated-panel and stopping guards remain. It does not inject key events or change firmware modes. This repairs the identified discovery approach; the target must confirm that the ACPI query succeeds. Brightness-key operation is not claimed fixed.

Slow submission logs separate mutex acquisition, submission begin, and finish. The part-level log separately records imports, surface output and buffer output. Existing GPU-wait label is retained for compatibility but still describes the combined phase. All new records are capped. Queue serialization, fresh command pools, actual fence waits and completion order remain unchanged.

The collector includes gamma-copy time and a bounded graphics ACPI snapshot, and explicitly warns when Weather is absent. It neither starts Weather nor requests capture permission. No full rendering suite is required.

## Validation and remaining work

x86_64 builds and signatures; actual publication-method fixtures for AvoidSync success, CannotLock fallback, matching unlock options, failed GPU/lock behavior, unchanged padding and lease cleanup;60 gamma guard-page layouts including tile-edge widths, unequal pitches, alpha/reference comparison, source preservation and overflow rejection; gamma decoder/setter fixtures; root counter endpoint fixture; command completion ordering; collector and guarded installer/kernel rollback fixtures. No Dell GPU test or target performance claim follows from these checks.

Weather text, native brightness-key delivery and elimination of CoreAnimation timeouts remain unresolved. A triple-buffered IOSurface scheduler requires destination ownership, retirement and dependency integration; three CPU staging slots alone are not that scheduler. Forced64/128-byte alignment would not repair an unknown format/swizzle/layout error, and the sampled BGRA pitches are already aligned. Preserve the working Night Shift/audio/slider baseline while measuring the candidate.

## Target check

From the working fallback desktop, run the existing install-windowserver.sh. After exit0, reboot through the USB. Leave Weather open with the faulty text visible, move/resize it normally, try F6/F7 alone and with Fn, leave Night Shift enabled, and run collect-desktop-failure.sh. No stress-test checklist rerun is requested. Keep the report from that same accelerated boot.

Previous EFI and replaced package files are saved in backups/scanout-copy-before-0.3.19 on the USB and locally. EFI, NMXGPU1.0.15, ANV4GiB, firmware, sound, restored touchpad, panel PWM controls, branding and security policy are unchanged. The backup is not an OS/AuxKC snapshot.

## Primary references

- Apple IOSurface avoidSync: https://developer.apple.com/documentation/iosurface/iosurfacelockoptions/avoidsync — detect expensive paging/readback, CannotLock on refusal; not permission to write without a successful lock.
- Installed Apple SDK IOSurfaceRef.h and IOSurfaceTypes.h — lock/unlock seed contract and option bits.
- Apple Metal triple buffering: https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/TripleBuffering.html — ownership/completion and memory/latency tradeoffs.
- Khronos VkBufferImageCopy: https://docs.vulkan.org/refpages/latest/refpages/source/VkBufferImageCopy.html — bufferRowLength in texels; zero denotes tight packing.
- BrightnessKeys upstream: https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp — ACPI panel selection and handled-notification markers.
- Linux device I/O documentation: https://www.kernel.org/doc/html/latest/driver-api/device-io.html — framebuffer memory transfers and write-combining behavior; this is background, not evidence of the target's measured bottleneck.
