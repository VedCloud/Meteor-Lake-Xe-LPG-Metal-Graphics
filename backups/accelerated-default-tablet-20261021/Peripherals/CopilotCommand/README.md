# Copilot → Command keyboard candidate
Applied to this USB's VoodooPS2Keyboard Info.plist only; existing executable, OpenCore config and other kexts are unchanged. Boot through this USB to test. This is PS/2 driver translation before macOS userspace, not BIOS/EC flashing and does not modify Windows.

The candidate recognizes the standard Shift+Windows+F23 sequence and emits a balanced Command press/release. Tested the upstream inversion functions under ASan/UBSan for all 36 make/release orders, Command+C, normal Shift+C, and timeout flush. The Dell's actual chord timing and hold behaviour remain unverified. First test Copilot+C and Copilot+V, then check ordinary Shift typing and modifier release. If the firmware emits only a tap, holding it as Command may require a different implementation.

Revert with `/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/CopilotCommand/restore-copilot.sh` then reboot through USB. No touchpad trial is enabled.

Source: https://github.com/acidanthera/VoodooPS2 ; standard Copilot chord reports: https://github.com/microsoft/PowerToys/issues/36692
