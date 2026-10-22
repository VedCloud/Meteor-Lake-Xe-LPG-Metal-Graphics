# Brightness-key report 23:14 and ACPI name-limit correction

Report: failure-detail-20261020-231400, boot23:07:28. Loaded framebuffer0.2.11 and BRTKEYS revision4 match the candidate. The new fields establish that validation of Query66Entries succeeds, then evaluateObject("\\LQCT") returns0xe00002c2 before any result decoding. snapshotFresh=0 correctly excludes stale counters. No firmware event counts are available, so this report still cannot establish whether the physical key invokes Q66/SMEE/BRT6. Weather is outside this work's scope.

## Specific implementation error

Read-only inspection of the supplied Sequoia15.6.1 recovery image's AppleACPIPlatform implementation shows that AppleACPIPlatformExpert::evaluateObject rejects an OSSymbol name whose length exceeds4 with BadArgument before calling AcpiEvaluateObject. IOACPIFamily's const-char overload constructs the symbol and dispatches to that implementation. The disassembly has a BadArgument default, calls the name's length accessor, compares it with4 and exits for greater lengths. This differs from validateObject's path lookup. Both earlier calls violated that restriction: the absolute LCD method path is long, and even "\\LQCT" is5 characters. The earlier package-conversion theory was unproven and did not identify this error.

The reference is recovery15.6.1 rather than the target15.8.1; the target's validate-success/evaluate-BadArgument trace matches this restriction. Saved local research evidence is research/acpi-evaluate-name-limit.asm, with the family dispatch and platform code in separate reference disassemblies. The recovery image was detached after inspection. No Apple binary is shipped or replaced.

## Framebuffer0.2.12 correction

Locate the actual LCD node through IORegistryEntry::fromPath using its target-confirmed IOACPIPlane path, IOACPIPlane:/_SB/PC00/GFX0@20000/LCD@400. This reaches the node even though it is not registered as an IOService. Then validate and evaluate the local four-character method name "LDBG". Its existing package returns the five software counters. Root/validated-panel/stopping guards, stage/result metadata and the helper's fresh-only reporting remain. The looked-up registry entry and returned object are released. No key notifications or firmware methods that access hardware are invoked by the query.

This restores one OSArray metaclass import relative to0.2.11; that import was already used by the target-tested0.2.10 framebuffer. Other imports remain within the prior known set. The existing IORegistryEntry::fromPath import was already used for /options lookup. The physical-key route remains revision4; no EFI or keyboard-driver change is made. Graphics scheduling, Night Shift and PWM behavior are unchanged. This is a concrete correction to my counter API usage, not a claim that physical key delivery is repaired.

## Local verification

The actual root endpoint fixture now models Apple's four-character name guard. It reproduces BadArgument for both prior query forms while validation can succeed, and verifies local LDBG success on the unregistered LCD node, bad package/integer handling, ACPI evaluation failures, failure metadata and root/stopping guards. ASan/UBSan checks pass. The existing actual-AML firmware-chain and notification checks, helper stale-result checks, x86_64 framebuffer build/signature and guarded installer fixture pass. Hardware query success and brightness-key operation remain pending.

The package remains graphics0.3.19 with framebuffer0.2.12. Installer compatibility includes the installed0.2.11 framebuffer. Previous files and EFI are backed up before publishing; EFI and the frozen rendering baseline must remain identical. Use the existing fallback installer, reboot through the USB, try F6/F7 alone and with Fn, and collect on that same boot. No rendering suite is requested.

References:
- Apple API declaration: https://developer.apple.com/documentation/kernel/ioacpiplatformdevice/3516509-evaluateobject
- BrightnessKeys upstream obtains ACPI nodes from IOACPIPlane paths before using them: https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp
- Local primary evidence: supplied BaseSystem.dmg, IOACPIFamily and AppleACPIPlatform fileset entries from BootKernelExtensions.kc; research/acpi-evaluate-name-limit.asm. API documentation alone does not describe the observed four-character guard.
