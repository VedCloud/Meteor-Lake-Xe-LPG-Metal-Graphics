#!/bin/bash
set -u
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run on the Dell in macOS.' >&2; exit 2; }
case "${1:-}" in laptop|tablet|portrait-left|portrait-right|upside-down) ;; *) echo 'Specify laptop, tablet, portrait-left, portrait-right, or upside-down.' >&2; exit 2;; esac
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="$HERE/reports/macos-$(date +%Y%m%d-%H%M%S)-$1"
mkdir -p "$OUT" || exit 2
printf '%s\n' "$1" > "$OUT/physical-position.txt"
# Enumerate the ACPI plane rather than treating an empty name match as success.
/usr/sbin/ioreg -p IOACPIPlane -a -l > "$OUT/acpi-plane.plist" 2> "$OUT/acpi-plane.stderr.txt" & child=$!
( sleep 5; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
if [ "$result" != 0 ] || [ ! -s "$OUT/acpi-plane.plist" ]; then echo "ACPI plane query failed or empty: exit=$result" >> "$OUT/status.txt"; else echo "ACPI plane captured; presence alone does not establish tablet state" >> "$OUT/status.txt"; fi
if [ -x "$HERE/latitude-tablet-state" ]; then
 "$HERE/latitude-tablet-state" > "$OUT/tablet-state.json" 2> "$OUT/tablet-state.stderr.txt" & child=$!
 ( sleep 5; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 echo "Native tablet inventory exit=$result" >> "$OUT/status.txt"
fi
for cls in LatitudeISHTransport LatitudeTabletState LatitudeTabletObserver IOHIDDevice ApplePS2Keyboard ApplePS2Mouse VoodooI2CHIDDevice NMXVGAFB; do
 /usr/sbin/ioreg -r -c "$cls" -l -w0 > "$OUT/registry-$cls.txt" 2>&1 & child=$!
 ( sleep 5; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 echo "$cls exit=$result" >> "$OUT/status.txt"
done
# Record loaded image identity independently from an empty registry match.
/usr/bin/kmutil showloaded > "$OUT/loaded-kexts.txt" 2> "$OUT/loaded-kexts.stderr.txt" & child=$!
( sleep 10; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 echo "Loaded kext query exit=$result (permissions failure is not proof of absent driver)" >> "$OUT/status.txt"
echo "Saved $OUT. Read-only registry snapshot; no rotation or input disabling by the collector; the live-state kext separately reads the firmware mode byte."
