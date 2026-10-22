# macOS 27 on the Intel Dell: researched route

22 October 2026. This is a source-backed feasibility result, not a working installer. The existing Sequoia EFI and Dell storage were not changed.

## Finding and recommended first milestone

Use the pinned darwin-vm/qemu-sptm system emulator for an isolated ARM macOS serial-shell experiment. This reuses an existing ARM instruction translator, Apple CPU/interrupt models, loader, SPTM/TXM support and ramdisk workflow. It is the shortest evidenced path found to executing macOS 27 code on this Intel CPU. It does not supply a full desktop or accelerated GPU, and no guest was booted in this session. A successful shell would establish only the boot milestone.

Directly copying Snapintosh into the Dell EFI will not work. Its EL2 monitor runs ARM code on Snapdragon and its graphics commands target Adreno. Intel virtualization does not translate ARM instructions. QEMU TCG is the applicable cross-architecture engine, while native hypervisor acceleration is not a substitute for that engine.

## Native Intel route

The host inventory establishes that this 27.2 installation has an x86_64 Darwin kernel and an x86_64 dyld slice. It also establishes that launchd, WindowServer, loginwindow, Finder and Dock here lack x86_64 slices. This is specifically build 26B5101f on this host, not a survey of every macOS 27 restore image.

Dortania's Golden Gate assessment independently describes the loss of essential Intel userspace and drivers. A bootable Intel system needs the matching kernel, platform/storage/input/display drivers, process bootstrap, frameworks and applications to work together. Rebuilding a kernel or patching the installer does not close those boundaries. Replacing the missing pieces with Tahoe binaries creates a hybrid requiring separate ABI qualification; changing a version string would not establish a macOS 27 port.

A reverse-Rosetta route would need more than an instruction decoder: executable loading, arm64e/PAC behavior, ARM memory ordering and atomics, Mach traps, thread state, signals/exceptions, dyld shared-cache loading and JIT/code-signing interactions all need correct mappings. No complete drop-in implementation for this Dell was found in the reviewed primary sources. QEMU's current user-mode documentation covers Linux and BSD, not a ready Darwin ARM-on-Intel process bridge. Darling targets running macOS applications on Linux; it is not a bootable macOS 27 replacement.

## What the actual QEMU fork contains

Pinned darwin-vm revision and submodule revision are recorded in RESEARCH-STATUS.json. The darwin machine opens the boot kernel collection, device tree, trustcache and ramdisk; enables emulated EL2; configures Apple registers, AIC and UART; and passes to the XNU loader. It sets max_cpus to 1. The source inspection supports a minimal serial system, not a complete Mac model.

run.sh uses the darwin machine with those four inputs and optionally SPTM/TXM. get_files.sh defaults to an iPhone image. For our experiment it must explicitly select a macOS device and a verified matching Apple macOS restore image; running its defaults would test the wrong OS. Boot-input identity must be recorded before execution. Preparation modifies only a separate ramdisk image, never the Dell disk.

The upstream README excludes GUI, graphics, Wi-Fi and Bluetooth. Existing generic QEMU Apple PVG source imports Apple's ParavirtualizedGraphics framework; its mere presence does not show it is connected to the darwin machine or usable on the Intel Dell. The upstream instructions also currently disable PVG for macOS 27 builds.

## Graphics after a shell

For a desktop, first provide a guest display device and matching macOS ARM driver, establish WindowServer startup and validate scanout/surface lifecycle. Acceleration then needs an ARM guest Metal/IOGPU interface, validated resource/fence ownership across the guest boundary, and a host backend capable of running work on Intel Meteor Lake. The existing x86_64 Dell GPU kext cannot load into an ARM kernel. Snapintosh's Adreno command streams cannot execute on Intel. Reusing our Metal translation concepts would require a new transport/backend integration, not copying binaries.

Start with one bounded command submission and readback across the guest/host boundary before claiming desktop acceleration. Then prove fences, reset/teardown, surfaces, compositing and frame pacing. Do not claim native performance for the CPU-emulated guest without measurements.

## Local work and checks

PASS: cloned darwin-vm and initialized the exact pinned qemu-sptm source on the external Corsair drive; inspected loader, CPU setup, device wiring and graphics boundary; recorded revisions; retained the earlier local architecture audit.

NOT COMPLETED: QEMU configuration. The internal host disk has approximately 121 MiB free, so research downloads were placed externally. QEMU rejects paths containing spaces; a temporary no-space alias gets past the initial check, but its virtualenv resolves the physical spaced path and configure fails at line 953. configure-host.log records the actual error. No system dependencies were installed and no existing data was removed.

NOT RUN: QEMU compilation, restored macOS guest, Intel-host boot, GUI, GPU and performance tests. Required extraction/build dependencies and matching boot inputs remain absent. This is not a hardware-qualified candidate.

## Next concrete execution steps

1. Establish a no-space external build location, with modern Python, Ninja, pkg-config and GLib. Build the pinned aarch64-softmmu fork with PVG disabled and preserve the build log.
2. Select one upstream-supported macOS guest identity and matching Apple image; record build/device/component hashes. Do not silently use the iPhone default or mix host 27.2 files into a pinned 27.0 guest.
3. Prepare a disposable ramdisk and trustcache using the reviewed upstream workflow. Keep Apple inputs local and separate from distributable code.
4. Boot via TCG with no physical-disk attachment. Capture a live serial transcript proving kernel version, launchd and a shell command. Repeat on an Intel host before calling it a Dell result.
5. Only after reproducible boot, scope the display/IOGPU integration above. This remains substantial engineering and has no verified completion date.

## Primary sources reviewed

- https://github.com/VedCloud/Snapintosh — ARM EL2/Adreno design, target and pinned build.
- https://github.com/jprx/darwin-vm — minimal macOS shell workflow and explicit GUI limitations.
- https://github.com/jprx/qemu-sptm — pinned implementation inspected locally.
- https://www.qemu.org/docs/master/system/introduction.html — TCG system emulation and accelerator model.
- https://github.com/qemu/qemu/blob/master/docs/user/main.rst — supported user-emulation OS interfaces.
- https://github.com/dortania/OpenCore-Legacy-Patcher/issues/1183 — remaining Intel components and missing userspace/drivers.
- https://github.com/DarlingHQ/darling — Linux compatibility-runtime scope.
- https://support.apple.com/en-us/127255 — official supported architecture/hardware.

Public posts about other Intel macOS 27 experiments were found, but no independently reviewed implementation or reproducible complete Dell boot was obtained from them. They are not treated as proof of a working route.
