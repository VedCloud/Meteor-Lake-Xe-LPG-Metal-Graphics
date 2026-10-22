#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(uname -m)" = x86_64 ] || { echo 'Run on the Dell.' >&2; exit 2; }
OUT="$HERE/reports/$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT" || exit 2
for saved in LatitudeDesktopLive.log LatitudeDesktopLive.log.previous LatitudeDesktopRegistry.log LatitudeDesktopRegistry.log.previous; do
  [ ! -f "/Library/Logs/$saved" ] || cp "/Library/Logs/$saved" "$OUT/$saved"
done
echo 'Collecting retained WindowServer failures (up to 90 seconds).'
/usr/bin/log show --last 15m --style compact --info --debug --predicate 'process == "WindowServer" OR (process == "kernel" AND (eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "Latitude" OR eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "drmn"))' > "$OUT/recent-desktop-history.txt" 2>&1 &
history_child=$!
( sleep 90; echo 'History search timed out; partial output retained.' >> "$OUT/history-status.txt"; kill -TERM "$history_child" 2>/dev/null; sleep 2; kill -KILL "$history_child" 2>/dev/null ) &
history_watcher=$!
wait "$history_child"; history_result=$?
kill "$history_watcher" 2>/dev/null
wait "$history_watcher" 2>/dev/null
printf 'history exit=%s\n' "$history_result" > "$OUT/history-status.txt"
mkdir -p "$OUT/WindowServer-crashes"
crash_count=0
while IFS= read -r -d '' crash; do
  cp "$crash" "$OUT/WindowServer-crashes/$(basename "$crash")"
  crash_count=$((crash_count + 1))
  [ "$crash_count" -lt 20 ] || break
done < <(find /Library/Logs/DiagnosticReports -type f -name 'WindowServer*' -mtime -1 -print0 2>/dev/null)
printf 'Copied %s recent WindowServer crash reports.\n' "$crash_count" > "$OUT/crash-status.txt"
sw_vers > "$OUT/macos.txt" 2>&1
sysctl machdep.cpu.brand_string >> "$OUT/macos.txt" 2>&1
csrutil status > "$OUT/sip.txt" 2>&1
kmutil showloaded > "$OUT/loaded-kexts.txt" 2>&1
for class in LatitudePanelClaim NMXGPU NMXAccel NMXGPUFB LatitudeMTLProbe NMXVGAFB NMXIntelVGAFB NMXDeviceControl NMXAGDCLoader IOFramebuffer; do
  ioreg -r -c "$class" -l -w0 > "$OUT/$class-ioreg.txt" 2>&1
done
ioreg -r -n GFX0 -l -w0 > "$OUT/GFX0-ioreg.txt" 2>&1
stat -f '%Sp %Su:%Sg %N' /Library/GPUBundles /Library/GPUBundles/nvmtl /Library/GPUBundles/nvmtl/libvulkan_intel.dylib /Library/GPUBundles/nvmtl/libdrm.2.dylib /Library/GPUBundles/NVMTLDriver.bundle > "$OUT/runtime-permissions.txt" 2>&1
for marker in nullmoth-gpu-once nullmoth-gpu-attaching nullmoth-panel-once; do nvram "$marker"; done > "$OUT/gpu-boot-markers.txt" 2>&1
system_profiler SPDisplaysDataType > "$OUT/displays.txt" 2>&1
dmesg > "$OUT/dmesg.txt" 2>&1
log show --last boot --style compact --predicate 'eventMessage CONTAINS[c] "NullMoth" OR eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "drmn" OR eventMessage CONTAINS[c] "GuC"' > "$OUT/driver-log.txt" 2>&1
run_bounded() {
  local name=$1; shift
  "$@" > "$OUT/$name.txt" 2>&1 &
  local child=$!
  ( sleep 45; echo 'TIMEOUT after 45 seconds' >> "$OUT/$name.txt"; kill -TERM "$child" 2>/dev/null; sleep 2; kill -KILL "$child" 2>/dev/null ) &
  local watcher=$!
  wait "$child"; local result=$?
  kill "$watcher" 2>/dev/null; wait "$watcher" 2>/dev/null
  printf '%s exit=%s\n' "$name" "$result" | tee -a "$OUT/results.txt"
}
# Run GPU clients as the logged-in user if the collector was started with sudo.
USERCMD=()
if [ "$(id -u)" = 0 ] && [ -n "${SUDO_USER:-}" ] && [ "$SUDO_USER" != root ]; then USERCMD=(sudo -u "$SUDO_USER"); fi
run_bounded i915-info "${USERCMD[@]}" "$HERE/tools/latitude-i915-info"
run_bounded vulkan "${USERCMD[@]}" "$HERE/tools/latitude-vulkan-test" "$HERE/tools/check.spv"
if grep -q '^PASS Vulkan hardware execution:' "$OUT/vulkan.txt"; then
  echo 'Waiting up to 75 seconds for NMXAccel MetalPluginName registration.' | tee "$OUT/metal-readiness.txt"
  metal_ready=0
  for ((attempt=0; attempt<=37; attempt++)); do
    ioreg -r -c NMXAccel -l -w0 > "$OUT/NMXAccel-before-metal-ioreg.txt" 2>&1
    if grep -Eq '"MetalPluginName" = "([^"[:space:]]*/)?NVMTLDriver"' "$OUT/NMXAccel-before-metal-ioreg.txt"; then
      metal_ready=1
      printf 'Metal plugin registered after approximately %s seconds.\n' "$((attempt * 2))" | tee -a "$OUT/metal-readiness.txt"
      break
    fi
    [ "$attempt" = 37 ] || sleep 2
  done
  if [ "$metal_ready" = 0 ]; then echo 'Metal plugin registration did not appear before the deadline.' | tee -a "$OUT/metal-readiness.txt"; fi
  run_bounded metal "${USERCMD[@]}" env NVMTL_ENABLE=1 "$HERE/tools/latitude-metal-test"
