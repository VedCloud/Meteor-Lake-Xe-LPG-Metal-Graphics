# 20 October candidate: backend 4GiB, pipeline cache and Dell touchpad

Baseline is the verified usable-desktop-real-memory-0.2.5-20261019-232025 checkpoint. Its archive is preserved, not replaced by this unverified candidate. GPU kernel1.0.15, framebuffer0.2.5, accelerator0.2.4, IOKit transport and guarded HID1.0.1 stay unchanged.

## Backend limit and animation investigation

The user authorized a2GiB or4GiB backend limit;4GiB is selected as the first candidate. On Darwin integrated Intel8086:7d45, ANV's actual shared system heap is min(normal queried/policy size,4GiB). This is applied before creation of VkMemoryHeap records, not in the UI. Existing ANV memory budgets clamp to that heap and allocation checks use the same size/current accounted usage, including a raw-size check that prevents integer-wrap bypass of the aligned-size guard. The limit is per ANV physical-device heap/accounting domain, not a new kernel-wide aggregate quota or reserved4GiB physical allocation. Smaller actual heaps stay smaller. Other devices/platforms/local-memory heaps retain their policy. No environment override can raise this target's cap. GPU name remains queried automatically, and the existing root reporter should publish4096MB only if the backend actually exposes4GiB. Hardware query/allocator behavior is not yet validated on the Dell.

Yesterday's sample included pipeline/shader compilation during WindowServer composition. Plugin0.3.7 creates one device-lifetime Vulkan pipeline cache for this integrated GPU in WindowServer, using normal internally synchronized flags, and passes it to graphics pipeline creation. If unavailable/creation fails, the prior uncached path remains. No cache data is persisted to disk. It does not recycle command pools, change fresh-pool policy, relax barriers, alter submission timing or implement zero-copy. This may reduce repeated compilation; it does not establish a cure for the42 CoreAnimation synchronization/fence timeout lines or steady-state choppiness. The loader now captures both early and later stack samples so the next report can separate startup from persistent costs. Frame timings/FPS remain unmeasured.

## Touchpad port

Linux identifies the Dell touchpad as ELAN04F3:328E on00:15.3, Intel8086:7E7B, subsystem1028:0CB6, ACPI\_SB.PC00.I2C3.TPD0. Existing USB adds only I2C0 touchscreen matching. The Legion project supplies Arrow Lake controller matching/timing/power handling; it is not a gesture engine. Linux maps the Dell's7E7B to the same bxt I2C timing:133MHz, SDA hold42ns, SDA fall171ns, SCL fall208ns. Those are used for this exact Dell controller.

The installed VoodooI2C2.9.1 binary lacks the Legion personality's PCILakeController class, so blindly copying its codeless kext would fail. LatitudeI2CMeteorLake1.0.0 supplies a small subclass of the existing exported PCI controller. It restricts vendor/device/subsystem, discovers the PCI PM capability instead of assuming its offset, requests and verifies D0, preserves PME status semantics and refuses a nonresponsive controller before base probe. Nine required Voodoo base symbols resolve in the existing core. Existing VoodooI2C/HID/VoodooInput binaries and the HID null guard remain intact; only one VoodooInput stays enabled.

Captured macOS logs also show TPD0._DSM fails with missing HIDG. The replacement methods bypass the absent shared helper: the HID DSM returns firmware TPDH; _CRS concatenates the device's own firmware SBFB serial-bus and SBFI interrupt buffers. No slave address, descriptor register or GPIO pin is guessed. The paired DSDT modifies only the initial byte of this single TPD0's _DSM and _CRS names (XDSM/XCRS) plus table checksum; length is unchanged and it decompiles. A small SSDT supplies the new methods. _STA/_INI and touchscreen methods are untouched. Polling is enabled only on PCI15.3 through DeviceProperties, avoiding an unverified Meteor Lake GPIO interrupt path. No global polling boot argument is added. Existing PS/2 fallback input and keyboard are retained for the first trial; if duplicate pointer events occur, use the report to decide whether the legacy mouse should be disabled.

The new controller should allow the existing precision-touchpad HID and VoodooInput gesture stack to attach. Multi-finger gestures have NOT yet been tested on this laptop. The collector captures the controller/HID/precision-touchpad/VoodooInput registry chain and exact backend hash. Gesture settings or further HID/ACPI issues may still need work; no claim that installing this port alone proves all gestures.

## Local validation and recovery

ANV cross-build/link and3 plugin variants pass; backend imports/exports are identical to baseline, plugin prior exports remain and no new external imports appear. ASan/UBSan checks run the actual heap policy/allocation guard, pipeline cache helpers and controller power helper (65535 PMCSR words). ACPICA executes the actual SSDT against simulated firmware objects and verifies descriptor/support/resource bytes. Installer fixtures cover backend replacement and rollback on late signature/arming/KC failures; loader fixtures retain active/fallback behavior and isolate reporting failure. Collector fixtures preserve crash/panic data. These are local checks, not a full hardware rendering suite rerun.

USB touchpad rollback: /bin/bash /Volumes/SEQOUIA/iGPU-Port/restore-touchpad.sh from a Mac with USB mounted. It verifies the candidate config/DSDT before restoring the saved pre-trial config/DSDT and removing only the added touchpad bundle/SSDT. It refuses to overwrite a subsequently changed config. This restores USB input configuration; it does not revert an installed graphics backend. Desktop installer retains prior plugin/backend/kernel/cache/helper copies in its normal target backup directory and rolls back on installation failure.

## Dell trial

Boot through the updated USB to the fallback desktop, then run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, restart USB once without ResetNVRAM. Let the usable desktop settle for about a minute, check two-finger scrolling/pinch and three/four-finger gestures, then collect:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

If the graphics trial fails, power off and boot USB fallback, then collect before reinstalling. Bring the USB back. No22-case suite required. No target installation, live NVRAM changes or automatic reboot were performed on this build host.

## Sources

- [LegionI2CArrowLake](https://github.com/watchthelight-labs/LegionI2CArrowLake), pinned15ebc45d2e5aa63eebaff01a736acd402a7c3de1. Its ISC license and VoodooI2C's GPL license are preserved with the port. The new subclass is a local Dell adaptation, not the original codeless Arrow Lake package.
- [Linux LPSS PCI IDs and timing](https://github.com/torvalds/linux/blob/master/drivers/mfd/intel-lpss-pci.c).
- [VoodooI2C source](https://github.com/VoodooI2C/VoodooI2C), local pinned2.9.1 headers/controller/resource parser.
- [Vulkan pipeline caches](https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineCache.html) and [graphics pipeline synchronization](https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateGraphicsPipelines.html).
- [Vulkan memory properties](https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceMemoryProperties.html). ANV local source heap creation, budget calculation and allocation checks were audited and changed locally; no upstream community submission was made.
