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
AGENT="$HOME/Library/LaunchAgents/com.ved.latitude-tablet-keyboard.plist"
LOGS="$HOME/Library/Logs/LatitudeTablet"
mkdir -p "$(dirname "$AGENT")" "$LOGS"
/bin/launchctl bootout "gui/$(id -u)/com.ved.latitude-tablet-keyboard" 2>/dev/null || true
[ ! -e "$AGENT" ] || /bin/cp "$AGENT" "$AGENT.previous"
/bin/rm -f "$AGENT"
/usr/bin/plutil -create xml1 "$AGENT"
/usr/bin/plutil -insert Label -string com.ved.latitude-tablet-keyboard "$AGENT"
/usr/bin/plutil -insert ProgramArguments -xml '<array/>' "$AGENT"
/usr/bin/plutil -insert ProgramArguments.0 -string "$TARGET/Contents/MacOS/latitude-tablet-keyboard" "$AGENT"
/usr/bin/plutil -insert RunAtLoad -bool YES "$AGENT"
/usr/bin/plutil -insert StandardErrorPath -string "$LOGS/keyboard-errors.log" "$AGENT"
/bin/launchctl bootstrap "gui/$(id -u)" "$AGENT"
echo "Installed: $TARGET"
echo 'Enable this app under System Settings > Privacy & Security > Accessibility.'
echo 'Manual keyboard test (no tablet signal required):'
printf 'open "%s" --args --manual\n' "$TARGET"
echo 'Automatic mode shows the panel only for an editable text field while live tablet mode is qualified. After this USB boot, unfold once and fold again to qualify the switch.'
echo 'A per-user login agent was installed. After granting Accessibility, rerun this installer to start it.'
