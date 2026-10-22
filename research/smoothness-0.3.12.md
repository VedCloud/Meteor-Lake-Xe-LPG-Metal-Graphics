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
