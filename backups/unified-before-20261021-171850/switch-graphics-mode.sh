#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
 58b92c950a5af3fec45353780b87817035306f3aa2b2567284792c58425b47bc) profile=base ;;
 5e9eb8f400581e6c59b95f81954f90b50f1dc2aec39f5908f62278d21a023e3c) profile=base ;;
 fdd43dd6972225cc1f472567cdb0a1bda5a89e8fa43f836abef4d3dece49fa9b) profile=touchpad ;;
 ae3f3944c1bcd1c84ce757680875146c92de673bdf163e1eae78066baf9cf248) profile=touchpad ;;
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
 base-accelerated:58b92c950a5af3fec45353780b87817035306f3aa2b2567284792c58425b47bc|base-recovery:5e9eb8f400581e6c59b95f81954f90b50f1dc2aec39f5908f62278d21a023e3c|touchpad-accelerated:fdd43dd6972225cc1f472567cdb0a1bda5a89e8fa43f836abef4d3dece49fa9b|touchpad-recovery:ae3f3944c1bcd1c84ce757680875146c92de673bdf163e1eae78066baf9cf248) ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
