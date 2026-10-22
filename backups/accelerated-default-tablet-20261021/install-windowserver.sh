#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
LOG="$HERE/windowserver-install-$(date +%Y%m%d-%H%M%S).txt"
/bin/bash "$HERE/Desktop/install-windowserver.sh" "$@" 2>&1 | tee "$LOG"
result=${PIPESTATUS[0]}
echo "WINDOWSERVER INSTALL exit=$result" | tee -a "$LOG"
exit "$result"
