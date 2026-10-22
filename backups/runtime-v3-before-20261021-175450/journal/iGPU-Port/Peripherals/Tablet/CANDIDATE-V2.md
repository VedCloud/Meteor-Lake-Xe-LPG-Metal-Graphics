# Live tablet-state candidate 0.2.0

This update targets the actual failure in the 15:04 laptop/tablet captures:
HIDD was absent, so the passive observer had no provider. It adds an exact
20-byte OpenCore patch that renames only HIDD's original _STA to XSTA, and a
scoped SSDT supplying _STA=0x0f plus the LTSM firmware-read method. The on-disk
firmware DSDT is unchanged. Explicit recovery profiles disable both the rename
and SSDT and the new state kext, preserving the original visibility behaviour.
Global Windows identity patches are left as they were.

LatitudeTabletState 0.2.0 reads the same firmware EC 0x39 byte used by VGBS,
without VGBS's IUCE gating or VBDS modification. It validates the reply package,
rejects read failures and 0xff, and publishes qualified mode/current state plus
a monotonic heartbeat. A folded state requires an unfolded reading first, an
actual CC fold event and a second stable matching firmware reading. CD unfold,
read failures, clock reset and sleep invalidate the state. This is candidate
logic validated with mocked firmware, not hardware confirmation from this build.
It does not disable keyboard/touchpad or write PCI/MMIO/sensor controls.

Install the keyboard as your normal logged-in user, without sudo:

/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/install-tablet-keyboard.sh

Enable the installed app in Privacy & Security -> Accessibility, then rerun that
installer. A per-user login agent starts the app. After booting the new USB,
unfold once and fold again. The keyboard appears only when the mode is qualified
and the focused element is an enabled editable text field. It never reads text
contents or physical key events. Its target GUI behaviour still needs testing.

Automatic orientation is NOT implemented: macOS still needs the live ISH
transport. The manual rotation validation client uses the real IOFramebuffer
requestProbe transform route (verified in Apple's source), targets only one
validated built-in framebuffer and checks CGDisplayRotation plus IOFBTransform.
It attempts rollback if metadata acknowledgement fails. Metadata acknowledgement
is not proof of correct visible pixels or touch mapping. It does not suppress
active contacts; keep all fingers/pen off the screen during an explicit test.
The existing Wacom source already transforms touch and pen from IOFBTransform.

Read-only diagnosis:
/Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/latitude-rotate --diagnose

Explicit manual tests (not run automatically):
/Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/latitude-rotate --rotate 90
Restore landscape:
/Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/latitude-rotate --rotate 0

Collect laptop/tablet reports with the existing collect-tablet-macos.sh commands.
They now include live-state properties and independently queried loaded-kext
identity; an empty ioreg match is not mistaken for an attached driver. The normal
desktop-failure collector also includes mode, rotation and the installed user's
keyboard permission/readiness plus Bluetooth/Wi-Fi capabilities.

No old graphics test suite or install-windowserver command is required for the
EFI/tablet/theme changes. GPU binaries and confirmed Copilot mapping are retained.
The failed I2C touchpad trial remains disabled in the normal profile.
