#!/bin/bash
set -uo pipefail
J=/Library/NullMoth/latitude-desktop-journal
stage() { "$J" stage "$1" "${2:-}" "${3:-0}" || echo "Journal stage failed: $1"; }
# Preserve the pre-created attempt file across fallback boots; each record has a boot UUID.
stage loader-start smoothness0.3.11
"$J" watch 120 >/dev/null 2>&1 &
journal_child=$!
bounded() {
  local seconds=$1 child watcher result
  shift
  "$@" & child=$!
  ( sleep "$seconds"; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & watcher=$!
  wait "$child"; result=$?
  kill "$watcher" 2>/dev/null; wait "$watcher" 2>/dev/null
  return "$result"
}
date
sysctl kern.boottime
CLAIM_FILE=$(mktemp /private/var/tmp/latitude-panel-claim.XXXXXX) || exit 2
stream_child=''; snapshot_child=''; sample_child=''; early_child=''; early_flush_child=''
cleanup() {
  for pid in "$stream_child" "$snapshot_child" "$sample_child" "$journal_child" "$early_child" "$early_flush_child"; do [ -z "$pid" ] || kill "$pid" 2>/dev/null; done
  rm -f "$CLAIM_FILE"
}
trap cleanup EXIT
# Start before the panel query: an early attach/HID panic can occur while
# that query waits. Separate files preserve this boot across fallback.
EARLY=/Library/Logs/LatitudeDesktopEarly.log
[ ! -f "$EARLY" ] || mv -f "$EARLY" "$EARLY.previous"
umask 077
{ date; sysctl kern.boottime; } > "$EARLY"
/usr/bin/log stream --style compact --level info --timeout 120 --predicate '(process == "kernel" OR process == "WindowServer") AND (logType == "error" OR logType == "fault" OR composedMessage CONTAINS[c] "panic" OR composedMessage CONTAINS[c] "NMX" OR composedMessage CONTAINS[c] "Latitude" OR composedMessage CONTAINS[c] "drmn" OR composedMessage CONTAINS[c] "i915" OR composedMessage CONTAINS[c] "NullMoth")' >> "$EARLY" 2>&1 &
early_child=$!
(
  for ((tick=0; tick<60; tick++)); do
    sleep 2
    "$J" flush "$EARLY" || true
  done
) & early_flush_child=$!
stage early-capture-start
claim_ready=0
for ((attempt=0; attempt<10; attempt++)); do
  stage claim-query-begin "$attempt"
  bounded 3 /usr/sbin/ioreg -r -c LatitudePanelClaim -l -w0 > "$CLAIM_FILE" 2>&1; result=$?
  stage claim-query-end "$attempt" "$result"
  if [ "$result" = 0 ] && grep -Fq 'class LatitudePanelClaim' "$CLAIM_FILE"; then claim_ready=1; break; fi
  sleep 1
done
if [ "$claim_ready" = 0 ]; then stage fallback-skip 'No panel reservation'; echo 'No panel reservation active; skipping framebuffer load.'; exit 0; fi
stage claim-active
umask 077
LIVE=/Library/Logs/LatitudeDesktopLive.log
SNAP=/Library/Logs/LatitudeDesktopRegistry.log
SAMPLE=/Library/Logs/LatitudeWindowServerSample.txt
SAMPLEERR=/Library/Logs/LatitudeWindowServerSample.stderr.txt
LATESAMPLE=/Library/Logs/LatitudeWindowServerLateSample.txt
LATEERR=/Library/Logs/LatitudeWindowServerLateSample.stderr.txt
for saved in "$LIVE" "$SNAP" "$SAMPLE" "$SAMPLEERR" "$LATESAMPLE" "$LATEERR"; do [ ! -f "$saved" ] || mv -f "$saved" "$saved.previous"; done
{ date; sysctl kern.boottime; } > "$LIVE"
"$J" flush "$LIVE" || true
stage capture-begin
# Keep graphics diagnostics without enabling WindowServer's high-volume HID debug stream.
/usr/bin/log stream --style compact --level info --timeout 120 --predicate '(process == "WindowServer" AND (logType == "error" OR logType == "fault" OR subsystem BEGINSWITH "com.apple.coreanimation" OR composedMessage CONTAINS[c] "NullMoth" OR composedMessage CONTAINS[c] "fence" OR composedMessage CONTAINS[c] "GPU")) OR (process == "kernel" AND (composedMessage CONTAINS[c] "NMX" OR composedMessage CONTAINS[c] "Latitude" OR composedMessage CONTAINS[c] "drmn" OR composedMessage CONTAINS[c] "i915"))' >> "$LIVE" 2>&1 &
stream_child=$!
(
  for delay in 2 8 20; do
    sleep "$delay"
    date
    for class in IOFramebuffer NMXAccel LatitudePanelClaim; do
      stage registry-query-begin "$class"
      bounded 5 /usr/sbin/ioreg -r -c "$class" -l -w0; result=$?
      stage registry-query-end "$class" "$result"
    done
    bounded 3 /bin/ps -axo pid,uid,pcpu,comm
    "$J" flush "$SNAP" || true
  done
) > "$SNAP" 2>&1 &
snapshot_child=$!
(
 for phase in early late; do
  if [ "$phase" = early ]; then sleep 8
  else sleep 37; SAMPLE=$LATESAMPLE; SAMPLEERR=$LATEERR; fi
  # Resolve a positive PID; sample(name) returned process0/exit0 without a file.
  pid=$(/usr/bin/pgrep -x WindowServer | /usr/bin/head -n 1)
  case "$pid" in ''|*[!0-9]*) result=2; echo 'No valid WindowServer PID for sample' > "$SAMPLEERR" ;;
    *)
      if [ "$pid" -le 1 ]; then result=2; echo 'Refusing sample PID0/1' > "$SAMPLEERR"
      else
        stage sample-begin "$pid"
        bounded 25 /usr/bin/sample "$pid" 3 10 -file "$SAMPLE" > "$SAMPLEERR" 2>&1; result=$?
        if [ "$result" = 0 ] && { [ ! -s "$SAMPLE" ] || /usr/bin/grep -Eq 'pid_for_task.*failed|Sampling process 0' "$SAMPLEERR"; }; then
          result=2; echo 'Sample reported success without a usable target stack' >> "$SAMPLEERR"
        fi
      fi ;;
  esac
  "$J" flush "$SAMPLE" || true
  "$J" flush "$SAMPLEERR" || true
  stage sample-end WindowServer "$result"
 done
) & sample_child=$!
stage framebuffer-load-begin
bounded 60 /usr/bin/kmutil load --load-style start-and-match -b com.nullmoth.LatitudeMTLFB
result=$?
stage framebuffer-load-end '' "$result"
echo "Framebuffer boot-load exit=$result"
# Query-only Vulkan enumeration: no device/queue/allocation/rendering tests.
# Do not delay framebuffer startup or change its result on reporting failure.
if [ "$result" = 0 ]; then
  IDLOG=/Library/Logs/LatitudeGPUIdentity.log
  [ ! -f "$IDLOG" ] || mv -f "$IDLOG" "$IDLOG.previous"
  { date; sysctl kern.boottime; } > "$IDLOG"
  stage gpu-identity-begin
  bounded 20 /Library/NullMoth/latitude-gpu-identity >> "$IDLOG" 2>&1
  identity_result=$?
  stage gpu-identity-end '' "$identity_result"
  "$J" flush "$IDLOG" || true
fi
wait "$stream_child"; stream_result=$?; stream_child=''
stage capture-end '' "$stream_result"
wait "$snapshot_child" 2>/dev/null; snapshot_child=''
wait "$sample_child" 2>/dev/null; sample_child=''
"$J" flush "$LIVE" || true
exit "$result"
