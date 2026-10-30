# Dell touchpad feature-safety candidate 1.0.10

Built from the existing complete-input 1.0.9 stack. Applied to the actual USB EFI and its two current copy-ready EFI folders on 2026-10-28. The internal disk was not modified. See DEPLOYMENT.json for the exact backup and changed files.

Corrected demonstrated GET_REPORT and SET_REPORT bugs: failed reads no longer copy memory into HID state; declared reply length and report identity are checked; allocation and descriptor failures are handled; full-width command lengths replace UInt8 truncation; extended IDs preserve the actual data report ID; unnumbered zero-leading reports retain their first byte.

The captured ELAN 04f3:328e report 3 declares a 16-bit input mode. This candidate writes both bytes and rejects mode replies missing either byte. It guards the mode setter against null elements and allocation failure. It does not claim successful on-device mode acknowledgement merely because the void IOHID setter returned.

The actual updated C++ functions pass ASan/UBSan replay with every 16-bit wire length, every report ID, extended IDs, zero-ID leading-zero payloads, large SETs, failing transport/allocation/descriptor operations and 16-bit mode initialization. Previous functions fail the failed-GET and long-SET negative controls. Input report/polling regression fixture also passes. Kernel x86_64 compilation, signature, export/import comparison, OpenCore config and confirmed Copilot retention checks pass.

The complete EFI differs from the selected USB EFI only inside VoodooI2CHID.kext. Graphics, PS/2 keyboard/controller, Copilot mapping, boot options, audio and Windows support are preserved. Exact details: VALIDATION.json and EFI-COMPARISON.json.

These checks establish software corrections, not a guaranteed physical touchpad or keyboard-freeze fix. The saved runtime captures predate 1.0.9 and 1.0.10. No runtime success is claimed.
