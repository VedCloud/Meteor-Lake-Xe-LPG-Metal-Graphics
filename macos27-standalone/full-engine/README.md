# macOS27 full UEFI engine snapshot — 30 October 2026

Current diagnostic x86_64 UEFI engine and corresponding local source, advancing
beyond the historical 0.0.3 preflight. Native Darwin27, launchd, recovery services
and WindowServer startup have been observed. The actual display remains black:
**this is not an interactive graphical installer or a Dell installation release.**

The current candidate uses 16 GiB logical guest RAM, bounded sparse backing,
word-wise memory operations, a read-only observer prefilter and unchanged-frame
GOP suppression. --software-compositor names an existing exact-build no-Metal
cleanup compatibility option; it does not implement a compositor or acceleration.

Source/ contains the local bridge/device/profile implementation and diagnostic
scripts. Clone https://github.com/intel/unicorn-for-efi at the exact revision in
Evidence/ENGINE-COMPILE.json and apply Upstream-overlay/ to reproduce the modified
engine source. EDK2, nanoprintf, the pinned toolchain and LLVM/GenFw build tools are
external prerequisites. Dependencies/Standalone supplies the original input-loader
sources. link.py/compile helpers retain their original workspace layout and absolute
local paths; adapt these to the build workspace before building. The source hashes
and original compile results are preserved, not represented as a new portable build.

Apple installer/recovery images, SDK/KDKs, trust caches and initialized-memory
captures are not included. Therefore this EFI cannot reproduce the full boot from
this archive alone. It validates its exact external inputs and refuses mismatches.
Do not replace the working Tahoe BOOTX64.EFI with this diagnostic. Firmware tests
and startup processes do not establish a graphical installer or usable input.
