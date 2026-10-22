# ISH runtime candidate 0.1.1

The new LatitudeISHTransport is an actual Intel PCI driver, not a replay tool.
It implements IRQ/workloop ownership, IPC reset and busy-doorbell handling,
HBM negotiation/GUID enumeration/connect/credits, fragment reassembly, HOSTIF
HID descriptor discovery and descriptor-bound acceleration polling. The actual
driver source passes an ASan/UBSan firmware-emulation test from interrupt through
published acceleration properties. This does not prove Dell hardware operation.

Normal candidate boots enable **read-only preflight**. No IPC reset, MMIO writes,
PCI configuration writes, DMA enable, sensor power/feature changes or automatic
rotation occur in this mode. The collector records InitialFWSTS, HostComm, RMP2,
driver identity and errors. A device that is not already memory-enabled and D0
is refused. It may therefore be absent until its power ownership is implemented.

`latitude-ish-pio=1` is the explicit experimental transport gate. It is **not**
added to normal or recovery boot arguments. Do not turn it on until the Dell's
preflight is reviewed. Armed mode refuses active firmware DMA; requires the
exact captured 3951-byte sensor descriptor; requires feature report 3 to already
report D0; and only publishes fresh, advancing, bounded gravity samples. It
does not power the sensor, process aggregated input notifications, wake firmware
using DMA or automatically rearm after sleep. Those remain implementation gates.

Automatic rotation still requires real sensor samples plus a coordinated active
touch/pen transition and visible rotated-output validation. The manual rotation
tool checks CoreGraphics and IOFBTransform acknowledgements and rolls back on
failure; metadata acknowledgement alone does not establish correct pixels.

Next capture after booting the updated USB:

```
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/collect-tablet-macos.sh laptop
```

Repeat `tablet` while folded. Do not reinstall graphics or rerun the rendering
suite for these snapshots. The corrected HIDD table and conditional PS2 gates
are retained. Missing/stale qualified mode data leaves built-in input enabled.

Primary transport reference: Linux v6.19 intel-ish-hid PCI/IPC/ISHTP/HID sources
archived with hashes in research/tablet-linux-reference. Sensor descriptor and
axis scale come from this Dell's guided Linux capture, not another laptop.
