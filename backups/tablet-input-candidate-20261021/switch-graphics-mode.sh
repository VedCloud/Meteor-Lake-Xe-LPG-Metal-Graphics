#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
 1b4b914f76114ffe99e06d8d9f14cc015868861178a85ffd6b30cebd5a989e11) profile=base ;;
 df006719f9cbf233df0de95be9a08fe09fa3c16ec0d7c8c64d6ad7d78645fe61) profile=base ;;
 e3797474539d32550019e8b79b886aaa9ca590133f05209ba55a40ac9c6ab8ae) profile=touchpad ;;
 5c712cd72eb1a45c49bda072a0f7618b3a9dc6bb255ec0fd98aa76578bea15ee) profile=touchpad ;;
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
 base-accelerated:1b4b914f76114ffe99e06d8d9f14cc015868861178a85ffd6b30cebd5a989e11|base-recovery:df006719f9cbf233df0de95be9a08fe09fa3c16ec0d7c8c64d6ad7d78645fe61|touchpad-accelerated:e3797474539d32550019e8b79b886aaa9ca590133f05209ba55a40ac9c6ab8ae|touchpad-recovery:5c712cd72eb1a45c49bda072a0f7618b3a9dc6bb255ec0fd98aa76578bea15ee) ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
