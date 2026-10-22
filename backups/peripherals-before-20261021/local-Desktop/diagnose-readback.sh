#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
PROFILE=${1:-fresh-pools}
case "$PROFILE" in fresh-pools|fresh-staging) ;; *) echo 'Use fresh-pools or fresh-staging; command-pool reuse has been removed.' >&2; exit 2 ;; esac
[ "$(id -u)" = 0 ] || { echo 'Run with sudo on the Dell.' >&2; exit 2; }
echo "Focused fresh-pool check: $PROFILE. Same three warmup tests, then 1000 padded frames."
/bin/bash "$HERE/test-readback.sh" 1000 "$PROFILE"
result=$?
echo "COMPARISON profile=$PROFILE exit=$result"
echo 'This focused check never arms desktop. The full test-and-arm-desktop.sh suite is required.'
exit "$result"
