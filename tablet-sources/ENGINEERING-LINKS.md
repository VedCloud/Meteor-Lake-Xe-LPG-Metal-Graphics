# Implementation links verified against source

## ISH transport
This Dell exposes 8086:7e45 at PCI 00:12.0. Linux maps PCI BAR0 and implements
IPC -> ISHTP/HBM -> HID client -> HID sensor reports. This is not the touchpad's
I2C3 or Wacom's I2C0. Correct HID client GUID is
33AECD58-B679-4E54-9BD9-A04D34F0C226, matching the captured sysfs devices.

Pinned Linux v6.19 primary files and hashes are in
research/tablet-linux-reference/SOURCES.json in the local workspace. The register
layout specifies FWSTS at 0x34, HOST_COMM 0x38, HOST2ISH doorbell 0x48, ISH2HOST
0x54, inbound messages 0x60 and outbound messages 0xe0, relative to BAR0. These
are a reference map, not permission to read/write an unowned or powered-down BAR.

latitude_ishtp_frame.h implements bounded PIO fragment framing/reassembly and
HOSTIF framing plus this descriptor's signed accelerometer payload decoding.
Tests cover fragmented 136-byte sensor reports, overflow and malformed lengths.
LatitudeISHTransport.cpp plus latitude_ish_pio.h, latitude_ishtp_hbm.h and
latitude_ish_hid.h now implement PCI mapping/IRQ ownership, IPC reset, HBM
connect/credits and HID descriptor-bound polling. The actual driver interrupt
path passes firmware emulation through published gravity properties. Receive
credits return after consuming a complete message, not each fragment. See
ISH-RUNTIME.md for the read-only default and still-missing DMA wake, sensor
power, resume and Dell hardware validation. No live Dell stream is established.

Next driver work must verify each stage through an explicit bounded state
machine with timeouts and reset generations, then publish a qualified live
state heartbeat. It must not enable tablet input suppression from the passive
observer's old event string. That observer does not call firmware methods to
enable notifications and has not yet been boot-validated.

## Coordinated display rotation
The proposed CGConfigureDisplayMtx/kCGDisplayRotate90 call is not declared by the
installed CoreGraphics SDK. Public CGBegin/CGComplete configuration transactions
exist, but do not establish that this rotation setter exists. Do not compile or
invoke an invented private symbol. Private API use needs a verified signature
and runtime availability checks against the Dell's actual Sequoia frameworks.

Apple’s published IOFramebuffer source implements requestProbe(kIOFBSetTransform)
through selectTransform/setTransform and a connection-change notification. This
is a concrete implementation lead; return-success alone does not prove the
Sequoia/NullMoth presentation path has accepted a new geometry.

NMXVGAFB inherits IOFramebuffer transform/requestProbe. An absent override does
not establish lack of support. Physical rotated output remains unvalidated.
It validates firmware landscape timing and physical scanout size. Changing a
registry property or swapping width/height alone is not a working rotation path.
The presentation copy, row bytes, cursor coordinates, display mode/timing metadata
and acknowledgement must stay consistent, without reprogramming panel timings
as if a portrait surface were the physical landscape panel.

The intended transaction is: verify support on the built-in display only;
release/cancel active touch, pen and drag state; prepare the new framebuffer and
touch transform; request display rotation; wait for actual display/framebuffer
acknowledgement; commit the matching Wacom transform; then accept new contacts.
On failure, restore the old display AND touch state. Do not affect external
monitors or leave a rotated panel with an old touch matrix.

## Touch and pen
The working VoodooI2CHID source already has checkRotation for touch, pen and
scroll coordinates, using swap/invert flags derived from IOFBTransform. Its
existing missing-property null guard must be retained. Do not apply a second
rotation to raw reports and again in checkRotation.

LatitudeTouchTransform.h is a tested integer reference for normalizing rectangular
raw axes before translating/rotating around the top-left origin. Clockwise 90°
is (u,v)->(1-v,u), 180°=(1-u,1-v), 270°=(v,1-u). A bare rotation matrix on
unnormalized raw coordinates would produce negative/out-of-range positions.
Tests cover all corners, nonzero minima, rectangular ranges and integer limits.
Physical orientation -> framebuffer angle/flags still needs hardware validation.

## On-screen keyboard
The compiled session app has a manual nonactivating panel and an automatic focus
path gated on a future qualified live mode publisher, with stale-state rejection.
Only AX role, enabled state and value-settable status are queried; field contents
and physical key events are not read. A session app cannot provide FileVault or
pre-login input. Manual panel behaviour and injection need Dell validation after
Accessibility permission is granted. Automatic popup is NOT enabled yet because
the live mode publisher has not yet been validated on the Dell. The 0.2.1
reader and conditional PS2 hooks now exist; V3 fixes their hidden HIDD provider.

Sources:
- https://github.com/torvalds/linux/blob/v6.19/Documentation/hid/intel-ish-hid.rst
- https://github.com/torvalds/linux/blob/v6.19/drivers/hid/intel-ish-hid/ipc/pci-ish.c
- https://github.com/torvalds/linux/blob/v6.19/drivers/hid/intel-ish-hid/ipc/hw-ish-regs.h
- https://github.com/torvalds/linux/blob/v6.19/drivers/hid/intel-ish-hid/ishtp/hbm.h
- https://github.com/torvalds/linux/blob/v6.19/drivers/hid/intel-ish-hid/ishtp-hid.h
- https://developer.apple.com/documentation/coregraphics/quartz-display-services
- https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebuffer.cpp
