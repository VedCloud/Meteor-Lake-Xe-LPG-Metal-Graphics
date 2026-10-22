#!/bin/bash
set -euo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run this on the Dell.' >&2; exit 2; }
[ "$(id -u)" != 0 ] || { echo 'Run as your normal logged-in user, without sudo.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
DEST="$HOME/Library/Application Support/LatitudeInput"
AGENT="$HOME/Library/LaunchAgents/com.ved.latitude-brightness.plist"
/bin/mkdir -p "$DEST" "$(dirname "$AGENT")"
/usr/bin/codesign --verify --strict "$HERE/latitude-brightness"
/bin/launchctl bootout "gui/$(id -u)/com.ved.latitude-brightness" 2>/dev/null || true
/bin/cp "$HERE/latitude-brightness" "$DEST/latitude-brightness"
/bin/chmod 755 "$DEST/latitude-brightness"
# plutil constructs/escapes the user's actual home path rather than interpolating XML.
[ ! -e "$AGENT" ] || /bin/cp "$AGENT" "$AGENT.previous"
/bin/rm -f "$AGENT"
/usr/bin/plutil -create xml1 "$AGENT"
/usr/bin/plutil -insert Label -string com.ved.latitude-brightness "$AGENT"
/usr/bin/plutil -insert ProgramArguments -xml '<array/>' "$AGENT"
/usr/bin/plutil -insert ProgramArguments.0 -string "$DEST/latitude-brightness" "$AGENT"
/usr/bin/plutil -insert ProgramArguments.1 -string daemon "$AGENT"
/usr/bin/plutil -insert RunAtLoad -bool YES "$AGENT"
/usr/bin/plutil -insert StandardErrorPath -string "$DEST/brightness-errors.log" "$AGENT"
/bin/launchctl bootstrap "gui/$(id -u)" "$AGENT"
echo "Installed: $DEST/latitude-brightness"
echo 'Allow this binary in System Settings > Privacy & Security > Accessibility, then rerun this installer.'
echo 'F6/F7 (or Control+Option+F6/F7 on an external keyboard) adjust brightness through the existing framebuffer.'
echo 'Manual + or - commands work without Accessibility permission. A frozen keyboard cannot produce shortcuts.'
