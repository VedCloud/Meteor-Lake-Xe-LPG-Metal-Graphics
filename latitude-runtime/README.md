> **30 October 2026:** This directory retains the historical Sequoia package. For current Tahoe 0.3.21 payloads and corresponding sources, see [packages](../packages/). Do not install Sequoia binaries onto Tahoe.

# Latitude 5350 acceleration test

This is a complete experimental render-runtime build for the physical Meteor Lake 8086:7D45 GPU, not just the PCI diagnostic. It has not yet executed on the Dell. A successful build is not proof of hardware acceleration.

## Run on the Dell

1. Boot Sequoia through this USB first. The updated OpenCore config supplies the required SIP mask, AMFIPass and driver-loading settings.
2. In Terminal, install the runtime:

```bash
sudo bash /Volumes/SEQOUIA/iGPU-Port/Acceleration/install-runtime.sh
```

The installer first checks the machine, SIP, AMFIPass, package hashes and both kexts against the Dell's actual kernel collections. If kernel validation fails, it leaves the live driver files alone and saves a report on the USB. Do not bypass that failure; bring the report back.

3. If installation succeeds, reboot through the same USB. The driver has a one-boot opt-in: the NVRAM variable is consumed before GPU initialization. There is no persistent nmxintel boot argument.
4. After login, run:

```bash
sudo bash /Volumes/SEQOUIA/iGPU-Port/Acceleration/collect-runtime.sh
```

Reports are written directly to iGPU-Port/Acceleration/reports on the USB. Vulkan must identify the actual Intel integrated device 8086:7d45, submit a compute shader and return 65,536 correct values. Metal must select a device attached to NMXAccel, compute correct values and render 4,096 correct pixels. A device name, reported VRAM, or successful kext load alone is not considered a pass. The tests run as the logged-in user even when the collector uses sudo for the kernel logs.

If the install fails, bring the install report back from the same reports directory. If the boot hangs or panics, power-cycle and boot through the USB again. The one-shot opt-in should already be consumed, and the driver also retains its attach-failure marker. Do not repeatedly re-arm it after a failure. Capture the next boot's reports and panic report for diagnosis.

## Roll back

On a boot that reaches Terminal:

```bash
sudo bash /Volumes/SEQOUIA/iGPU-Port/Acceleration/rollback-runtime.sh
```

The installer saves the old runtime and Auxiliary KC under /Library/NullMoth/latitude5350-backups and records the latest path in /Library/NullMoth/latitude5350-last-backup.txt. Rollback restores those files, removes the one-shot opt-in, and requires a reboot. A failed install attempts the same rollback automatically. The older MyIntelGPU.kext that failed authentication in your supplied reports is moved into the backup during a successful install.

USB settings are backed up separately under iGPU-Port/backups. The before-sip-change config restores the previous security settings while retaining the successful PCI diagnostic and GPU unhiding. Copy the intended backup config to EFI/OC/config.plist from a working system and boot through that USB to apply it. The original-before-unhide config also restores the old GPU-hiding entry.

## What was built

- NMXGPU, restricted to real PCI 8086:7D45: 438 i915, DRM/TTM, LinuxKPI, compatibility and IOKit source files compiled. AMD and Xe drivers are not linked into this laptop build. The AMD-only display initialization reference is conditionally excluded; no driver-internal C symbols remain unresolved at link.
- NMXAccel for Sequoia with the supplied VM repository's generic fixes and DRM user-client transport. No virtio display kext is installed.
- NVMTLDriver Metal plugin, AIR-to-SPIR-V shader translator, libdrm, and ANV Intel Vulkan driver, all built for x86_64 from the specified patch bases. ANV completed 1,045 build steps. CLC tools were separately built for the Apple Silicon build host.
- Meteor Lake DMC 2.23, GuC 70.53.0, HuC 8.5.4 and GSC firmware matching the versions in your Linux report. Firmware source URLs and hashes are recorded in BUILD-MANIFEST.json.
- Vulkan and Metal output tests, target-kernel validation, install/rollback and a local report collector. Code signatures verify ad hoc; target loading still depends on the updated OpenCore configuration and successful kernel validation.

