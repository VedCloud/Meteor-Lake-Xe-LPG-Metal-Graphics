#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
 cac6062e2c6413f16824fb0b48e17b8ae2b02c30f47683d8f16e6dfc0ae42d27) profile=base ;;
 9c68b78fa0823f4e89c34bdb1ffd64fea540a236fcc5dd3828ac48ad53688795) profile=base ;;
 f3dd6e5d56ba16552040445f9ab925a1a67a4c5857d17b051d5fa0de427aa8fc) profile=touchpad ;;
 163a9d65a47d3380ef459690f8548d478b9440f3e0a460fc29c4c3a206cceb65) profile=touchpad ;;
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
 base-accelerated:cac6062e2c6413f16824fb0b48e17b8ae2b02c30f47683d8f16e6dfc0ae42d27|base-recovery:9c68b78fa0823f4e89c34bdb1ffd64fea540a236fcc5dd3828ac48ad53688795|touchpad-accelerated:f3dd6e5d56ba16552040445f9ab925a1a67a4c5857d17b051d5fa0de427aa8fc|touchpad-recovery:163a9d65a47d3380ef459690f8548d478b9440f3e0a460fc29c4c3a206cceb65) ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
