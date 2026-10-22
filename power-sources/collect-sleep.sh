#!/bin/bash
# Read-only Dell collection. Never requests sleep, clears assertions or changes pmset.
set -uo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run this on the Dell, not the preparation Mac.' >&2; exit 2; }
phase=${1:-snapshot}
case "$phase" in before|after|snapshot) ;; *) echo 'Usage: collect-sleep.sh [before|after|snapshot]' >&2; exit 2;; esac
HERE=$(cd "$(dirname "$0")" && pwd)
OUT="$HERE/reports/sleep-$(date -u +%Y%m%d-%H%M%S)-$phase"
mkdir -p "$HERE/reports" || exit 2
mkdir "$OUT" || exit 2
printf 'Sleep diagnostics: %s\nEach query is bounded; no sleep/power settings are changed.\n' "$phase"
capture() {
 local name=$1 seconds=$2 child watch result
 shift 2
 "$@" > "$OUT/$name.txt" 2> "$OUT/$name.stderr.txt" & child=$!
 ( sleep "$seconds"; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watch=$!
 wait "$child"; result=$?
 kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 printf '%s exit=%s\n' "$name" "$result" >> "$OUT/status.txt"
}
{ date -u; printf 'phase=%s\n' "$phase"; /usr/bin/sw_vers; /usr/sbin/sysctl kern.boottime kern.bootsessionuuid; } > "$OUT/context.txt" 2>&1
capture settings 3 /usr/bin/pmset -g custom
capture assertions 3 /usr/bin/pmset -g assertions
capture capabilities 3 /usr/bin/pmset -g cap
capture scheduled-wakes 3 /usr/bin/pmset -g sched
capture power-domain 4 /usr/sbin/ioreg -r -l -w0 -c IOPMrootDomain
capture lid-state 3 /usr/sbin/ioreg -r -l -w0 -c AppleACPIButton
capture gpu-power 3 /usr/sbin/ioreg -r -l -w0 -c NMXGPU
capture panel-power 3 /usr/sbin/ioreg -r -l -w0 -c NMXVGAFB
capture loaded-kexts 8 /usr/bin/kmutil showloaded
capture power-history 5 /usr/bin/pmset -g log
# Preserve the recent tail separately without falsely interpreting an idle-sleep
# assertion as proof that explicit menu/lid sleep is blocked.
/usr/bin/tail -n 4000 "$OUT/power-history.txt" > "$OUT/power-history-recent.txt"
capture power-unified 12 /usr/bin/log show --last 30m --style compact --info --predicate 'process == "powerd" OR (process == "kernel" AND (eventMessage CONTAINS[c] "sleep" OR eventMessage CONTAINS[c] "wake" OR eventMessage CONTAINS[c] "NullMoth GPU:"))'
printf 'Saved: %s\n' "$OUT"
echo 'An exit of 143/137 means that query timed out; partial output is retained.'
if [ "$phase" = before ]; then echo 'Now try Apple menu → Sleep. If it stays awake, run this script again with after.'; fi
