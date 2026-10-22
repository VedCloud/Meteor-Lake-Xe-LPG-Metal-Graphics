# New text + AWDL package, 21 October 2026

All existing tablet, graphics, theme, chime and Copilot settings are retained.
The active EFI Wi-Fi driver and graphics binaries were not replaced by packaging.

Text rendering: ordinary CoreText and AppKit glyph routes, native San Francisco
variable fonts, deferred transparent CPU text-image composition, clipping and
ownership pass host sanitizer/release tests. Intel binaries are ready for a Dell
fixture run. This does not deploy FreeType into all existing Apple apps or
GPU-backed CoreAnimation surfaces. Late dyld activation is explicitly unsupported.

Run on the Dell (no sudo or graphics reinstall needed):

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Desktop/TextRendering/validate-native-text.sh

AWDL: Peripherals/Continuity/Candidate-0.1.0 contains the complete compiled kext,
build identity, private changes, bounded parser/ownership tests and exact working
Wi-Fi bundle. No full AirDrop claim and no hardware transmission proof exist.
The raw current-channel action gate remains off. First qualify ordinary Wi-Fi
compatibility before enabling any future radio transmission experiment.

To select the staged AWDL build for a USB boot (does not arm transmission):

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/Candidate-0.1.0/trial-awdl.sh apply

To restore the saved working Wi-Fi bundle:

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/Candidate-0.1.0/trial-awdl.sh restore

These operate on this USB only, verify exact source/target hashes, save the
previous bundle and preserve configuration/boot arguments. Reboot through the
USB after selection. They never unload a live driver.

Read-only radio collection on the Dell:

    sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/collect-continuity.sh

The ordinary desktop collector includes ISH state and native text fixture logs.
Do not run install-windowserver.sh expecting it to activate this AWDL/text work;
the existing working graphics installer is preserved.
