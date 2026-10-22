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
