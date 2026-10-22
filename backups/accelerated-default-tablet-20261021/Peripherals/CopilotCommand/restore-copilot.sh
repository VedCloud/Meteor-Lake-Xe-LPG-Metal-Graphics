#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
REL="EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist"
EXPECTED="5f1ff2ec4e0a4a1333555093cd1a47be6469bf89a2ad96907614f906b7c3c95b"
[ "$(shasum -a 256 "$ROOT/$REL" | cut -d ' ' -f1)" = "$EXPECTED" ] || { echo 'Keyboard configuration has changed; refusing to overwrite it.' >&2; exit 2; }
cp "$ROOT/iGPU-Port/backups/input-system-sync-20261021/$REL" "$ROOT/$REL"
echo 'Original keyboard profile restored. Reboot through this USB.'
