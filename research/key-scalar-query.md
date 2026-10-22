# Brightness-key research and scalar-counter query

Source report: failure-detail-20261020-224044, boot22:37:29, collection22:40:44. Weather is outside this work's scope.

## Findings

Metal0.3.19, accelerator0.2.9, framebuffer0.2.10 and PS2Keyboard2.3.8 loaded with the expected identities. Firmware logs confirm BRTKEYS revision4. The key-routing patch therefore loaded; repeating the same install cannot explain the failure. The PWM brightness handler is ready. Slider operation was previously confirmed by the user.

The counter request returned0xe00002c2 (BadArgument). The old endpoint could return that at its initial validation guard, after a successful evaluation that returned an unexpected package, or after finding a non-integer package member. It did not publish the failing stage. The helper did not retrieve the final snapshot status and could include a cached counter dictionary after a failed request. The report therefore cannot establish whether Q66, SMEE or BRT6 executed. The exact cause of the old BadArgument is not yet identified; package conversion must not be presented as proven.

The ACPI snapshot confirms LCD exists in IOACPIPlane but is not registered/matched as an IOService, explaining why the first nameMatching("LCD") approach was unsuitable. ECDV is registered. Its availability does not alone prove a query succeeded.

Revision4 routes BRT6 directly to PS2K rather than through BrightnessKeys. Consequently, the absence of BrightnessKeys' handled-panel marker is no longer a useful test of this route. Upstream VoodooPS2 supports notification0x0365/0x0366 as paired F14/F15 events, matching BrightnessKeys' chosen brightness codes. Offline tests of that decoder and actual AML passed, but the report still does not demonstrate actual firmware delivery or the desktop's handling of those events. Linux's Dell WMI implementation also documents separate brightness event delivery; this is a possible alternative only if the existing firmware event path proves absent. Do not enable another parallel producer speculatively and risk double key presses.

## Query correction

Framebuffer0.2.11 evaluates the five existing root software integers individually through registered ECDV: LQCT, LSCT, LSFL, LBCT and LBAR. It no longer depends on the LCD method's package return or conversion of package elements. Apple's existing evaluateObject API is used; no new undefined kernel imports are introduced. The queried objects are known counters declared by our SSDT, not EC operation-region reads or firmware commands.

Registry diagnostics now identify locate/validate/evaluate/decode/publish/complete stages and the counter involved. The helper retrieves the stage, counter and snapshot result after the request. It marks snapshotFresh explicitly and only includes counters following a successful request. Root, validated-panel and stopping guards remain. Failure preserves the last valid dictionary but the helper does not label it fresh.

This corrects the diagnostic design. It does not claim hardware-confirmed query success or working brightness keys. The direct PS2 route, DSDT, EC firmware behavior and BIOS settings remain unchanged. No global scan-code logging, extra keyboard kext, event injection test or brightness-mode change is added. Metal0.3.19, accelerator0.2.9, GPU1.0.15, Night Shift, PWM and touchpad behavior are unchanged.

## Validation and next observation

ASan/UBSan fixture executes the actual root endpoint, exercising individual successful reads, missing objects, evaluation failures, invalid integer results, exact failure stage/counter, stopping/root guards and the unchanged GPU identity publication branch. Actual AML root counters are directly evaluable under ACPICA; its key route and Darwin/fallback tests pass. Framebuffer/helper x86_64 builds, signatures and no-new-kernel-import comparison pass. Installer fixture checks compatibility and rollback. These are local tests, not a Dell boot.

The USB collector already runs latitude-key-state and saves its JSON/status; replacing that helper includes the new fields without changing collection semantics. Keep the same brightness-key test: F6/F7 alone and with Fn, then collect on that boot. A successful counter snapshot distinguishes an absent EC query, missing SMEE dispatch, a missing BRT6 call and downstream key handling. The five reads are not an atomic event snapshot; they are coarse monotonic diagnostic evidence after testing.

The graphics package remains0.3.19 with a framebuffer diagnostic update to0.2.11. The existing fallback install-windowserver.sh installs it with preflight/rollback checks. EFI is unchanged by this update. The previous framebuffer/helper/package metadata and EFI are backed up before publishing. No rendering suite is required.

## Primary sources

- Apple evaluateObject API: https://developer.apple.com/documentation/kernel/ioacpiplatformdevice/3516509-evaluateobject
- VoodooPS2 notification decoder: https://github.com/acidanthera/VoodooPS2/blob/master/VoodooPS2Keyboard/VoodooPS2Keyboard.cpp
- VoodooPS2 stock map: https://github.com/acidanthera/VoodooPS2/blob/master/VoodooPS2Keyboard/ApplePS2ToADBMap.h
- BrightnessKeys panel selection and event delivery: https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp
- Linux Dell WMI brightness events: https://github.com/torvalds/linux/blob/master/drivers/platform/x86/dell/dell-wmi-base.c
