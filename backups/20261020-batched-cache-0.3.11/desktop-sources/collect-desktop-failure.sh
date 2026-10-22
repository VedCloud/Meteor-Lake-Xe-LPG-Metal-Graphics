#!/bin/bash
set -uo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run this on the Dell, from its working fallback desktop.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
if [ "$#" -ge 1 ]; then
  START=$1
else
  boot_epoch=''
  for report in "$HERE"/Desktop/reports/readback-[0-9]*; do
    [ ! -f "$report/boot.txt" ] || boot_epoch=$(sed -n 's/.*sec = \([0-9]*\),.*/\1/p' "$report/boot.txt")
  done
  case "$boot_epoch" in ''|*[!0-9]*) boot_epoch=$(($(date +%s)-1200));; esac
  attempt_epoch=$(cat "$HERE/Desktop/reports/windowserver-attempt-start.txt" 2>/dev/null || true)
  case "$attempt_epoch" in ''|*[!0-9]*) ;; *) [ "$attempt_epoch" -le "$boot_epoch" ] || boot_epoch=$attempt_epoch;; esac
  START=$(date -r "$((boot_epoch-5))" '+%Y-%m-%d %H:%M:%S')
fi
END=${2:-$(date '+%Y-%m-%d %H:%M:%S')}
OUT="$HERE/Desktop/reports/failure-detail-$(date +%Y%m%d-%H%M%S)"
mkdir "$OUT" || exit 2
printf 'Reading retained logs from %s to %s. No GPU tests or system changes.\n' "$START" "$END"
printf 'This search has a 90-second limit. Partial output is retained.\n'
{ date; sysctl kern.boottime; sw_vers; } > "$OUT/collection-context.txt" 2>&1

# The loader preserves these across fallback boots; unified logs may lose a forced-off boot.
# Identify the kernel actually loaded on this collection boot; failed-boot
# identity remains in retained live/unified logs and the install report.
kmutil showloaded > "$OUT/current-loaded-kexts.txt" 2>&1 &
identity_child=$!
( sleep 15; kill -TERM "$identity_child" 2>/dev/null; sleep 1; kill -KILL "$identity_child" 2>/dev/null ) >/dev/null 2>&1 &
identity_watcher=$!
wait "$identity_child"; identity_result=$?
kill "$identity_watcher" 2>/dev/null; wait "$identity_watcher" 2>/dev/null
printf 'current kernel identity query exit=%s\n' "$identity_result" >> "$OUT/status.txt"
/usr/sbin/system_profiler SPDisplaysDataType -json > "$OUT/current-displays.json" 2> "$OUT/current-displays.stderr.txt" &
profile_child=$!
( sleep 20; kill -TERM "$profile_child" 2>/dev/null; sleep 1; kill -KILL "$profile_child" 2>/dev/null ) >/dev/null 2>&1 &
profile_watcher=$!
wait "$profile_child"; profile_result=$?
kill "$profile_watcher" 2>/dev/null; wait "$profile_watcher" 2>/dev/null
printf 'current display profile exit=%s\n' "$profile_result" >> "$OUT/status.txt"
# Collection-boot installed file identity, not proof of the failed boot's loaded UUID.
if [ -f /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB ]; then
  shasum -a 256 /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB > "$OUT/current-installed-framebuffer-sha256.txt" 2>&1
  /usr/bin/plutil -p /Library/Extensions/LatitudeMTLFB.kext/Contents/Info.plist > "$OUT/current-installed-framebuffer-info.txt" 2>&1 || true
fi
if [ -f /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU ]; then
  shasum -a 256 /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU > "$OUT/current-installed-kernel-sha256.txt" 2>&1
fi
if [ -f /Library/GPUBundles/nvmtl/libdrm.2.dylib ]; then
  shasum -a 256 /Library/GPUBundles/nvmtl/libdrm.2.dylib > "$OUT/current-installed-transport-sha256.txt" 2>&1
fi
if [ -f /Library/GPUBundles/nvmtl/libvulkan_intel.dylib ]; then
  shasum -a 256 /Library/GPUBundles/nvmtl/libvulkan_intel.dylib > "$OUT/current-installed-backend-sha256.txt" 2>&1
