# Broader firmware execution engine candidate — 22 October 2026

Primary sources inspected:
- https://github.com/intel/unicorn-for-efi
- https://github.com/intel/MultiArchUefiPkg
- https://www.qemu.org/docs/master/system/arm/emulation.html
- https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85

Intel's archived Unicorn fork supplies a real UEFI portability layer, ARM64 CPU translation and an x64 host build. Its MultiArchUefiPkg driver primarily handles non-native UEFI applications/drivers; that is not a Darwin board or an ARM OS boot solution. The CPU library is the candidate for reuse. The full qemu-sptm Darwin platform and Apple-specific CPU behavior are still needed. No evidence yet establishes that this older Unicorn fork can execute SPTM through the required Apple contracts.

Sources are cloned and pinned on the Corsair-backed build volume. All 80 selected ARM64/engine/stub source files compile into distinct x64 ELF objects and combine into UnicornA64.o. This is a compile milestone only. Twenty-eight firmware/standard C helper symbols remain unresolved. The complete list and exact revisions are in UnicornBuild/STATUS.json and UNRESOLVED.txt. No services were stubbed with successful returns.

The fork expects LP64 (64-bit unsigned long), so a Windows-target COFF compile is incompatible. Freestanding x64 ELF compiles with NO_MSABI_VA_FUNCS for its SysV internal varargs; UEFI protocol calls still use ms_abi. A correctly matched formatting and varargs bridge must be supplied. Directly mixing these SysV va_lists with a stock MS-varargs PrintLib would be incorrect. EDK2 GenFw has been built locally to support a future ELF-to-PE conversion, but no combined firmware engine has linked or run.

The independently written MIT scalar probe remains separate, not a replacement for full CPU emulation. Version 0.0.3 uses it to execute selected EL2 state operations and five instructions from the pinned real SPTM image inside x86 OVMF, stopping before the first boot-argument access. The loader rejects malformed segments, overflow, W+X mappings, unsupported entry formats and out-of-file data. It deliberately leaves boot arguments unmapped, rather than filling them with invented values. This is not a kernel boot.

Next integration boundary: resolve and test firmware services for the complete engine, qualify CPU reset/EL2/MMU/PAC against qemu-sptm, and reproduce its actual boot-argument/device-tree/memory layout. Do not install the unlinked engine as a bootloader or infer desktop compatibility from compile success.