The rebuilt driver uses the i915 physical-GPU path, not Panther Lake SR-IOV or the QEMU virtio scanout path. The supplied repository's reusable fixes were applied at its specified NullMoth, drm-kmod, libdrm and Mesa bases.

## Limits of this candidate

The immediate goal is real hardware rendering and compute in applications. The current NMXAccel physical-display path is headless from the Metal plugin's perspective, and the plugin deliberately declines WindowServer on that path. Therefore this candidate does not yet provide GPU-composited WindowServer on the laptop panel. Revision 1.0.2 uses the i915 no-display discovery path and skips the DRM-to-IOFramebuffer bridge, leaving the panel to its existing firmware framebuffer while the render runtime is tested.

Full desktop composition, reliable sleep/wake, brightness, external displays, video decoding and long-run stability are unverified. Passing the included GPU-output tests establishes application compute/render acceleration only, not those features. More hardware iterations may be needed even to reach the first output pass.

## Changes already made to this USB

GPU-hiding DeviceProperties removed; LatitudeMTLProbe retained; csr-active-config set to Data 430A0000 (0x0A43, the driver-required custom SIP mask); csr-active-config and boot-args added to NVRAM/Delete so previous values are replaced; SecureBootModel Disabled; AMFIPass 1.4.1 enabled after Lilu; amfi_get_out_of_my_way=0x1, amfi=0x80 and ipc_control_port_options=0 added per the NullMoth setup. These security changes take effect on the next boot through this USB. No runtime was installed on the Apple Silicon build Mac.

Original reference repositories and source checkouts remain in /Users/ved/Documents/iGPU Port on the build Mac. BUILD-MANIFEST.json identifies each revision. Local patches and build logs are retained in port-artifacts there. The supplied reports identify Sequoia 15.8.1, build 24H32, and confirm the diagnostic already attached to the real GPU.

## Revision 1.0.1 — first target boot fixes

The 14:06 target report did not reach GPU initialization: NMXGPU failed because its NMXIrq metaclass collided with the installed NMXNPU, and the user-mode Vulkan test received errno 13 opening ANV. This revision renames the GPU helper to LatitudeMTLGPUIrq, preserving the NPU, and grants GPU clients read/traverse permission on runtime trees after copying from FAT32. The installer retains the existing backup and one-shot mechanisms. Re-run install-runtime.sh, reboot through this USB, and run collect-runtime.sh again. The collector now also records file permissions and GPU boot markers. There is still no successful GPU-output result.

## Revision 1.0.2 — render-only initialization

The 14:12 report confirms NMXGPU 1.0.1 started, recognized Meteor Lake, loaded DMC 2.23 and delivered an interrupt. It then logged a panel power-sequence timeout and device initialization failed with -102. The panel timeout function only logs an error, so that message alone does not establish the precise cause of the fatal return. Vulkan loaded its library successfully but found no device; Metal was skipped.

Revision 1.0.2 skips display discovery using i915's existing no-display path for this exact PCI ID under NMXKPI and skips the NMX framebuffer bridge. It also logs the initialization phase before each major probe step and includes the failing phase in the error. This is the agreed application-rendering-first experiment, not a panel sequencing fix. No GPU-output pass has been established. Re-run the installer, reboot through the USB, and collect again.

## Revision 1.0.3 — enable the Meteor Lake GuC profile

The 14:18 report shows 1.0.2 attached and NMXAccel started, but GT0 was declared wedged after uC initialization returned -5. Vulkan loaded ANV but could not obtain usable device information. The inherited FreeBSD tree defaults enable_guc to 0 and suppresses Linux's automatic GuC defaults. Revision 1.0.3 sets enable_guc=3 only for 8086:7D45 under NMXKPI, matching the working Linux report, and logs the uC policy/state. It retains render-only display behavior and the one-boot opt-in. This enables the intended firmware path; its hardware success is not yet established. The new i915-info.txt report records individual capability and topology-query results. Reinstall, reboot through the USB, then collect reports as before.

