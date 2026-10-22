#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
LOG="$HERE/windowserver-install-$(date +%Y%m%d-%H%M%S).txt"
/bin/bash "$HERE/Desktop/install-windowserver.sh" "$@" 2>&1 | tee "$LOG"
result=${PIPESTATUS[0]}
if [ "$result" = 0 ]; then
  /bin/bash "$HERE/switch-graphics-mode.sh" accelerated 2>&1 | tee -a "$LOG"
  mode_result=${PIPESTATUS[0]}
  if [ "$mode_result" != 0 ]; then
    echo "Graphics installation succeeded, but selecting persistent accelerated USB mode failed: $mode_result" | tee -a "$LOG"
    result=$mode_result
  else echo 'Accelerated graphics selected for every normal USB boot; recovery is manual.' | tee -a "$LOG"; fi
fi
echo "WINDOWSERVER INSTALL exit=$result" | tee -a "$LOG"
exit "$result"
