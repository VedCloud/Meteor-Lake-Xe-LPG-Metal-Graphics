# macOS 27 on the Dell Latitude 5350: initial feasibility audit

Target: existing Intel Dell Latitude 5350. Repository revision: 28663c30f0a14c1862c099d6ea46f1b3421ab011. Date: 22 October 2026.

## Verified locally

Snapintosh describes and implements an ARM EL2 monitor for the Snapdragon X2 Elite ASUS A16. Its entry assembly uses ARM exception-level registers and its graphics stack encodes Adreno commands. These components cannot execute directly on the Dell Intel CPU/GPU. Copying the current Dell EFI or changing the model identifier does not implement CPU instruction translation.

The development Mac runs macOS 27.2 build 26B5101f, rather than Snapintosh's pinned 27.0 build 26A428. The host kernel file contains an x86_64 Darwin 27.2 kernel, and dyld contains an x86_64 slice. However launchd, WindowServer, loginwindow, Finder and Dock on this installation contain only arm64e variants. The associated paths, hashes and lipo output are in host-binary-inventory.json. This is evidence about these host files only: it neither proves all macOS 27 distributions omit Intel slices nor establishes a bootable Intel installation.

## Engineering gap

A native Intel route needs compatible Intel kernel, essential drivers, boot support and executable userland as one coherent build. The observed Intel kernel alone does not satisfy this. Reusing Tahoe components would be a hybrid system requiring separate ABI/behavior qualification, not proof that macOS 27 was ported.

An ARM guest route on the Dell needs ARM CPU/system-register/exception and memory-order emulation or binary translation, ARM platform/monitor integration, a guest boot chain, and GPU/API translation to Intel hardware. Intel virtualization extensions do not execute ARM instructions natively. Snapintosh supplies no complete Intel-host implementation of those boundaries. The existing Dell x86_64 kexts cannot be loaded into an ARM kernel.

The next acceptance milestone is a reproducible boot to launchd on an isolated, non-destructive guest using identified local kernel/userland inputs. Desktop acceleration comes after that milestone. No claim of achievable native performance or working translation is made.

## Validation

PASS: clean repository clone, revision identification and architecture inspection with file/lipo/dwarfdump.
FAIL (environment prerequisite): tools/test-profile.py and tools/test-classify.py under system Python 3.9 fail importing Python 3.10 union annotations. No upstream runtime defect established.
NOT RUN: guest build/boot, firmware extraction, private ABI reconstruction, Intel-to-ARM translation, GPU and hardware tests. Matching 26A428 KDK, extracted boot inputs and q1n1 are absent. No Dell/USB files or active EFI were changed.

## Sources

- https://github.com/VedCloud/Snapintosh (README and docs/COMPATIBILITY.md)
- https://support.apple.com/en-us/127255 (macOS 27 supported hardware)
- https://support.apple.com/en-us/127455 (Apple silicon upgrade requirement)

This report is private local research under ignored build/. It contains no copied vendor binaries or disassembly. Machine-specific inventory must remain local.
