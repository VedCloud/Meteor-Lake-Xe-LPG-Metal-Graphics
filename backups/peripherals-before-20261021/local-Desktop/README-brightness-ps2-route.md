# Brightness keyboard route, BRTKEYS revision4

The Control Centre brightness slider already works. This change addresses delivery of a keyboard brightness request; it does not change PWM registers, display modes, graphics scheduling or Night Shift.

Dell's Latitude5350 manual assigns brightness down/up to F6/F7, with Fn behavior affected by function-lock settings. The target firmware's EV6 calls GFX0.BRT6. The current Darwin BRT6 wrapper sends LCD notifications0x86/0x87, which require BrightnessKeys to select the correct ACPI panel and handle its notifications. The latest available report (failure-detail-20261020-220831) shows BrightnessKeys active but no handled panel notification marker. Its firmware-counter query failed, so we cannot yet conclude where the original event stops. No newer hardware report was present during this change.

The installed VoodooPS2Keyboard is2.3.8. Upstream ApplePS2Keyboard::message explicitly handles notification prefixes3/4 as a make/break pair. Its stock map maps normal scan codes0x65/0x66 to F14/F15, the same brightness codes used by BrightnessKeys. Revision4 sends BRT6 up requests directly to PC00.LPCB.PS2K with0x0366 and down requests with0x0365. It replaces the LCD notification route, rather than sending both and potentially doubling brightness steps. The firmware query, SMEE flag read, counters and non-Darwin original-method fallback remain unchanged. No global F6/F7 remap is installed, and no keyboard or touchpad kext is replaced.

This is a supported alternative delivery path, not proof that firmware emits the required event on this Dell. It will not repair a missing upstream firmware event. The corrected counter endpoint in framebuffer0.2.10 and existing collector remain the way to distinguish that case. No broad scan-code logging is enabled.

Validation: ACPICA compiled and executed the actual SSDT with mock firmware, observed both PS2K notification values, checked query/SMEE/BRT6 counters and original non-Darwin fallback. A separate ASan/UBSan fixture executes the upstream VoodooPS2 message method and stock map; each notification yields exactly one F14/F15 press/release pair. These are offline checks, not target brightness validation. OpenCore config validation and an exact one-file EFI diff are required when publishing.

The graphics candidate remains0.3.19. Only EFI/OC/ACPI/SSDT-LAT-BRT.aml changes in EFI; its source, package copy and integrity metadata are updated. The previous EFI and replaced package metadata are backed up locally and on USB before publishing. The collector already records the USB route hash, keyboard/BrightnessKeys registry, graphics ACPI subtree and firmware counters; no collector change is necessary for this route.

On the Dell, boot this USB and test F6/F7 alone and Fn+F6/F7. Check both the on-screen indicator and actual panel brightness. If also installing graphics0.3.19, use its existing fallback installer and normal reboot procedure. The ACPI key patch itself does not require installing another kext. A failed test should be followed by the existing desktop collector on the same boot.

Primary references:
- Dell Latitude5350 function keys: https://www.dell.com/support/manuals/en-us/latitude-13-5350-2-in-1-laptop/latitude-5350-owners-manual/keyboard-function-keys?guid=guid-703f5965-dd57-4650-8547-e31b9d6db280&lang=en-us
- VoodooPS2 notification decoder: https://github.com/acidanthera/VoodooPS2/blob/master/VoodooPS2Keyboard/VoodooPS2Keyboard.cpp
- VoodooPS2 stock map: https://github.com/acidanthera/VoodooPS2/blob/master/VoodooPS2Keyboard/ApplePS2ToADBMap.h
- BrightnessKeys brightness codes and panel selection: https://github.com/acidanthera/BrightnessKeys/blob/master/BrightnessKeys/BrightnessKeys.cpp
