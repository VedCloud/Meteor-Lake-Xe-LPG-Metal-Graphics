#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(id -u)" = 0 ] || { echo 'Run with sudo on the Dell.'; exit 2; }
LOCK=/private/var/run/latitude-about-this-mac.lock
mkdir "$LOCK" 2>/dev/null || { echo 'Another branding transaction is active, or its lock needs inspection.'; exit 2; }
trap 'rmdir "$LOCK" 2>/dev/null || true' EXIT

BACKUP="${1:-}"
if [ -z "$BACKUP" ]; then
  BACKUP=$(/usr/libexec/PlistBuddy -c 'Print :backup' '/Library/Application Support/LatitudeBranding/latest.plist')
fi
case "$BACKUP" in
  '/Library/Application Support/LatitudeBranding/'0.3.11-*) ;;
  *) echo 'Expected an original LatitudeBranding backup path.'; exit 2;;
esac
codesign --verify --strict "$HERE/AboutThisMac/latitude-about-this-mac"
"$HERE/AboutThisMac/latitude-about-this-mac" restore "$BACKUP"
