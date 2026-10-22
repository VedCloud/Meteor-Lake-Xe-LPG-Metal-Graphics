#!/bin/bash
set -uo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run this on the Dell desktop.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
REPORTS="$HERE/Desktop/reports"
mkdir -p "$REPORTS" || exit 2
OUT=$(mktemp -d "$REPORTS/motion-$(date +%Y%m%d-%H%M%S)-XXXXXX") || exit 2
pid=$(/usr/bin/pgrep -x WindowServer | /usr/bin/head -n 1)
case "$pid" in ''|*[!0-9]*) echo 'WindowServer is not running.' >&2; exit 2;; esac
[ "$pid" -gt 1 ] || exit 2
{ date; sysctl kern.boottime; sw_vers; sysctl kern.bootargs; } > "$OUT/context.txt" 2>&1
for binary in /Library/GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver /Library/GPUBundles/nvmtl/libvulkan_intel.dylib; do
  [ ! -f "$binary" ] || shasum -a 256 "$binary" >> "$OUT/installed-files.txt"
done
echo 'In 5 seconds: drag a window continuously and move over the Dock for 8 seconds.'
echo 'This captures stacks and CPU use; it does not arm acceleration or run rendering tests.'
sleep 5
echo 'CAPTURING NOW: keep the desktop moving for 8 seconds.'
/usr/bin/sample "$pid" 8 10 -file "$OUT/windowserver-motion.txt" > "$OUT/sample.stderr.txt" 2>&1 &
sample_child=$!
monitor_child=''
watch_child=''
cleanup() {
  for child in "$sample_child" "$monitor_child" "$watch_child"; do
    [ -z "$child" ] || kill -TERM "$child" 2>/dev/null || true
  done
}
trap 'cleanup; exit 130' INT TERM
( sleep 35; kill -TERM "$sample_child" 2>/dev/null; sleep 1; kill -KILL "$sample_child" 2>/dev/null ) >/dev/null 2>&1 &
watch_child=$!
(
  for index in 1 2 3 4 5 6 7 8; do
    date '+%Y-%m-%d %H:%M:%S'
    /bin/ps -p "$pid" -o pid=,etime=,%cpu=,rss=,time=,state=,comm=
    sleep 1
  done
) > "$OUT/windowserver-cpu.txt" 2>&1 &
monitor_child=$!
wait "$sample_child"; result=$?
kill "$watch_child" 2>/dev/null || true
wait "$watch_child" 2>/dev/null || true
wait "$monitor_child" 2>/dev/null || true
trap - INT TERM
[ "$result" != 0 ] || [ -s "$OUT/windowserver-motion.txt" ] || result=2
echo "PID=$pid sample_exit=$result" > "$OUT/status.txt"
echo 'CPU percentages are ps lifetime averages; time is cumulative CPU time for delta analysis. Neither is an animation FPS measurement.' >> "$OUT/status.txt"
echo 'Capture complete. This sample shows work during interaction, not measured frame pacing.'
echo "Saved: $OUT"
exit "$result"
