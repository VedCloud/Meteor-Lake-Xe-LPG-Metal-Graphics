# Meteor Lake Xe-LPG Metal Graphics for macOS

[![License](https://img.shields.io/badge/License-Mixed%3B_see_LICENSE-lightgrey)](LICENSE)
![macOS](https://img.shields.io/badge/macOS-Sequoia_15.8.1_%2B_Tahoe_26.7.1-blue)
![Architecture](https://img.shields.io/badge/Graphics_port-x86__64-informational)
![Bootloader](https://img.shields.io/badge/Bootloader-OpenCore_1.0.9-9cf)
![Hardware](https://img.shields.io/badge/CPU-Intel_Core_Ultra_5_135U-orange)
![Graphics](https://img.shields.io/badge/GPU-Meteor_Lake_Xe--LPG_8086%3A7D45-red)
![Firmware](https://img.shields.io/badge/GuC-70.53.0-blue)
![APIs](https://img.shields.io/badge/APIs-Metal_%2B_Vulkan-purple)
![Backend](https://img.shields.io/badge/Backend-Mesa_ANV-purple)
![Kernel](https://img.shields.io/badge/Kernel-i915_on_LinuxKPI-red)
![Desktop](https://img.shields.io/badge/WindowServer-GPU_composition_working-green)
![Tahoe baseline](https://img.shields.io/badge/Tahoe_graphics_baseline-0.3.21-blue)
![Application validation](https://img.shields.io/badge/Sequoia_application_gate-26%2C000_frames-green)
![macOS 27 research](https://img.shields.io/badge/macOS_27_research-native_WindowServer_startup-orange)
![Installer status](https://img.shields.io/badge/macOS_27_installer-not_yet_visible-yellow)

**Updated 30 October 2026.** This project ports Metal and Vulkan graphics to the
Intel Meteor Lake Xe-LPG iGPU (`8086:7D45`) in the **Dell Latitude 5350 2-in-1**.
The laptop now runs **macOS Tahoe 26.7.1**, following the working Sequoia 15.8.1 port.
The restored Tahoe graphics checkpoint uses **NVMTLDriver 0.3.21**. Performance and
peripheral work continue; this is an experimental hardware port.

![Dell Latitude 5350 2-in-1 running macOS Tahoe 26.7.1, with the custom Dell icon and About This Mac window](Tahoe-26.7.1-desktop.png)

*Latest Dell screenshot, supplied 30 October 2026. About This Mac shows the custom
Dell identity, 16 GB RAM and the requested “Intel Iris Xe Graphics” label with
4096 MB reported shared graphics memory. The hardware remains Meteor Lake
**Xe-LPG**; the display label does not change its PCI identity. Shared memory is
not dedicated VRAM, and a screenshot alone is not a rendering benchmark.*

## Contents

- [Current hardware status](#status)
- [Graphics validation](#validation)
- [Hardware and compatibility](#target)
- [Architecture](#architecture)
- [Components and OS differences](#components)
- [Separate macOS 27 runtime progress](#macos27)
- [Building, installation and diagnostics](#build)
- [Remaining work](#roadmap)
- [Attribution and licensing](#licensing)

<a name="status"></a>
## Current hardware status

Status is scoped to the tested Dell and its working profiles. A passing local
build or test fixture is recorded separately from a hardware result.

| Area | Current status |
|---|---|
| **Metal / Vulkan applications** | Verified GPU execution on Sequoia; the graphics pipeline has also been ported to the installed Tahoe system. The historical application gate is detailed below. |
| **WindowServer desktop** | GPU-composited desktop working on the Dell. The Tahoe checkpoint uses 0.3.21, including A8 alpha-mask, blending and render-view corrections. Smoothness, glyph artefacts and regressions remain under investigation. |
| **GPU identification and memory** | Real PCI identity `8086:7D45`, Dell subsystem `1028:0CB6`. The backend uses a 4 GiB shared-heap policy; About This Mac uses the user-requested graphics label. |
| **Wi-Fi** | Custom Meteor Lake AirportItlwm works, including the Tahoe installer and installed system. This does not establish AWDL or AirDrop. |
| **Bluetooth** | Basic Bluetooth connections work. Full Continuity and AirDrop are not established. |
| **Audio — ALC295** | Working. Sequoia used AppleALC; Tahoe uses the separate SimpleHDA path. Keep `keepsyms=1` for SimpleHDA. |
| **Brightness / Night Shift** | Panel brightness and Night Shift have been confirmed. Keyboard brightness helper and slider/HUD synchronization still need qualification on the retained profile. |
| **Touchscreen** | Confirmed on working profiles and in the Tahoe installer. Touchpad trials have caused input regressions; their results are not promoted to the stable profile. |
| **Touchpad / gestures** | Unresolved. Trials caused lag, erratic movement and built-in keyboard freezes. Another Meteor Lake laptop's working EFI is a reference, not proof of Dell compatibility. |
| **Copilot → Command** | Confirmed by the user. Retain the VoodooPS2Keyboard macro inversion in every active and recovery EFI. |
| **Boot UI / chime** | GoldenGateExt theme and boot chime confirmed. Auxiliary tools are hidden until Spacebar. Quiet boot and Windows/Fedora dual-boot behavior have been confirmed. |
| **Pen** | Bluetooth/hover have been observed, but nib input is not qualified. The user also reproduced nib failure outside macOS; it is not established as a graphics-driver defect. |
| **Sleep/wake** | Not qualified; the reported stay-awake behavior remains open. |
| **AWDL / AirDrop** | Not functional. Native peer-data transport and channel scheduling remain unfinished. |
| **Tablet mode / rotation / on-screen keyboard** | Linux sensor/switch evidence and local implementation tests exist. A qualified live macOS sensor/mode publisher and complete coordinated behavior remain unfinished. |
| **System-wide FreeType text** | Renderer, shaping/frontend and some native interception tests pass. Complete system-app and GPU-backed layer integration is unfinished. |

The custom Dell icon and About This Mac branding are cosmetic. They do not prove
acceleration, a dedicated graphics-memory allocation, or compatibility with another laptop.

<a name="validation"></a>
## Graphics validation

The strongest recorded application result remains the **Sequoia** report
`readback-20261019-114424`:

- **22 cases:** 11 through IOKit and 11 through the direct transport.
- **26,000 exact frames**, including padded buffers, textures, multiple queues and IOSurfaces.
- **80,020 command pools created and destroyed**, with zero live, pending or quarantined pools at completion.
- Vulkan compute verified **65,536 values**; Metal rendering verified **4,096 pixels**.

See [the dated full-gate report](research/verified-0.3.1-full-gate-20261019.md).
These counts describe that application test, not a new Tahoe run or a WindowServer soak test.

Desktop composition is a separate result: WindowServer submits real GPU work and
readback/presentation must be checked independently. The previously measured
**5.0 ms average readback is one stage**, not total frame time or a guarantee of
60 Hz. Publication stalls, fence timeouts and visible artefacts require their own
logs and visual checks. An installer exit code, GPU name, transparency or a clean
screenshot does not replace that evidence.

The fresh-command-pool policy is retained: pools are released after confirmed
fence completion; uncertain completions are quarantined rather than recycled.

<a name="target"></a>
## Hardware and compatibility

| Item | Tested target |
|---|---|
| Laptop | Dell Latitude 5350 2-in-1, 2024, 13.3-inch panel |
| CPU / RAM | Intel Core Ultra 5 135U; 16 GB RAM |
| GPU | Meteor Lake Xe-LPG, PCI `8086:7D45`, subsystem `1028:0CB6`, stepping B0 |
| Audio | Realtek ALC295 on Intel controller `8086:7E28` |
| Wi-Fi | Intel Meteor Lake `8086:7E40` with custom AirportItlwm |
| Touch input | ELAN04F3:328E touchpad and I2C-HID touchscreen; controller `8086:7E7B` |
| Installed/tested macOS | Sequoia 15.8.1 (`24H32`) and Tahoe 26.7.1 (`25G241`), x86_64 |
| Tahoe recovery ABI reference | 26.6.2 (`25G83`); not a claim that every Tahoe build is supported |
| GPU firmware reference | GuC 70.53.0, DMC 2.23, HuC 8.5.4, GSC 102.1.15.1926 |

Other laptops and PCI IDs are **untested**. Xe2/Xe3, Panther Lake SR-IOV and
flat-CCS fixes must not be copied blindly onto Meteor Lake (`has_flat_ccs=no`).
A device-ID injection alone does not implement a hardware port.

<a name="architecture"></a>
## 🧭 Architecture

```
Metal app
   │
   ▼
NVMTLDriver.bundle          ← out-of-tree Metal driver plugin (NSBundle-loaded user client)
   │  • NVMTLDevice / NVMTLAccelerator  (IOKit-facing provider subclasses)
   │  • NVMTLObjects.m                   (resource, texture, sampler, surface lifetime)
   │
   ▼
libnvmtl_translate.dylib    ← Apple AIR → SPIR-V shader translator
   │  (based on metal2vulkan; LGPL-3.0 / GPL-3.0)
   │
   ▼
ANV (Mesa Vulkan)           ← Intel Vulkan backend, built from Mesa 26.3.0-devel
   │  patched for the i915-on-xnu path
   │
   ▼
libdrm + xf86drm.c          ← userspace DRM abstraction, rebuilt as a pure IOKit render-node model
   │
   ▼
NMXAccel.kext               ← DRM user-client transport / accelerator (com.nullmoth.NMXAccel)
   │
   ▼
NMXGPU.kext                ← 438 i915 + DRM/TTM + LinuxKPI + compatibility + IOKit sources
   │  (com.nullmoth.NMXGPU, personality NMXGPUIntel)
   │
   ▼
Xe-LPG hardware  (GuC-managed engine, GGTT, DMC/HuC/GSC firmware)
```

Two transports are exercised and both matter: **direct** (the DRM device-file interface) and **IOKit**
(the render-node transport). The gate runs 11 cases on each route. A call named `drmNmxIoctl` does not
prove IOKit was used, so each case logs and verifies the route it actually took.

<a name="components"></a>
## Components and OS differences

The port retargets the [NullMoth](https://github.com/nullmoth/nvidia-macos-driver)
architecture: Intel GuC and i915/LinuxKPI replace the NVIDIA-specific kernel path,
Mesa ANV replaces NVK, and the Metal plugin translates AIR to SPIR-V.

| Component | Role / current scope |
|---|---|
| `NMXGPU` | i915, DRM/TTM and LinuxKPI on xnu; stabilized kernel baseline 1.0.15. |
| `NMXAccel` / libdrm | IOKit DRM transport. The Tahoe bridge has OS-specific private-family forwarding; it is not just an OpenCore version-range change. |
| `NVMTLDriver` | Metal plugin. Working Tahoe checkpoint: 0.3.21. Older 0.3.19 source/build references in this tree remain historical Sequoia material. |
| `LatitudeMTLFB` / panel claim | Panel/framebuffer integration, reporting, gamma and brightness. Preserve the passing hardware configuration. |
| ANV / translator | Intel Vulkan backend and AIR-to-SPIR-V translation. |
| AirportItlwm | Meteor Lake PCI match, API-83 firmware, fragmented PNVM loading, internal auxiliary-station and receive Block Ack changes. Browsing works; AWDL remains separate. |
| Audio | AppleALC/Lilu for the original Sequoia configuration; SimpleHDA for the Tahoe configuration. |
| Input | VoodooI2C/HID/GPIO and PS/2 components; preserve working keyboard, touchscreen and Copilot mapping while testing the touchpad. |
| Boot | OpenCore 1.0.9, OpenCanopy/AudioDxe, GoldenGateExt and hidden auxiliary entries. |

**Quiet Tahoe audio:** `keepsyms=1` is required by the local SimpleHDA driver.
It retains kernel symbols and does not enable visible verbose boot. Removing it
broke the audio startup prerequisite. Do not remove it while quieting the EFI.

Normal accelerated profiles retain the OpenCore Delete+Add refresh of
`nullmoth-gpu-once` and `nullmoth-panel-once`; an explicit recovery profile and the
attach-failure guard remain available. Historical backups stay unchanged.

The local Tahoe installer validates the actual IOAcceleratorFamily2,
IOGraphicsFamily, IOSurface and AppleGraphicsDeviceControl identities and the
resulting kernel collection. Sequoia and Tahoe packages are not interchangeable.
Some later Tahoe packaging and macOS 27 experiments are maintained outside this
checkout; this status update is not a claim that all those binaries or builders
have been published here.

<a name="macos27"></a>
## macOS 27: substantial runtime progress, installer still pending

This is a **separate experimental ARM64-on-Intel firmware runtime**, targeting
the supplied macOS 27 build `26A428`. It does not turn the existing Intel Tahoe
EFI into a macOS 27 installer. ARM instructions execute through a software
translation engine beneath the guest kernel.

The work has advanced well beyond the earlier scalar preflight:

- **Native Darwin 27 kernel execution**, pmap startup and HFS recovery-root bring-up.
- **Native launchd, recoveryosd, OpenDirectory, LaunchServices and CoreServices startup.**
- **Native WindowServer startup.** Earlier baseline captures also show its CoreAnimation/HID threads, session setup and IOSurface connection initialization.
- **16 GiB logical guest RAM**, with bounded sparse backing and firmware-supplied entropy.
- Implemented and tested MMU/COW, exception, PAC, timer/interrupt, PCI, VirtIO storage, AES, RTC, GPIO and framebuffer paths. The cipher tests include **4,096 QARMA comparisons**, **72 monitor/PAC cases** and **768 SPRR checks**.
- The compiled x86 UEFI engine runs in OVMF tests and rejects a modified initialized-memory capture. These are real execution/admission tests, not a substitute for an installer screen.

The current performance candidate combines bounded word-wise memory copy/fill,
a faster read-only diagnostic filter and suppression of unchanged firmware-frame
copies. Its firmware/CPU and modified-input tests pass; a valid-image boot is
running independently with persistent run-state and exit-code records. The
latest run has reached native WindowServer startup again. No completed desktop
performance claim follows from those component tests.

**The graphical installer has not been verified:** captured screens remain
black during startup. Installer keyboard/pointer input, physical target-storage
access and installation on the Dell are not qualified. Native WindowServer
processes are progress, but they are not proof that an installer is usable.

The working Intel Tahoe EFI remains separate and preserved. An isolated diagnostic
loader and its inputs were added alongside it on USB; that is not a bootable
installation release. [`macos27-standalone/`](macos27-standalone/) retains the
earlier preflight source; the latest full-engine experiments run from the
Corsair-backed build workspace and are not all represented by that older folder.

<a name="build"></a>
## Building, installation and diagnostics

Start with [the runtime documentation](latitude-runtime/README.md),
[desktop documentation](latitude-desktop/README.md) and dated [research](research/).
Use the matching OS/build package and its manifest; preserve a working EFI and
collect the failed boot's reports before reinstalling.

The original Sequoia build pipeline is translator → libdrm/ANV → Metal plugin →
i915/LinuxKPI and accelerator/framebuffer kexts → package validation → hardware
checks. Build scripts under `desktop-sources/` include version-specific historical
builders. The kernel compile scripts still contain machine-specific paths and
are not a portable one-command build. A script named for 0.3.19 does not reproduce
the later Tahoe 0.3.21 checkpoint.

Tahoe's local combined graphics/audio package performs exact family/ABI checks,
a temporary kernel-collection preflight, backup and final-cache verification.
Do not bypass a failed validation or install an old Sequoia package onto Tahoe.
The current screenshot does not establish that every repository build script
matches the currently installed laptop.

For the retained Dell installation, ordinary desktop-failure collection is used
for WindowServer, presentation, driver identity and peripheral diagnostics. The
historical 26,000-frame application gate is not rerun merely to collect a desktop report.

The experimental setup disables portions of SIP/AMFI and loads custom kernel
code. Installation, recovery and rollback instructions are package-specific;
keep backups and a known-good boot path. No installer or test script should
reboot the Dell automatically.

<a name="roadmap"></a>
## Remaining work

1. **macOS 27:** finish the native display/renderer path, verify actual graphical
   recovery/installer output, then qualify input and target storage on the Dell.
2. **Tahoe graphics:** continue smoothness, frame-time, glyph and regression work
   against the retained 0.3.21 baseline; preserve working audio and quiet boot.
3. **Input and tablet:** resolve touchpad/keyboard contention, qualify brightness
   key integration, live tablet-mode reporting, rotation and coordinate transforms.
4. **Networking and text:** complete AWDL peer data/channel scheduling and real
   AirDrop transfers; finish system-wide FreeType integration.
5. **Stability:** sleep/wake, external displays, hardware video decode and sustained use.

[ROADMAP.md](ROADMAP.md) and [UNIFIED-CANDIDATE-WORK.md](UNIFIED-CANDIDATE-WORK.md)
retain the earlier dated engineering history. Their historical version/status
entries must be read in that context; this README records the 30 October update.

<a name="licensing"></a>
## ⚖️ Attribution and licensing

This project is a **retargeting**, not original work. It stands on:

- [NullMoth / nvidia-macos-driver](https://github.com/nullmoth/nvidia-macos-driver) — driver skeleton,
  plugin, translator and the `xkpi` LinuxKPI-on-xnu layer. **PolyForm Noncommercial 1.0.0**; the kexts,
  plugin and translator carry this licence, which is **noncommercial** and materially different from
  the Apache-2.0 badge this README previously displayed.
- The [NullMoth-Xe](https://github.com/nullmoth/nvidia-macos-driver) Panther Lake write-up — the upstream
  portability fixes reused for drm-kmod, libdrm, Mesa and the plugin.
- Intel `i915`, DRM and TTM from the Linux kernel — **GPL-2.0**; firmware under `firmware/LICENSE.i915`.
- FreeBSD `linuxkpi` and `freebsd-src` — **BSD-2-Clause**.
- [Mesa](https://gitlab.freedesktop.org/mesa/mesa) 26.3.0-devel incl. ANV — **MIT**; `libdrm` — **MIT**.
- Khronos `Vulkan-Headers` — Apache-2.0 / MIT.
- The AIR→SPIR-V translator (based on `metal2vulkan`) — **LGPL-3.0** with **GPL-3.0** components.
- `ogkm610` — NVIDIA open kernel modules, MIT except where noted.
- **AirportItlwm** (the custom laobamac Sequoia build, `d695c97`, `com.zxystd.AirportItlwm` 2.4.0) — the
  macOS port of Intel's Linux `iwlwifi` driver, which is **GPL-2.0**. The Meteor Lake PCI match, the
  fragmented-PNVM loading and the `itlwm_mtl_internal_aux` path are this project's additions on that base.
- The **VoodooI2C** touch stack (VoodooI2C, VoodooI2CHID, VoodooGPIO) and the LegionI2CArrowLake
  adaptation — **GPL-2.0**, with the Legion kext under ISC.
- **AppleALC** 1.9.8 and **IntelBluetoothFirmware** / BrcmPatchRAM — by their respective authors, under
  their own terms.
- AMFIPass 1.4.1, Lilu and VirtualSMC — by their respective authors, under their own terms.

**Original work** (VedCloud code, scripts, patches, tests and documentation) is licensed under the
**[Meteor Lake Xe-LPG Metal Graphics License](LICENSE)**, whose grant is substantively equivalent to
Apache-2.0 and which is additionally available under Apache-2.0. **Third-party components are not
licensed by it** and remain under their own terms, enumerated in Schedule A of `LICENSE`.

Because the tree combines **GPL-2.0-only** code (e.g. `drm-kmod/drivers/gpu/drm/drm_gpuvm.c`,
`dma-buf/sync_file.c`) with **GPL-3.0** code (the translator) and with **PolyForm Noncommercial** code
(NullMoth), no single license covers the combined work: GPL-2.0-only and GPL-3.0 are mutually
incompatible, and PolyForm NC is incompatible with both the GPL and permissive relicensing. Those
incompatibilities are inherent to the upstreams and are not resolved by the project license. The
per-component licences are preserved in-tree (`latitude-desktop/licenses/`, `latitude-runtime/licenses/`,
and each upstream tree's own `LICENSE`/`COPYING`), and must remain in place.
