# Local Dell Latitude 5350 project invariants

## Confirmed Copilot → Command mapping (2026-10-21)
The user confirmed the Copilot key works as Command after rebooting through the USB and explicitly requires retaining this fix everywhere. Preserve it in all future active/candidate/recovery EFI builds. This is VoodooPS2Keyboard Info.plist Macro Inversion, not a userspace remapper or a firmware flash.

- Canonical artifact: `port-artifacts/copilot-command/VoodooPS2Keyboard.kext`.
- Idempotent retention/validation: `python3 copilot-sources/retain.py /path/to/EFI` then `python3 copilot-sources/retain.py /path/to/EFI --check`.
- Regression test: `python3 copilot-sources/test-macros.py` exercises actual upstream inversion code.
- Preserve existing profile macros, executable and unrelated settings. Default `Swap command and option` must stay false for this mapping. A new keyboard implementation requires explicit porting/validation, not blind profile copying.
- Touchpad apply/restore scripts currently do not replace the PS/2 bundle, so keep that property. Any future tool replacing the PS/2 bundle must retain and validate these macros before publishing, and update relevant package hashes.
- Do not rewrite historical backups, frozen passing graphics baselines, or old checkpoints to pretend they contained this later fix. Reapplying an old EFI requires applying this confirmed fix to a new working copy.
- The confirmed Copilot change is independent of brightness helper 0.2.0, whose slider/HUD integration remains unverified and whose captured diagnosis showed Accessibility permission absent. Do not bundle that unverified state as a confirmed keyboard requirement.

## Normal USB graphics boots (2026-10-21)
The user explicitly requests accelerated graphics on every normal USB boot. Preserve OpenCore NVRAM Delete+Add refresh of `nullmoth-gpu-once` and `nullmoth-panel-once` (Apple GUID 7C436110-AB2A-4BBB-A880-FE41995C9F82) in new base and peripheral configs. `graphics-mode-sources/prepare.py` creates/validates this policy. Do not return to one-shot-only normal operation. Keep explicit manual recovery profiles and the driver's attach-failure guard. Regenerate hash-guarded GraphicsModes profiles/switch script when changing EFI config; do not overwrite new changes using an old profile. Historical backups and frozen baseline remain unchanged.

## Tablet support status (2026-10-21)
User confirmed automatic suppression of built-in keyboard/touchpad while folded, restoration when unfolded; external input and touchscreen remain usable. Auto-rotation also needs matching touchscreen/pen coordinates. Linux guided capture now verifies Intel HID switches and accelerometer axes across 50 samples; Dell WMI switch remains false. macOS still has no qualified live ISH sensor/mode publisher. See tablet-sources/LINUX-RESULTS-20261021.md. Preserve the 500 ms debounced policy, stale-signal fail-open, flat hold and editable-field-only on-screen-keyboard intent. Follow `tablet-sources/REPORT-20261021.md`; the original ACPI collector silently returned empty data and has been corrected. Do not treat empty or zero-gated firmware readings as tablet mode or claim the diagnostic-only helper implements automatic features.
