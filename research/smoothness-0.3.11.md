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
