# Candidate 0.3.15: brightness keys, short publication locks and complete wallpaper redraws

This is a target-validation candidate, not a guaranteed fix. It retains the working Control Centre brightness, custom About This Mac icon, captured 13.3-inch panel EDID, kernel1.0.15, framebuffer0.2.7, 4GiB ANV backend and transport. Metal plugin becomes0.3.15; accelerator becomes0.2.6. Fresh command pools, cache-visibility boundaries, successful-fence requirements and one-shot fallback policy remain.

## What the latest evidence establishes

Report172853 has67 early readbacks averaging5.004537ms,11 retained CoreAnimation fence-timeout messages, publication outliers up to564.350ms and submission/wait outliers up to624.475ms. WindowServer blocks in waitUntilScheduled in122/275 main-thread samples. No GPU-hang/device-lost signature appears in the examined log. These facts do not prove an optimized whole rendering loop, permanent GPU stability, an IOSurface scanout-lock bottleneck, a compressed glyph atlas, or a row-pitch mismatch. The 5ms includes staging, GPU transfer/wait, cache work and CPU copying; it is not total GPU rendering time. The long publication measurement combines several operations and previously did not isolate IOSurface lock wait. Frame-pacing capture skipped because permission was absent.

## Brightness keys

BrightnessKeys1.0.4 is already enabled and loaded. The Dell firmware's SMEE/EV6/BRT6 path notifies GFX0.LCD with0x86/0x87. The new Darwin-only BRT6 wrapper routes these events to the existing PS2K keyboard using Notify0x0406(up) and0x0405(down). VoodooPS2 defines prefix4 as an extended make+break packet; extended05/06 map to native brightness-down/up ADB keys. The stock keyboard and touchpad binaries/profiles remain unchanged; no duplicate brightness kext is added. The original BRT6 is preserved as XBR6 and called for non-Darwin systems. The firmware SMEE brightness gate depends on Windows OSID detection, so the Darwin-only SMEE wrapper reads the same GENS event flags once and routes them without that Windows gate. It preserves the separate bit0x08/EV19 event path. The original SMEE is retained as XSME for other operating systems; no global OSID policy is changed.

The custom DSDT is loaded through ACPI/Add, after OpenCore's firmware patch phase. Therefore the already-added DSDT file itself receives two tightly scoped definition-only BRT6-to-XBR6 and SMEE-to-XSME replacements and a recalculated checksum. Its old SHA256 must match42d65940f44de1482427a022f254e86c9f5c5b40b360418c4aefbc33facec063 and the five-byte definition patterns must each occur exactly once. EV6 calls remain BRT6 and reach the added331-byte SSDT-LAT-BRT.aml. The only config change is that SSDT's enabled ACPI/Add entry. No BIOS firmware is flashed. The previous complete EFI is backed up before publication. Actual key-event delivery remains unverified on the Dell; Fn-lock state can determine whether Fn is needed.

## Publication changes

The old surface-out method held the IOSurface CPU lock through GPU readback and fence waiting. The new WindowServer path first prepares successfully completed pixels in a bounded three-slot CPU staging pool, then takes the IOSurface lock only for the final copy/unlock. Each slot is exclusive until that CPU copy completes. Try-lock acquisition never waits; if all three slots are busy, allocation fails or the snapshot exceeds16MiB, the existing synchronous path is used. Retained CPU staging is bounded to48MiB total, allocated on demand. There is one additional CPU memory copy in the staged path, so a target speedup is not assumed. Diagnostics separately record preparation, IOSurface lock wait, copy and unlock durations.

This is not an asynchronous replacement of WindowServer-owned IOSurfaces, a frame-dropping queue, an early-signaled completion or a bypass of display-server ownership. Event/fence completion still follows successful publication. No claim that this eliminates every CoreAnimation timeout is made.

## Wallpaper/full-frame refresh

