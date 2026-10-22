#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(/usr/bin/uname -m)" = x86_64 ] || { echo 'Run this Intel candidate test on the Dell.' >&2; exit 2; }
( cd "$HERE"; /usr/bin/shasum -a 256 -c NATIVE-SHA256SUMS.txt )
REPORT="$HERE/reports/native-text-$(date +%Y%m%d-%H%M%S).txt"
mkdir -p "$HERE/reports"
# One process only; never set launchctl/global DYLD variables.
DYLD_INSERT_LIBRARIES="$HERE/libLatitudeLCDNative-x86_64.dylib" "$HERE/test-interpose-x86_64" 2>&1 | /usr/bin/tee "$REPORT"
DYLD_INSERT_LIBRARIES="$HERE/libLatitudeLCDNative-x86_64.dylib" "$HERE/test-appkit-x86_64" 2>&1 | /usr/bin/tee -a "$REPORT"
echo "Saved: $REPORT"
