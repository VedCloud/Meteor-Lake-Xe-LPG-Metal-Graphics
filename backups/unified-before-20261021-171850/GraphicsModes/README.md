# Persistent accelerated USB boot

The user requested accelerated graphics on every normal boot through SEQOUIA. The working driver binaries are unchanged. OpenCore NVRAM Delete+Add now refreshes `nullmoth-gpu-once` and `nullmoth-panel-once` before each USB boot, so the existing drivers' consumed one-shot markers no longer make the following normal boot unaccelerated. The attach-failure guard and panel driver's timeout handling remain intact; this changes normal boot selection, not hardware failure handling.

No graphics installation or test rerun is required for the next boot. The current OS must already contain the installed working runtime/LaunchDaemon/AuxKC; this EFI change alone cannot install it.

Manual recovery selection (edit the USB before boot, from another Mac if necessary):
`/bin/bash /Volumes/SEQOUIA/iGPU-Port/switch-graphics-mode.sh recovery`

Normal accelerated selection:
`/bin/bash /Volumes/SEQOUIA/iGPU-Port/switch-graphics-mode.sh accelerated`

The switch preserves the selected base/touchpad profile, verifies source and destination hashes, backs up the current config, and refuses unknown changes. Stored base/touchpad configs both default to accelerated mode; the failed touchpad trial remains disabled in the active config. Copilot Command translation is retained. A future EFI edit requires regenerating these profiles, not overwriting that edit with an old snapshot.

The WindowServer installer wrapper selects persistent accelerated mode only after installation succeeds. The core runtime updater still requires a maintenance recovery boot before replacing its active driver files. Normal use does not require recovery or another install. This package does not run NVRAM commands on the development Mac or change an internal EFI.
