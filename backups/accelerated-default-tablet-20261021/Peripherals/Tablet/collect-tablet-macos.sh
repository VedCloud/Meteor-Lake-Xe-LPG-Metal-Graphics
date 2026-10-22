#!/bin/bash
set -u
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run on the Dell in macOS.' >&2; exit 2; }
case "${1:-}" in laptop|tablet|portrait-left|portrait-right|upside-down) ;; *) echo 'Specify laptop, tablet, portrait-left, portrait-right, or upside-down.' >&2; exit 2;; esac
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="$HERE/reports/macos-$(date +%Y%m%d-%H%M%S)-$1"
mkdir -p "$OUT" || exit 2
printf '%s\n' "$1" > "$OUT/physical-position.txt"
for node in HIDD ISHC ISH BIND CIND DIND; do
 /usr/sbin/ioreg -r -n "$node" -l -w0 > "$OUT/acpi-$node.txt" 2>&1 & child=$!
 ( sleep 5; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 echo "$node exit=$result" >> "$OUT/status.txt"
done
for cls in IOHIDDevice ApplePS2Keyboard VoodooI2CHIDDevice NMXIntelVGAFB; do
 /usr/sbin/ioreg -r -c "$cls" -l -w0 > "$OUT/registry-$cls.txt" 2>&1 & child=$!
 ( sleep 5; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child"; result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 echo "$cls exit=$result" >> "$OUT/status.txt"
done
echo "Saved $OUT. Read-only registry snapshot; no rotation, input disabling or ACPI method execution."
