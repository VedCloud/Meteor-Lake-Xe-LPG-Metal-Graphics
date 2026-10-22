#!/bin/bash
# Read-only target ABI evidence; no injection, interface creation or radio TX.
set -uo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run on the Dell.' >&2; exit 2; }
HERE=$(cd "$(dirname "$0")" && pwd)
OUT="$HERE/reports/native-integration-$(date -u +%Y%m%d-%H%M%S)"
mkdir -p "$HERE/reports" && mkdir "$OUT" || exit 2
capture() {
 local name=$1 child watch rc;shift
 "$@" > "$OUT/$name.txt" 2> "$OUT/$name.stderr.txt" & child=$!
 ( sleep 8;kill -TERM "$child" 2>/dev/null;sleep 1;kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child";rc=$?;kill "$watch" 2>/dev/null;wait "$watch" 2>/dev/null
 printf '%s exit=%s\n' "$name" "$rc" >> "$OUT/status.txt"
}
capture version /usr/bin/sw_vers
capture awdl-registry /usr/sbin/ioreg -r -l -w0 -c AirportItlwm
capture wifi-properties /usr/sbin/ioreg -r -l -w0 -c IOPCIDevice
capture loaded /usr/bin/kmutil showloaded
binary=/System/Library/Extensions/IO80211Family.kext/Contents/MacOS/IO80211Family
if [ -f "$binary" ]; then
 size=$(/usr/bin/stat -f %z "$binary")
 case "$size" in ''|*[!0-9]*) echo 'IO80211Family size unavailable' >> "$OUT/status.txt";;
 *) if [ "$size" -le 134217728 ]; then
  capture io80211-copy /bin/cp "$binary" "$OUT/IO80211Family.macho"
  if [ -f "$OUT/IO80211Family.macho" ]; then /usr/bin/shasum -a 256 "$OUT/IO80211Family.macho" > "$OUT/IO80211Family.sha256";fi
 fi;; esac
else echo 'Standalone IO80211Family binary absent; kernel-collection ABI needs another export route.' >> "$OUT/status.txt";fi
weather=/System/Applications/Weather.app
if [ -d "$weather" ]; then
 capture weather-signature /usr/bin/codesign -dvvv "$weather"
 capture weather-entitlements /usr/bin/codesign -d --entitlements :- "$weather"
fi
printf 'Saved native integration evidence: %s\nNo radio transmission or system-app injection was performed.\n' "$OUT"
