#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../.." && pwd)"
OC="$USB/EFI/OC"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "${1:-}" in
 apply) from=Base; to=Touchpad; expect_cfg=c07f2ad8514d7cff3b0d0a9dd0e1975b72e1e2188e6de63fd30a54961d32d3d7; expect_dsdt=e79df56999ebd7ee99036ff2a1c7a0c503c34a45b4ae6ff769b2b69cfe88d9ad;;
 restore) from=Touchpad; to=Base; expect_cfg=0c07e9bd6cd37002e8cc5f46fede408a451e8d505ab48155fa99a53cd397f8f0; expect_dsdt=a2393119f4ac1c9bdf9ede1a2bc733d286aa5c8db6131c717fbbca22eaeadacd;;
 *) echo 'Usage: /bin/bash touchpad-trial.sh apply|restore'; exit 2;;
esac
[ "$(hash "$OC/config.plist")" = "$expect_cfg" ] || { echo 'Config changed; refusing to overwrite it.' >&2; exit 2; }
[ "$(hash "$OC/ACPI/DSDT.aml")" = "$expect_dsdt" ] || { echo 'DSDT changed; refusing to overwrite it.' >&2; exit 2; }
check_tree() { ( cd "$1"; /usr/bin/shasum -a 256 -c "$2" >/dev/null ); }
check_tree "$HERE" "$HERE/TRIAL-SHA256SUMS.txt"
rollback() {
 /bin/cp "$HERE/$from/config.plist" "$OC/config.plist"
 /bin/cp "$HERE/$from/DSDT.aml" "$OC/ACPI/DSDT.aml"
 if [ "$from" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi
}
trap 'rollback; echo "Trial operation failed; previous config restored." >&2' ERR
if [ "$to" = Touchpad ]; then
 [ ! -e "$OC/Kexts/LatitudeI2CMeteorLake.kext" ] || { echo 'Unexpected existing controller bundle; refusing.' >&2; exit 2; }
 /bin/cp -R "$HERE/Touchpad/LatitudeI2CMeteorLake.kext" "$OC/Kexts/"
fi
/bin/cp "$HERE/$to/DSDT.aml" "$OC/ACPI/DSDT.aml"
/bin/cp "$HERE/$to/config.plist" "$OC/config.plist"
if [ "$to" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi
trap - ERR
/bin/sync
echo "USB $to configuration selected. Reboot through this USB."