## Revision 1.0.4 — i915 user-space memory mapping

The 14:27 report confirms GuC 70.53.0 loaded, submission/SLPC/RC enabled, all recorded DRM queries succeeded, and Vulkan enumerated Intel MTL 8086:7D45. Device creation returned VK_ERROR_MEMORY_MAP_FAILED (-5), before execution. The i915 ANV backend still used native mmap for DRM memory, whereas the patched Xe backend used the Darwin mapping bridge. This revision routes i915 through the same existing bridge. NMXGPU stays at kernel version 1.0.3. The test now captures Vulkan debug-utils errors and the collector saves kernel logs after testing. Hardware execution remains unverified. Reinstall, reboot through this USB, and collect again.

## Revision 1.0.5 — isolate the remaining mapping failure

The 14:34 target report still fails at vkCreateDevice with MEMORY_MAP_FAILED. Revision 1.0.5 is diagnostic: ANV reports whether the mmap-offset ioctl or the Darwin mapping ioctl fails, with numeric errno, flags and sizes. The i915-info tool creates its own temporary GEM objects at 4 KiB, 64 KiB and 2 MiB, requests WB and WC offsets separately, maps through libdrm, checks CPU write/read access, and releases mappings and objects. CPU readback does not demonstrate GPU execution. No kernel revision or cache policy changes are included. Reinstall, reboot through this USB, and collect again.

## Revision 1.0.6 — advertise supported mmap-offset capability

Report 14:42 proves temporary GEM objects can be mapped and read/written through the Darwin bridge in WB and WC modes at all three tested sizes. Vulkan still fails without either new offset/bridge failure log. Source inspection identifies why: nmxkpi_prelude.h defines FreeBSD version 1500000, causing i915_gem_mmap_gtt_version to report 3. Mesa enables has_mmap_offset only for version >=4, so it selects the disabled legacy mmap ioctl instead of the bridge path added in 1.0.4. The local i915 fork now returns 4 under NMXKPI. It does not advertise partial-map version 5. NMXGPU is rebuilt as 1.0.6; the diagnostic now records mmap_gtt_version. Hardware Vulkan/Metal execution remains unverified. Reinstall, reboot through the USB, then collect reports.

## First Vulkan execution pass — report 20261018-145313

NMXGPU 1.0.6 reports mapping version 4. Vulkan executed on 8086:7D45 and verified all 65,536 output values (seed 528003493). This is the first successful GPU workload. Metal enumeration failed while MetalPluginName remained withheld: the driver attached at 14:52:43 and the report started at 14:53:13, before its 60-second headless restoration. The collector now waits up to 75 seconds for MetalPluginName before launching the Metal test in a fresh process and records readiness separately. This change does not rebuild or replace the kernel. Metal rendering and physical-panel composition remain unverified. If still booted with NMXGPU active, re-run collect-runtime.sh after one minute; no reinstall is needed for the collector change. Otherwise run the existing installer to arm the next GPU boot, reboot through USB, and collect.

## Vulkan and Metal output pass — report 20261018-145513

Vulkan compute passed all 65,536 values (seed 380845299). Metal enumerated the real NMXAccel-backed Intel MTL device (registry 0x100000539), passed all 65,536 compute values (seed 96391575), and rendered a triangle whose 4,096 output pixels matched expected RGBA values. All three diagnostic processes exited 0. These results establish the tested application GPU compute/render paths; they do not establish accelerated WindowServer composition, broad application compatibility, sleep/wake or stability. The readiness timeout was a collector matching bug: MetalPluginName contains ../../../Library/GPUBundles/NVMTLDriver, while the collector expected only NVMTLDriver. Its check now accepts the path and is verified against the captured IORegistry report. No runtime library or kernel changes are included in this collector correction.
