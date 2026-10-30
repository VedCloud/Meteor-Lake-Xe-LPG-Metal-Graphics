# Native recovery display bridge — isolated v13 experiment

The v12 boot reached securityd's native receive loop and recovery's loginwindow
service loading. Its final real 1280×800 display capture remained black after
169,350,012,907 scheduled slots. It was stopped through its owned QEMU monitor;
that controlled stop is not a guest panic or a completed installer boot.

This candidate adds a guest-only LoginWindow/Aqua launch agent. It attempts to:

1. Open the actual native `IOBootFramebuffer` server user client, query its mode,
   and map the native VRAM descriptor using Apple's documented memory-type value.
2. Create a native `CGVirtualDisplay` at that framebuffer's measured dimensions.
3. Request real WindowServer-produced BGRA IOSurfaces and copy complete rows into
   the firmware aperture, respecting independent source/destination strides.

This is an experimental **software presentation path**, not a Metal driver or
hardware acceleration. Native rendering, capture permission, agent loading and
actual presentation still need runtime verification. No installer bitmap, fake
completion or fabricated framework result is supplied.

## What was checked

- The exact recovery cache contains the virtual-display classes and the legacy
  CoreGraphics/SkyLight capture exports. It does **not** contain ScreenCaptureKit;
  the candidate therefore has no ScreenCaptureKit link dependency.
- All eight linked images are present in that recovery cache.
- Runtime Objective-C signatures are checked before calling private methods.
  Read-only host inspection caught the mode constructor's 32-bit width/height
  arguments and passed after correcting the declarations. Guest signatures must
  pass independently; host inspection is not a guest result.
- Forty-five real, host-allocated IOSurface copies passed ASan/UBSan checks with
  independent padded strides, destination guards and invalid-dimension rejection.
  These are memory-copy tests, not installer rendering or full presentation tests.
- The helper refuses normal host execution without its guest marker. Actual guest
  execution also requires the native VirtIO block device. No host display was
  created or opened during inspection/tests.
- Native framebuffer open/map and capture failures remain failures. No permission
  override or synthetic success is returned. The copy checks pixel format, row
  size, dimensions and accessible allocation size before touching image data.
- The owned COW HFS candidate passes filesystem verification. Added helper,
  marker and launch-agent files have root ownership; parent metadata is restored
  exactly. Existing Apple executable bytes, original images, the published
  baseline and the Tahoe EFI are unchanged.

`VALIDATION.json` pins the EFI, helper, initialized-memory input and own source
hashes. Firmware admission and the positive native boot are separate tests; inspect
that record for their current status. **No native frames or graphical installer
have been verified for v13.** The separate frame-comparison optimization is not
included, keeping this comparison focused on the guest presentation bridge.

## Developer use

`build.sh` builds/ad-hoc-signs the ARM64 helper with the local macOS27 SDK.
`--inspect` checks class/method metadata only. Do not install the guest agent on a
normal host. `prepare-display-bridge-profile.py` is an original-workspace recipe
for checked private recovery inputs, not a portable end-user installer.
Apple images, SDKs, initialized memory and trust data are not redistributed.

References: [Apple IOFramebufferUserClient](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebufferUserClient.cpp),
[Apple memory-type definition](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOKit/graphics/IOGraphicsTypesPrivate.h),
[DeskPad virtual-display usage](https://github.com/Stengo/DeskPad/blob/main/DeskPad/Frontend/Screen/ScreenViewController.swift).
