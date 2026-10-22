#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
 0ffb168d7113ba1c8f3a9d44de20e2b6f8374af90169670f4b1062709c84d44e) profile=base ;;
 5a09c3b782544044fd596a65332a42f2f3a494b4f4eadd693dd1f06b059f172a) profile=base ;;
 e3a915a43db9289cdcaf702df9e7d50d5d4a94fd681ed03d1ba387c3623d3784) profile=touchpad ;;
 1ecd0ea8ede687a8752227e602137b78b257b09a22100a71b6840720185194a3) profile=touchpad ;;
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
 base-accelerated:0ffb168d7113ba1c8f3a9d44de20e2b6f8374af90169670f4b1062709c84d44e|base-recovery:5a09c3b782544044fd596a65332a42f2f3a494b4f4eadd693dd1f06b059f172a|touchpad-accelerated:e3a915a43db9289cdcaf702df9e7d50d5d4a94fd681ed03d1ba387c3623d3784|touchpad-recovery:1ecd0ea8ede687a8752227e602137b78b257b09a22100a71b6840720185194a3) ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
