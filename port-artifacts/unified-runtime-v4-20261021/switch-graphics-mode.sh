#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
 fd40a9c5c519d8a19b7f0cbdea6f324d75f4951445b4aa57d4edad6f3c45fd60) profile=base ;;
 a1cf2bc99db71487765216be0754f5cc8dd21cfd6dbd74543578c94671525be0) profile=base ;;
 c2e9066b96b74491093c3511f70fd286aa42457c78c4bc3c2232f3b760ad2077) profile=touchpad ;;
 2c3542635fbfeab08923a9af1320adc09da7879949c932d414c3daa6bafc5c24) profile=touchpad ;;
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
 base-accelerated:fd40a9c5c519d8a19b7f0cbdea6f324d75f4951445b4aa57d4edad6f3c45fd60|base-recovery:a1cf2bc99db71487765216be0754f5cc8dd21cfd6dbd74543578c94671525be0|touchpad-accelerated:c2e9066b96b74491093c3511f70fd286aa42457c78c4bc3c2232f3b760ad2077|touchpad-recovery:2c3542635fbfeab08923a9af1320adc09da7879949c932d414c3daa6bafc5c24) ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
