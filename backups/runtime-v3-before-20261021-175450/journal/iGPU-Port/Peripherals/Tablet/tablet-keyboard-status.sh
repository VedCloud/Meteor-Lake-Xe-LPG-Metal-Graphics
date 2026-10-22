#!/bin/bash
set -euo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run on the Dell.'; exit 2; }
APP="$HOME/Applications/Latitude Tablet Keyboard.app/Contents/MacOS/latitude-tablet-keyboard"
[ -x "$APP" ] || { echo 'Tablet keyboard app is not installed for this user.'; exit 2; }
"$APP" --diagnose
/bin/launchctl print "gui/$(id -u)/com.ved.latitude-tablet-keyboard" 2>&1 || true