The old physical front-buffer retry timer blindly recopied the source for one second, then stopped. That can miss a later completed publication and is a plausible explanation for content updating only after cursor activity. Accelerator0.2.6 retains the current surface and observes the existing NMXPending/NMXPublished completion markers. An incomplete new source keeps the previous physical scanout. A newer completed publication causes a full bounded front-buffer copy even without a new WindowServer transaction. After the active retry window, an unchanged tracked surface is checked at5Hz without repeated pixel copies. Source retirement stops monitoring; untracked legacy surfaces keep the bounded old retry behavior. New read-only counters distinguish late complete refreshes from held incomplete transactions.

The plugin also defers a surface's publication marker until its final write in a split command buffer. Otherwise an early part could mark the entire number published before a later part changed the same plane. These are concrete lifecycle corrections, but the Dell must confirm the wallpaper symptom is gone. The5Hz idle check can add up to about200ms to observing a publication after the active retry window; it is not the animation frame rate and does not claim tear-free/vblank-synchronized scanout.

## Weather/stride

No shared IOSurface stride is fabricated or forced to64/128bytes. The existing host-alias path already requires64-byte alignment; private transfer copies accept tight Vulkan rows and use the actual IOSurface plane stride, preserving destination padding. An arbitrary new shared pitch would change the layout contract. New bounded small-surface metadata records process, format, bytes-per-element, plane, dimensions, actual pitch and allocation size. This and separated publication timing are intended to isolate Weather's still-unconfirmed texture/format/synchronization path. Weather text corruption is not declared fixed by CPU layout tests.

## Validation

Three plugin variants compile, accelerator x86_64 links, signatures pass and old exports are preserved with no new plugin/accelerator imports. ASan/UBSan checks cover the actual three-slot helper (exclusive leases, saturation/reuse/cap and516 odd-width/padded R8/RG8/RGBA8/RGBA16 layouts), actual surface-out method (GPU preparation before IOSurface locking, failure cleanup, correct padding and synchronous fallback), actual publication-marker method (final repeated-plane part), and actual front timer/arm (incomplete hold, late completion, unchanged idle, retirement, legacy bound and disabled timer). Existing readback/cache/quarantine, swizzle and scheduled-completion fixtures pass. Installer checks include known previous accelerator acceptance and complete rollback. Collector captures the new counters and keyboard services. The full hardware suite is not rerun.

Brightness AML compiles without warnings, patched DSDT decompiles, exact byte differences/checksum and retained call sites are verified, and the config passes official ocvalidate1.0.8. This does not upgrade the bootloader. No target installation, arming, reboot or GPU execution is performed from the build host.

## Dell run

Boot the fallback desktop through the updated USB, then run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, reboot through USB. Try the printed brightness keys, with Fn if required; verify Control Centre still agrees. Change wallpaper, wait with the cursor stationary, then move/resize Weather and check its text. Collect on that active boot:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

If graphics fail, return through the fallback boot and run the same collector. Prior full EFI and replaced package files are saved under backups/publication-keys-before-0.3.15 on USB and locally. This is a package/EFI backup, not an OS/AuxKC snapshot; the target installer separately backs up installed runtime and AuxiliaryKC.

## Primary references

- [BrightnessKeys source and ACPI notification handling](https://github.com/acidanthera/BrightnessKeys)
- [VoodooPS2 keyboard event handling and native brightness mapping](https://github.com/acidanthera/VoodooPS2/tree/master/VoodooPS2Keyboard)
- [OpenCore ACPI processing order](https://github.com/acidanthera/OpenCorePkg/blob/master/Docs/Configuration.tex)
- [Vulkan buffer-image copy rows can be tightly packed](https://docs.vulkan.org/refpages/latest/refpages/source/VkBufferImageCopy.html)
- [Apple IOSurface plane row-stride query](https://developer.apple.com/documentation/iosurface/iosurfacegetbytesperrowofplane(_:_:))
