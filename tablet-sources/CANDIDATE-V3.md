# Unified runtime candidate 0.2.1 — 21 October 2026

The 17:45 laptop/tablet reports still had no HIDD device or loaded tablet reader.
The matching OpenCore log establishes the ordering error: it patched the OEM
557414-byte DSDT, then replaced it with our 557441-byte custom DSDT. This restored
the unrenamed HIDD _STA. The injected SSDT could not override that method.

This candidate renames HIDD _STA to XSTA directly in BOTH supplied custom DSDTs
and updates their ACPI checksums. Original tables remain available as explicit
Base/Touchpad recovery entries. The ineffective OpenCore rename is removed.
Global OS identity and the existing working device patches are retained.

Reader 0.2.1 arms device-local PS2 gates only after an actual CC fold event,
measured unfolded state and two stable folded readings. Unknown, failed, stale,
future, sleep and unfold signals leave/restore built-in input enabled. The
keyboard releases delivered held keys and suppresses ACPI/backlight/sleep key
side effects while folded. The mouse releases held buttons and suppresses
motion/scrolling. Held keys/buttons require release before resuming. USB devices
and the Wacom touchscreen are unaffected. Current PS2 platform profiles and
Copilot Macro Inversion remain byte-for-byte unchanged. The original plugins are
included in InputRecovery; restore-tablet-input.sh restores them if needed.
These driver candidates are compiled, linked and signed; Dell runtime validation
is still required. The optional failing I2C touchpad trial is not enabled and its
input path is not covered by these PS2 hooks.

Chime: the log found ALC295, the file and an unmuted gain, but playback setup
returned Unsupported for the 44.1 kHz stream. The official resource is converted
to 48 kHz MP3 and routed to speaker output mask 1 from the observed widget list.
This is a targeted compatibility candidate, not confirmation of audible playback.

FreeType now includes immutable component-alpha glyph layers, CoreText shaping,
late opaque-background composition, orientation checks and sanitizer validation.
It is NOT connected to existing system-app text producers or WindowServer yet.
AWDL now includes the pinned OWL protocol core built for Intel, with offline sync,
peer and action-frame tests. It is NOT connected to the Intel radio/Skywalk path.
Neither library is advertised as making native apps/AirDrop work on this boot.

ISH HBM negotiation, client GUID discovery, connection credits and fragment
consumption are implemented and tested offline. PCI power/interrupt/MMIO runtime,
HID feature-report setup and a live accelerometer publisher remain unimplemented.
Automatic rotation cannot run without those signals and matching scanout/contact
coordination. The manual rotation tool remains a separate explicit validation
client; this update does not automatically rotate the screen.

After booting the USB, collect a transition with:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Tablet/collect-tablet-transition.sh
Follow its unfold/fold/unfold prompts. This diagnostic never disables input itself.