else
  echo 'Metal test skipped until Vulkan GPU output is correct.' | tee "$OUT/metal.txt"
fi
desktop_counters() {
  for key in debug.nvaccelfb debug.nmxaccel_headless debug.nvaccel_heads_published debug.nvaccel_heads_registered debug.nvaccel_heads_piped debug.nvaccel_iop_ok debug.nvaccel_iop_ok_h0 debug.nvaccel_iop_fail debug.nmxfb_front_copies debug.nmxfb_blob_flips debug.nmxfb_blob_refused debug.nmxfb_agdc debug.nmxfb_agdc_cmds debug.nmxfb_agdc_k5_link debug.nmxfb_agdc_k5_caps debug.nmxfb_agdc_fbmap; do sysctl "$key"; done
}
desktop_counters > "$OUT/desktop-counters-before.txt" 2>&1
run_bounded desktop "${USERCMD[@]}" "$HERE/tools/latitude-desktop-test"
desktop_counters > "$OUT/desktop-counters-after.txt" 2>&1
ioreg -r -c NMXAccel -l -w0 > "$OUT/NMXAccel-after-desktop-ioreg.txt" 2>&1
ioreg -r -c NMXVGAFB -l -w0 > "$OUT/panel-after-desktop-ioreg.txt" 2>&1
ps -axo pid,uid,pcpu,comm | awk 'NR==1 || /WindowServer/' > "$OUT/windowserver-process.txt"
ws_pid=$(pgrep -x WindowServer | head -1)
if [ -n "$ws_pid" ]; then
  vmmap "$ws_pid" > "$OUT/windowserver-vmmap.txt" 2>&1
  lsof -p "$ws_pid" > "$OUT/windowserver-open-files.txt" 2>&1
fi
log show --last boot --style compact --predicate 'process == "WindowServer" OR eventMessage CONTAINS[c] "NVMTL" OR eventMessage CONTAINS[c] "NMX-agdc"' > "$OUT/windowserver-log.txt" 2>&1
dmesg > "$OUT/dmesg-after-tests.txt" 2>&1
log show --last 5m --style compact --predicate 'process == "kernel" AND (eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "drmn" OR eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "Latitude" OR eventMessage CONTAINS[c] "agdc")' > "$OUT/driver-after-tests.txt" 2>&1
if [ -f /Library/NullMoth/latitude5350-last-backup.txt ]; then
  backup=$(cat /Library/NullMoth/latitude5350-last-backup.txt)
  case "$backup" in /Library/NullMoth/latitude5350-backups/*)
    for file in kmutil-preflight.log kmutil-installed.log kcditto.log; do
      [ ! -f "$backup/$file" ] || cp "$backup/$file" "$OUT/$file"
    done ;;
  esac
fi
USBROOT="$(cd "$HERE/../.." && pwd)"
latest_oc=$(ls -t "$USBROOT"/opencore-*.txt 2>/dev/null | head -1)
if [ -n "$latest_oc" ]; then cp "$latest_oc" "$OUT/opencore-boot.txt"; fi
if [ -f /Library/Logs/LatitudePanelLoad.log ]; then cp /Library/Logs/LatitudePanelLoad.log "$OUT/panel-boot-loader.txt"; fi
cat "$OUT/vulkan.txt" "$OUT/metal.txt"
printf 'Reports saved on USB: %s\n' "$OUT"
