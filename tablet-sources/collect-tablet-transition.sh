#!/bin/bash
set -u
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run on the Intel Dell in macOS.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="$HERE/reports/transition-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT" || exit 2
bounded() {
 local dest=$1; shift
 "$@" > "$dest" 2> "$dest.stderr" & local child=$!
 ( sleep 3; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & local watcher=$!
 wait "$child"; local result=$?;kill "$watcher" 2>/dev/null;wait "$watcher" 2>/dev/null
 printf '%s exit=%s\n' "${dest##*/}" "$result" >> "$OUT/status.txt"
}
echo 'Read-only mode transition capture: start with the Dell unfolded in laptop mode.'
echo 'Follow the fold/unfold prompts below. This does not disable input or request rotation.'
for ((i=1;i<=20;i++)); do
 case "$i" in 1) echo 'Keep it unfolded.';; 5) echo 'Fold into tablet mode now; keep it folded.';; 15) echo 'Unfold into laptop mode now.';; esac
 date -u '+%Y-%m-%dT%H:%M:%SZ' > "$OUT/sample-$i.time"
 bounded "$OUT/sample-$i-state.txt" /usr/sbin/ioreg -r -c LatitudeTabletState -l -w0
 sleep 1
done
for cls in ApplePS2Keyboard ApplePS2Mouse; do bounded "$OUT/$cls.txt" /usr/sbin/ioreg -r -c "$cls" -l -w0; done
if ! /usr/bin/grep -q 'LatitudeTabletStateVersion' "$OUT"/sample-*-state.txt; then echo 'No live-state publisher was captured. Keep input suppression disabled; review the ordinary laptop/tablet reports.'; fi
echo "Saved $OUT"
