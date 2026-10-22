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
