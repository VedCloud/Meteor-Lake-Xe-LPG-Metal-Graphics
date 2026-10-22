#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../../.." && pwd)"
TARGET="$USB/EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
[ "$(hash "$TARGET/VoodooPS2Keyboard.kext/Contents/Info.plist")" = "30e4fa224169d584ad935c25a8031a064584cd9dab5427351ce854867d6dbbf0" ] || { echo "Input candidate changed; refusing overwrite." >&2; exit 2; }
[ "$(hash "$TARGET/VoodooPS2Keyboard.kext/Contents/MacOS/VoodooPS2Keyboard")" = "74d4d665b5a8afacf8af569193b0a4cf97aabe7175766de86cddad31bcc756ef" ] || { echo "Input candidate changed; refusing overwrite." >&2; exit 2; }
[ "$(hash "$TARGET/VoodooPS2Mouse.kext/Contents/Info.plist")" = "d6014cfd191cdb5dd110b49278fcb3f8940f5a295ddf83df1a82f72ccbcca9b9" ] || { echo "Input candidate changed; refusing overwrite." >&2; exit 2; }
[ "$(hash "$TARGET/VoodooPS2Mouse.kext/Contents/MacOS/VoodooPS2Mouse")" = "355c494d1bdb901859251897237cc7e7561ac4c73c2d2988634921f3add5002b" ] || { echo "Input candidate changed; refusing overwrite." >&2; exit 2; }
BACKUP=$(mktemp -d "$HERE/InputRecovery/applied-rollback.XXXXXX")
/usr/bin/codesign --verify --strict "$HERE/InputRecovery/VoodooPS2Keyboard.kext"
/bin/cp -R "$TARGET/VoodooPS2Keyboard.kext" "$BACKUP/"
/bin/cp -R "$HERE/InputRecovery/VoodooPS2Keyboard.kext" "$TARGET/VoodooPS2Keyboard.kext.restore.kext"
/bin/rm -rf "$TARGET/VoodooPS2Keyboard.kext"
/bin/mv "$TARGET/VoodooPS2Keyboard.kext.restore.kext" "$TARGET/VoodooPS2Keyboard.kext"
/usr/bin/codesign --verify --strict "$HERE/InputRecovery/VoodooPS2Mouse.kext"
/bin/cp -R "$TARGET/VoodooPS2Mouse.kext" "$BACKUP/"
/bin/cp -R "$HERE/InputRecovery/VoodooPS2Mouse.kext" "$TARGET/VoodooPS2Mouse.kext.restore.kext"
/bin/rm -rf "$TARGET/VoodooPS2Mouse.kext"
/bin/mv "$TARGET/VoodooPS2Mouse.kext.restore.kext" "$TARGET/VoodooPS2Mouse.kext"
echo "Original PS2 keyboard/mouse restored. Reboot through this USB. Graphics configuration retained."
