# USB state — 2026-10-22

Both original SanDisk partitions were erased with user authorization after verifying the full Corsair backup. The main exFAT volume is macOS 27. No Sequoia EFI was restored, and no default EFI/BOOT/BOOTX64.EFI was installed.

The small FAT volume holds EFI/Tools/Dell27Preflight.efi, now version 0.0.3, and four hash-pinned Apple boot inputs under macOS27/. It currently has the Microsoft Basic Data GPT type; automatic removable boot discovery is unqualified. Dell Boot from File can be used to select the Tools application for a diagnostic test.

Version 0.0.3 executes bounded scalar and selected EL2 payloads inside the x86 UEFI program, then five instructions from the real pinned SPTM image. The SPTM entry probe stops intentionally at its first boot-argument read. Virtual OVMF execution passes with 69 instructions and 80 QEMU-matched result bytes. A separate run rejects a modified device tree. This does not complete SPTM initialization or execute TXM/the Apple kernel. A complete standalone privileged ARM runtime and desktop remain unimplemented.

The main volume contains Standalone-UEFI-Preflight with sources, evidence and manifests, plus macOS27-Shell, an older hosted QEMU shell reference. The hosted package requires macOS; it is not the selected standalone architecture. No Linux host is being used. Dell hardware has not yet qualified either execution path.

The previous 0.0.1 diagnostic is archived at Standalone-0.0.1-archive on Corsair. Whole-original-USB backup remains at ../SEQOUIA-USB-Backup-20261022/SEQOUIA-Whole-USB.dmg.
