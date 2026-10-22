# Dell27 standalone UEFI preflight 0.0.1

This is an x86_64 UEFI application that runs without Linux or Sequoia. It is **not a macOS 27 bootloader**: the ARM-to-x86 execution runtime is not implemented and no guest kernel is executed. Its console explicitly stops at that boundary. It keeps UEFI boot services active and performs no raw storage writes, GPU register writes, NVRAM changes or firmware installation.

The application inventories the firmware memory map and current GOP dimensions/stride, reads four pinned ARM macOS 27.0 build 26A428 inputs from the same firmware filesystem, checks their exact sizes and SHA-256 hashes, and validates the kernel's ARM64 MH_FILESET load-command bounds. It does not guess kernel execution addresses or attempt to run ARM bytes on the Intel processor.

Input paths from the filesystem root: macOS27/bootkc, macOS27/dtree, macOS27/sptm, macOS27/txm. The prepared ramdisk remains in the separate shell reference package; no standalone ARM runtime currently consumes it.

Validation: strict host build with ASan/UBSan; 14 SHA-256 comparisons with Python hashlib; 47 malformed/truncated command inputs rejected; real pinned kernel accepted; EFI entry actually executed under x86 OVMF with RAM/GOP inventory and all four hashes passing. A second OVMF run rejected a one-byte-modified device tree with status 26 (EFI_SECURITY_VIOLATION). These are virtual-firmware tests, not Dell hardware tests.

USB placement: EFI/Tools/Dell27Preflight.efi on the small FAT volume, plus macOS27/ inputs there. It is deliberately not installed as EFI/BOOT/BOOTX64.EFI, because this diagnostic cannot start macOS. The small volume currently has a Microsoft Basic Data GPT type after formatting; automatic removable-media discovery has not been qualified. No new firmware boot option or Sequoia loader was installed. Do not expect a macOS 27 desktop when booting this USB.

Further work: an independently bootable x86 monitor needs physical memory management, interrupts/timers, protected JIT memory, ARM CPU/system-register/exception execution, guest address translation, Apple platform contracts and guest device drivers. The existing hosted QEMU shell is a behavior reference, not that bare-metal runtime. Hardware GPU/display integration is a subsequent stage.

EDK2 headers were obtained from the official tianocore/edk2 repository. This application's code is MIT; it includes the local boot_input admission implementation and FIPS 180-4 SHA-256 arithmetic. Sources, profiles and evidence remain on Corsair. The Apple boot inputs are private local files and must not be published.
