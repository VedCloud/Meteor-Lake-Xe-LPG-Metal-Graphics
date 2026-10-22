#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../../../.." && pwd)"
TARGET="$USB/EFI/OC/Kexts/AirportItlwm.kext"
case "${1:-}" in apply|restore) action=$1;; *) echo 'Usage: trial-awdl.sh apply|restore'; exit 2;; esac
[ -f "$USB/EFI/OC/config.plist" ] || { echo 'Expected USB EFI configuration is missing.' >&2; exit 2; }
verify() { ( cd "$1"; /usr/bin/shasum -a 256 -c "$2" >/dev/null 2>&1 ); }
verify "$HERE" "$HERE/SHA256SUMS.txt" || { echo 'Candidate package verification failed.' >&2; exit 2; }
if [ "$action" = apply ]; then
 if verify "$TARGET" "$HERE/CANDIDATE-BUNDLE-SHA256SUMS.txt"; then echo 'AWDL candidate is already selected on the USB.'; exit 0; fi
 { verify "$TARGET" "$HERE/WORKING-BUNDLE-SHA256SUMS.txt" || verify "$TARGET" "$HERE/PREVIOUS-BUNDLE-SHA256SUMS.txt" || verify "$TARGET" "$HERE/PRIOR-0.1.1-BUNDLE-SHA256SUMS.txt"; } || { echo 'Current Wi-Fi driver is not one of the recognized original/0.1.0/0.1.1 bundles; refusing overwrite.' >&2; exit 2; }
 SOURCE="$HERE/AirportItlwm.kext"
else
 if verify "$TARGET" "$HERE/WORKING-BUNDLE-SHA256SUMS.txt"; then echo 'Working Wi-Fi driver is already selected on the USB.'; exit 0; fi
 { verify "$TARGET" "$HERE/CANDIDATE-BUNDLE-SHA256SUMS.txt" || verify "$TARGET" "$HERE/PREVIOUS-BUNDLE-SHA256SUMS.txt" || verify "$TARGET" "$HERE/PRIOR-0.1.1-BUNDLE-SHA256SUMS.txt"; } || { echo 'Current Wi-Fi driver differs from the recognized candidate versions; refusing overwrite.' >&2; exit 2; }
 SOURCE="$HERE/KnownWorking/AirportItlwm.kext"
fi
NEXT="$TARGET.awdl-staged"
[ ! -e "$NEXT" ] || { echo 'An earlier staging directory exists; no changes made.' >&2; exit 2; }
mkdir -p "$HERE/InstalledBackups"
SAVE=$(mktemp -d "$HERE/InstalledBackups/trial.XXXXXX")
moved=0
rollback() {
 result=$?
 if [ "$moved" = 1 ] && [ ! -e "$TARGET" ]; then /bin/mv "$SAVE/AirportItlwm.kext" "$TARGET" || true; fi
 if [ -d "$NEXT" ]; then /bin/rm -rf "$NEXT"; fi
 exit "$result"
}
trap rollback EXIT
/usr/bin/ditto "$SOURCE" "$NEXT"
if [ "$action" = apply ]; then
 /usr/bin/codesign --verify --strict "$NEXT"
 verify "$NEXT" "$HERE/CANDIDATE-BUNDLE-SHA256SUMS.txt"
else
 verify "$NEXT" "$HERE/WORKING-BUNDLE-SHA256SUMS.txt"
fi
/bin/mv "$TARGET" "$SAVE/AirportItlwm.kext"; moved=1
/bin/mv "$NEXT" "$TARGET"; moved=0
trap - EXIT
echo "USB Wi-Fi selection: $action. Previous bundle: $SAVE"
echo 'Reboot through this USB to load the selected bundle. No live driver unload/reload was performed.'
echo 'Graphics, keyboard mapping and boot arguments were retained. The raw-radio gate was not enabled.'
