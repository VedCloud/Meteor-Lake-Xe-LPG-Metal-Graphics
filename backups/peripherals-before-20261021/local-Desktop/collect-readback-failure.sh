#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(id -u)" = 0 ] || { echo 'Run with sudo on the Dell.' >&2; exit 2; }
LATEST=''
for candidate in "$HERE"/reports/readback-[0-9]*; do
  [ -d "$candidate" ] && LATEST=$candidate
done
[ -n "$LATEST" ] || { echo 'No readback report found.' >&2; exit 2; }
OUT="$HERE/reports/readback-failure-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT"
printf 'Source report: %s\n' "$LATEST" | tee "$OUT/source.txt"
# Read the recorded failure's time, not "last 20m" on a later fallback boot.
last_epoch=0
for artifact in "$LATEST"/*.txt; do
  [ -f "$artifact" ] || continue
  epoch=$(stat -f %m "$artifact") || exit 2
  [ "$epoch" -le "$last_epoch" ] || last_epoch=$epoch
done
[ "$last_epoch" -gt 0 ] || { echo 'Latest report has no timestamped evidence'; exit 2; }
start_epoch=$((last_epoch-150));end_epoch=$((last_epoch+60))
# Include driver registration during the report's boot, even if no case ran.
boot_epoch=$(sed -n 's/.*sec = \([0-9]*\),.*/\1/p' "$LATEST/boot.txt" 2>/dev/null)
case "$boot_epoch" in ''|*[!0-9]*) ;; *) start_epoch=$((boot_epoch-5));; esac
START=$(date -r "$start_epoch" '+%Y-%m-%d %H:%M:%S')
END=$(date -r "$end_epoch" '+%Y-%m-%d %H:%M:%S')
printf 'start=%s\nend=%s\n' "$START" "$END" | tee "$OUT/window.txt"
for artifact in results.txt boot.txt capture-window.txt accelerator-before.txt loaded-kexts.txt plugin.plist binaries.txt; do
  [ ! -f "$LATEST/$artifact" ] || cp "$LATEST/$artifact" "$OUT/"
done
[ -f "$OUT/results.txt" ] || printf 'Suite stopped before recording case results; see source report and registration evidence.\n' > "$OUT/results.txt"
printf 'Reading only this failure interval (maximum 45 seconds). No GPU tests or boot changes.\n'
log show --start "$START" --end "$END" --style compact --predicate '(process == "kernel" AND (eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "drmn" OR eventMessage CONTAINS[c] "GPU HANG" OR eventMessage CONTAINS[c] "GuC" OR eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "Latitude")) OR eventMessage CONTAINS[c] "NullMoth" OR subsystem == "org.nullmoth.libdrm"' > "$OUT/failure-window.txt" 2> "$OUT/log-stderr.txt" &
child=$!
( sleep 45; echo 'TIMEOUT: partial historical log retained' > "$OUT/history-timeout.txt"; kill -TERM "$child" 2>/dev/null; sleep 2; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 &
watcher=$!
wait "$child"; result=$?
kill "$watcher" 2>/dev/null; wait "$watcher" 2>/dev/null
printf 'history exit=%s\n' "$result" | tee "$OUT/status.txt"
dmesg > "$OUT/dmesg.txt" 2>&1
sysctl kern.boottime > "$OUT/current-boot.txt" 2>&1
sysctl kern.bootargs > "$OUT/boot-arguments.txt" 2>&1
ioreg -r -c NMXAccel -l -w0 > "$OUT/accelerator.txt" 2>&1
for marker in nullmoth-panel-once nullmoth-gpu-once; do nvram "$marker"; done > "$OUT/boot-markers.txt" 2>&1
printf 'Saved failure evidence: %s\n' "$OUT"
if [ "$result" != 0 ]; then echo 'Historical capture was incomplete; see status and timeout files.'; fi
exit 0
