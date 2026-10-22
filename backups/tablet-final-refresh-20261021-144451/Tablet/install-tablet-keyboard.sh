#!/bin/bash
set -euo pipefail
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run as your desktop user on the Intel Dell.' >&2; exit 2; }
[ "$(id -u)" != 0 ] || { echo 'Run without sudo, as the logged-in desktop user.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
APP='Latitude Tablet Keyboard.app'
/usr/bin/codesign --verify --strict "$HERE/$APP"
mkdir -p "$HOME/Applications"
TARGET="$HOME/Applications/$APP"
if [ -e "$TARGET" ]; then
 BACKUP=$(mktemp -d "$HOME/Applications/LatitudeKeyboardBackup.XXXXXX")
 /bin/cp -R "$TARGET" "$BACKUP/$APP"
 /bin/rm -rf "$TARGET"
fi
/bin/cp -R "$HERE/$APP" "$TARGET"
/usr/bin/codesign --verify --strict "$TARGET"
echo "Installed: $TARGET"
echo 'Enable this app under System Settings > Privacy & Security > Accessibility.'
echo 'Manual keyboard test (no tablet signal required):'
printf 'open "%s" --args --manual\n' "$TARGET"
echo 'Automatic mode remains gated on a qualified live tablet-state publisher; that transport is not implemented in this candidate.'
echo 'No login item or automatic startup has been enabled.'