fi
# Sample this currently usable boot as well as retained startup samples.
current_ws_pid=$(/usr/bin/pgrep -x WindowServer | /usr/bin/head -n 1)
case "$current_ws_pid" in ''|*[!0-9]*) echo 'No valid current WindowServer PID' > "$OUT/current-windowserver-sample.status.txt";;
 *) if [ "$current_ws_pid" -gt 1 ]; then
      echo 'Capturing a current WindowServer stack sample (25-second limit).'
      /usr/bin/sample "$current_ws_pid" 3 10 -file "$OUT/current-windowserver-sample.txt" > "$OUT/current-windowserver-sample.stderr.txt" 2>&1 &
      sample_child=$!
      ( sleep 25; kill -TERM "$sample_child" 2>/dev/null; sleep 1; kill -KILL "$sample_child" 2>/dev/null ) >/dev/null 2>&1 & sample_watch=$!
      wait "$sample_child"; sample_result=$?
      kill "$sample_watch" 2>/dev/null; wait "$sample_watch" 2>/dev/null
      if [ "$sample_result" = 0 ] && [ ! -s "$OUT/current-windowserver-sample.txt" ]; then sample_result=2; fi
      echo "PID=$current_ws_pid exit=$sample_result" > "$OUT/current-windowserver-sample.status.txt"
    fi;;
esac
# Bounded input-chain snapshots for the Meteor Lake touchpad trial.
mkdir -p "$OUT/input"
for input_class in LatitudeI2CMeteorLakeController VoodooI2CHIDDevice VoodooI2CPrecisionTouchpadHIDEventDriver VoodooInput; do
  /usr/sbin/ioreg -r -c "$input_class" -l -w0 > "$OUT/input/$input_class.txt" 2>&1 &
  input_child=$!
  ( sleep 5; kill -TERM "$input_child" 2>/dev/null; sleep 1; kill -KILL "$input_child" 2>/dev/null ) >/dev/null 2>&1 & input_watch=$!
  wait "$input_child"; input_result=$?
  kill "$input_watch" 2>/dev/null; wait "$input_watch" 2>/dev/null
  printf '%s exit=%s\n' "$input_class" "$input_result" >> "$OUT/input/status.txt"
done
mkdir -p "$OUT/retained-boot-files"
for saved in LatitudeWindowServerLateSample.txt LatitudeWindowServerLateSample.txt.previous LatitudeWindowServerLateSample.stderr.txt LatitudeWindowServerLateSample.stderr.txt.previous LatitudeGPUIdentity.log LatitudeGPUIdentity.log.previous LatitudeDesktopEarly.log LatitudeDesktopEarly.log.previous LatitudeDesktopAttempt.trace LatitudeDesktopAttempt.trace.previous LatitudeWindowServerSample.txt LatitudeWindowServerSample.txt.previous LatitudeWindowServerSample.stderr.txt LatitudeWindowServerSample.stderr.txt.previous LatitudeDesktopLive.log LatitudeDesktopLive.log.previous LatitudeDesktopRegistry.log LatitudeDesktopRegistry.log.previous LatitudePanelLoad.log; do
  if [ -f "/Library/Logs/$saved" ]; then
    if cp -p "/Library/Logs/$saved" "$OUT/retained-boot-files/$saved"; then
      stat -f '%Sm %z %N' "/Library/Logs/$saved" >> "$OUT/retained-files-status.txt"
    else
      printf 'COPY FAILED %s\n' "$saved" >> "$OUT/retained-files-status.txt"
    fi
  else
    printf 'MISSING %s\n' "$saved" >> "$OUT/retained-files-status.txt"
  fi
done
# Identify the injected HID binary without assuming that disk identity proves
# a failed boot's loaded image. Panic/showloaded UUIDs remain authoritative.
for input_root in "$HERE/../EFI/OC/Kexts" "$HERE/../../EFI/OC/Kexts"; do
  [ -f "$input_root/VoodooI2CHID.kext/Contents/MacOS/VoodooI2CHID" ] || continue
  shasum -a 256 "$input_root/VoodooI2CHID.kext/Contents/MacOS/VoodooI2CHID" > "$OUT/usb-hid-sha256.txt" 2>&1
  /usr/bin/plutil -p "$input_root/VoodooI2CHID.kext/Contents/Info.plist" > "$OUT/usb-hid-info.txt" 2>&1 || true
  break
done
sysctl kern.bootargs > "$OUT/current-boot-arguments.txt" 2>&1
if [ -f /Library/GPUBundles/NVMTLDriver.bundle/Contents/Info.plist ]; then
  cp /Library/GPUBundles/NVMTLDriver.bundle/Contents/Info.plist "$OUT/installed-plugin.plist"
  shasum -a 256 /Library/GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver > "$OUT/installed-plugin-sha256.txt" 2>&1
fi


# Preserve full crash reports before the bounded historical search.
mkdir -p "$OUT/WindowServer-crashes" || exit 2
crash_count=0
crash_failures=0
user_home=${HOME:-/var/root}
if [ -n "${SUDO_USER:-}" ] && [ "$SUDO_USER" != root ]; then
  resolved_home=$(/usr/bin/dscl . -read "/Users/$SUDO_USER" NFSHomeDirectory 2>/dev/null | sed 's/^NFSHomeDirectory: //')
  [ -z "$resolved_home" ] || user_home=$resolved_home
