# Current workspace

The authoritative copied Dell project is ../iGPU Port. macOS 27 work is in this folder. Dell27Build.sparsebundle stores the isolated no-space build filesystem, mounted at /Volumes/Dell27Build. All its storage resides on the Corsair drive.

To remount after disconnect/reboot:

    hdiutil attach "/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/Dell27Build.sparsebundle" -nobrowse -mountpoint /Volumes/Dell27Build

The full source trees, dependency environments, build logs, firmware preparation and verified package are inside that mounted volume. Sources are pinned, not modified to fake guest success. QEMU built with Python 3.12 and PVG disabled. The x86_64 build explicitly uses the 26.5 SDK and macOS 15 deployment target. Shell boots were tested on the development ARM Mac with both its ARM emulator and the Intel executable via Rosetta. No Dell hardware test or desktop/GPU support is claimed.

The current USB is macOS 27, replacing both old SanDisk partitions after verification of the full original backup. It contains macOS27-Shell, not a native macOS 27 EFI installer. Read the package README and VALIDATION.json before testing.

Standalone code is /Volumes/Dell27Build/Standalone. Current version 0.0.3 executes scalar/EL2 probes and a bounded real SPTM entry inside x86 UEFI; privileged ARM/XNU execution remains incomplete. Source/evidence are also mirrored at Standalone-UEFI-Source outside the build image. Read USB-CURRENT-STATE.md for current physical media details.
