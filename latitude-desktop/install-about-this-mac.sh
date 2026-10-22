#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(id -u)" = 0 ] || { echo 'Run with sudo on the Dell.'; exit 2; }
LOCK=/private/var/run/latitude-about-this-mac.lock
mkdir "$LOCK" 2>/dev/null || { echo 'Another branding transaction is active, or its lock needs inspection.'; exit 2; }
trap 'rmdir "$LOCK" 2>/dev/null || true' EXIT

cd "$HERE"
shasum -a 256 -c SHA256SUMS.txt > /dev/null
codesign --verify --strict "$HERE/AboutThisMac/latitude-about-this-mac"
"$HERE/AboutThisMac/latitude-about-this-mac" install "$HERE/AboutThisMac"

"$HERE/AboutThisMac/latitude-about-this-mac" refresh-icons "$HERE/AboutThisMac"