fi
for source in system user; do
  if [ "$source" = system ]; then crash_root=/Library/Logs/DiagnosticReports; else crash_root="$user_home/Library/Logs/DiagnosticReports"; fi
  [ -d "$crash_root" ] || continue
  mkdir -p "$OUT/WindowServer-crashes/$source" || exit 2
  while IFS= read -r -d '' crash; do
    if cp -p "$crash" "$OUT/WindowServer-crashes/$source/$(basename "$crash")" 2>> "$OUT/crash-copy-errors.txt"; then
      crash_count=$((crash_count+1))
      printf '%s\n' "$crash" >> "$OUT/crash-sources.txt"
    else
      crash_failures=$((crash_failures+1))
    fi
  done < <(find "$crash_root" -type f -name 'WindowServer*' -mtime -2 -print0 2>> "$OUT/crash-copy-errors.txt")
done
printf 'WindowServer crash reports from last 48 hours: copied=%s failed=%s\n' "$crash_count" "$crash_failures" | tee "$OUT/crash-status.txt"
if [ "$crash_count" = 0 ]; then
  echo 'No recent WindowServer crash report found; retained boot logs will still be collected.' | tee -a "$OUT/crash-status.txt"
fi

# Kernel panics are separate from WindowServer userspace crashes. Preserve
# originals before the historical log query, including nested Retired reports.
mkdir -p "$OUT/Kernel-panics"
panic_count=0
panic_failures=0
for panic_root in /Library/Logs/DiagnosticReports /Library/Logs/PanicReporter; do
  [ -d "$panic_root" ] || continue
  panic_source=$(basename "$panic_root")
  while IFS= read -r -d '' panic_file; do
    panic_relative=${panic_file#"$panic_root"/}
    panic_destination="$OUT/Kernel-panics/$panic_source/$panic_relative"
    mkdir -p "$(dirname "$panic_destination")"
    if cp -p "$panic_file" "$panic_destination" 2>> "$OUT/panic-copy-errors.txt"; then
      panic_count=$((panic_count+1))
      printf '%s\n' "$panic_file" >> "$OUT/panic-sources.txt"
    else
      panic_failures=$((panic_failures+1))
    fi
  done < <(find "$panic_root" -type f \( -iname '*panic*' -o -name 'Kernel*' \) -mtime -2 -print0 2>> "$OUT/panic-copy-errors.txt")
done
if [ -f /Library/Logs/panic.log ]; then
  if cp -p /Library/Logs/panic.log "$OUT/Kernel-panics/legacy-panic.log" 2>> "$OUT/panic-copy-errors.txt"; then
    panic_count=$((panic_count+1))
  else
    panic_failures=$((panic_failures+1))
  fi
fi
printf 'Kernel panic reports: copied=%s failed=%s\n' "$panic_count" "$panic_failures" | tee "$OUT/panic-status.txt"
if [ "$panic_count" = 0 ]; then
  echo 'No saved kernel panic report found; a panic-screen photo may be needed.' | tee -a "$OUT/panic-status.txt"
fi

/usr/bin/log show --start "$START" --end "$END" --style compact --info --debug --predicate 'process == "kernel" OR process == "WindowServer" OR process == "ReportCrash" OR process == "kernelmanagerd" OR process == "kernelmanager_helper" OR eventMessage CONTAINS[c] "NVMTL" OR eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "Latitude"' > "$OUT/failed-boot.txt" 2>&1 &
child=$!
( sleep 90; echo 'Search time limit reached; partial output retained.' >> "$OUT/status.txt"; kill -TERM "$child" 2>/dev/null; sleep 2; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 &
watcher=$!
wait "$child"; result=$?
kill "$watcher" 2>/dev/null
wait "$watcher" 2>/dev/null
printf 'log search exit=%s\n' "$result" >> "$OUT/status.txt"
wc -lc "$OUT/failed-boot.txt" | tee -a "$OUT/status.txt"
grep -Ei 'GUC: (Crash dump|Exception) notification|Non-recoverable wedged on init|Engine (default-state initialization|workaround verification) failed|reserved stolen prefix|cannot bound firmware scanout' "$OUT/failed-boot.txt" > "$OUT/gpu-initialization-summary.txt" || true
if [ -s "$OUT/gpu-initialization-summary.txt" ]; then
  echo 'GPU initialization messages retained in gpu-initialization-summary.txt; review before downstream WindowServer errors.'
fi
printf 'Saved: %s\n' "$OUT"
exit "$result"
