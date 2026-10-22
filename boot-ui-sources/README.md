# GoldenGateExt + auxiliary entries + boot chime

Theme revision d43617aef6ffff047d004d68832a71b156ae8f28 from
https://github.com/HJebbour/GoldenGateExt-OpenCore-Theme.
PickerMode=External, PickerVariant=Acidanthera\GoldenGateExt,
PickerAttributes=147, HideAuxiliary=true. ResetNvramEntry's source explicitly
marks its entry auxiliary; press Spacebar to reveal it. Other auxiliary entries
are also hidden by OpenCore's extended-mode policy.

OpenCanopy and AudioDxe come from the exact active OpenCore build,
81ecf1a7e75c318dfc859ddabe5b91f4af757231 (1.0.9 DEBUG, 10 October 2026).
The downloaded archive hash was checked against Dortania's build manifest and
its OpenCore.efi compared byte-for-byte to the active loader. The loader and
OpenRuntime are not replaced.

Official OcBinaryData OCEFIAudio_VoiceOver_Boot.mp3 is included. HDA path is the
Dell's 00:1f.3 controller, codec 0, all enumerated outputs, PlayChime=Enabled,
maximum gain retained at -15 dB. Existing OS volume/mute preferences are not
forcibly rewritten. A muted system can remain silent; hardware codec/output
routing and actual playback still need a USB boot test. macOS sound working
does not by itself establish preboot AudioDxe playback.

The previous full EFI is backed up before deployment. Actual UEFI visual/audio
behaviour cannot be tested on the Apple Silicon development host.
