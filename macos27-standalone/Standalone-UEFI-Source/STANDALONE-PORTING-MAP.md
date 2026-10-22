# Standalone Dell monitor: what the Sequoia project supplies

Target confirmed by the user: Intel Dell Latitude 5350, UEFI -> standalone monitor -> ARM macOS 27, no Linux or existing macOS runtime host. The hosted QEMU shell remains an offline comparison tool. It is not the selected deployment architecture.

## Reusable evidence and source boundaries

- `desktop-sources/latitude_scanout.h`: portable, bounds-checked stolen-memory/GGTT-to-CPU scanout translation. It rejects noncontiguous or incorrectly flagged pages and checks the boot framebuffer alias against the actual reservation. Keep these checks in any new monitor framebuffer mapping. Do not treat a GOP address as a universally reusable physical reservation.
- `NullMoth/kexts/NMXVGAFB/nmxvgafb.cpp`: the tested Meteor Lake panel path, framebuffer publication and brightness/gamma implementation. It is an XNU IOFramebuffer driver, not freestanding code. Reuse register provenance, validated ordering and regression cases; replace its IOKit lifecycle with a defined monitor/guest display boundary. No raw panel-register writes were added to the UEFI preflight.
- `NullMoth/plugin/nvmtl_vk.c`: AIR/Metal-to-Vulkan submission behavior, fresh command-pool workaround and fence-owned resource retirement. This needs a macOS userspace runtime, Vulkan backend and kernel transport. Preserve actual hardware completion before retirement; do not transplant calls to the freestanding loader or mark a queued frame complete.
- `NullMoth/gpu-port/xkpi/xnu/nmx_kext.cpp`: Intel kernel-driver lifecycle through XNU IOService and IOUserClient. These dependencies are precisely why the x86_64 Sequoia kext cannot execute inside an ARM guest or a UEFI application. A standalone GPU backend must supply allocator, locks, IRQs, DMA/IOMMU, firmware submission and reset semantics rather than stub those services with success.
- `NullMoth/gpu-port/patches/mesa-macos.patch`: OS-level portability changes for ANV and libdrm. The shader and hardware-driver work is relevant, but it does not translate ARM CPU instructions.

SEQUOIA-REFERENCE-HASHES.json identifies the inspected source versions. Existing passing GPU stress tests were not repeated, and their results have not been promoted to macOS 27 evidence.

## Required standalone boundaries

1. Firmware entry, pinned component admission and current display/memory inventory: the new `Dell27Preflight.efi` is built and tested under OVMF. It never exits boot services or executes the guest kernel.
2. Native x86 monitor memory, exception/interrupt/timer handling, W^X/JIT buffers and cache retirement: UNIMPLEMENTED.
3. Full ARM privileged CPU semantics, guest memory translation, exception delivery, PAC and Apple-specific contracts on top of x86 code generation: UNIMPLEMENTED as a bare-metal runtime. Hosted qemu-sptm proves a reference guest can run, but uses host allocation, protected mappings, signals and other runtime services. Replacing those services is required before firmware can run it.
4. Guest loader, platform devices and single-vCPU boot through launchd: hosted reference passes; standalone execution UNIMPLEMENTED.
5. Monitor-owned Intel GPU and ARM guest Metal/display bridge: UNIMPLEMENTED. Existing x86 drivers and Adreno commands cannot be copied across this boundary.

Acceptance for the next execution milestone is an actual ARM instruction block running through the selected standalone translation engine after a defined firmware handoff. A UEFI banner, file validation or C-only mock does not meet it. A complete guest kernel milestone then needs serial evidence from that runtime; desktop acceleration requires separate evidence after kernel boot.

## NullMoth research, 22 October 2026

The author's public progress posts report an early macOS 27 UI, and then Dock/cursor progress:
- https://www.reddit.com/user/null-moth/comments/1xb4h31/mac_os_27_on_trash_can_mac_pro/
- https://www.reddit.com/user/null-moth/comments/1xc36gt/mac_os_27_updates/

These are author-reported results, not our reproduced results. The reviewed public GitHub account lists four repositories: 1401, 1401-probe, nullmoth and nvidia-macos-driver. No macOS 27 standalone CPU/boot implementation was found there. The website currently documents the Sequoia NVIDIA stack. No private implementation was inferred from screenshots and no messages were sent to the author.

- https://github.com/nullmoth
- https://nullmothsystems.com/
- https://github.com/VedCloud/Snapintosh

The research does not establish that a standalone accelerated Dell port can be completed by packaging these components. The missing execution and hardware boundaries above remain real engineering work.
