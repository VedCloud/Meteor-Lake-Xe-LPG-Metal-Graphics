#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../.." && pwd)"
OC="$USB/EFI/OC"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "${1:-}" in
 apply) from=Base; to=Touchpad; expect_cfg=1b4b914f76114ffe99e06d8d9f14cc015868861178a85ffd6b30cebd5a989e11; expect_dsdt=e79df56999ebd7ee99036ff2a1c7a0c503c34a45b4ae6ff769b2b69cfe88d9ad;;
 restore) from=Touchpad; to=Base; expect_cfg=e3797474539d32550019e8b79b886aaa9ca590133f05209ba55a40ac9c6ab8ae; expect_dsdt=49a29171b50b85af76e8fbda2ec014e972584cb38a652e1f36e4e924515d2d9a;;
 *) echo 'Usage: /bin/bash touchpad-trial.sh apply|restore'; exit 2;;
esac
[ "$(hash "$OC/config.plist")" = "$expect_cfg" ] || { echo 'Config changed; refusing to overwrite it.' >&2; exit 2; }
[ "$(hash "$OC/ACPI/DSDT.aml")" = "$expect_dsdt" ] || { echo 'DSDT changed; refusing to overwrite it.' >&2; exit 2; }
check_tree() { ( cd "$1"; /usr/bin/shasum -a 256 -c "$2" >/dev/null ); }
check_tree "$HERE" "$HERE/TRIAL-SHA256SUMS.txt"
for hid_file in Contents/Info.plist Contents/MacOS/VoodooI2CHID; do
 [ "$(hash "$OC/Kexts/VoodooI2CHID.kext/$hid_file")" = "$(hash "$HERE/$from/VoodooI2CHID.kext/$hid_file")" ] || { echo 'HID bundle changed; refusing overwrite.' >&2; exit 2; }
done
copy_hid() {
 /bin/rm -rf "$OC/Kexts/VoodooI2CHID.kext.input-new"
 /bin/cp -R "$HERE/$1/VoodooI2CHID.kext" "$OC/Kexts/VoodooI2CHID.kext.input-new"
 /bin/rm -rf "$OC/Kexts/VoodooI2CHID.kext"
 /bin/mv "$OC/Kexts/VoodooI2CHID.kext.input-new" "$OC/Kexts/VoodooI2CHID.kext"
}
rollback() {
 copy_hid "$from"
 /bin/cp "$HERE/$from/config.plist" "$OC/config.plist"
 /bin/cp "$HERE/$from/DSDT.aml" "$OC/ACPI/DSDT.aml"
 if [ "$from" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext" "$OC/Kexts/LatitudeGPIO.kext"; else /bin/rm -rf "$OC/Kexts/LatitudeGPIO.kext"; /bin/cp -R "$HERE/Touchpad/LatitudeGPIO.kext" "$OC/Kexts/"; fi
}
trap 'rollback; echo "Trial operation failed; previous config restored." >&2' ERR
if [ "$to" = Touchpad ]; then
 [ ! -e "$OC/Kexts/LatitudeI2CMeteorLake.kext" ] || { echo 'Unexpected existing controller bundle; refusing.' >&2; exit 2; }
 [ ! -e "$OC/Kexts/LatitudeGPIO.kext" ] || { echo "Unexpected GPIO bundle; refusing." >&2; exit 2; }
 /bin/cp -R "$HERE/Touchpad/LatitudeGPIO.kext" "$OC/Kexts/"
 /bin/cp -R "$HERE/Touchpad/LatitudeI2CMeteorLake.kext" "$OC/Kexts/"
fi
copy_hid "$to"
/bin/cp "$HERE/$to/DSDT.aml" "$OC/ACPI/DSDT.aml"
/bin/cp "$HERE/$to/config.plist" "$OC/config.plist"
if [ "$to" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext" "$OC/Kexts/LatitudeGPIO.kext"; fi
trap - ERR
/bin/sync
echo "USB $to configuration selected. Reboot through this USB."
