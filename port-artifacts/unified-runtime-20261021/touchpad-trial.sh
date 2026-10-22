#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../.." && pwd)"
OC="$USB/EFI/OC"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "${1:-}" in
 apply) from=Base; to=Touchpad; expect_cfg=0ffb168d7113ba1c8f3a9d44de20e2b6f8374af90169670f4b1062709c84d44e; expect_dsdt=4a1803ab03293335ca327b6b7c8978d5c89f9339fdad8218c9163637cf872869;;
 restore) from=Touchpad; to=Base; expect_cfg=e3a915a43db9289cdcaf702df9e7d50d5d4a94fd681ed03d1ba387c3623d3784; expect_dsdt=aa18d3f14961a90f98699aca4514bda448c1d7072e7719afd97afe2b5753f686;;
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
