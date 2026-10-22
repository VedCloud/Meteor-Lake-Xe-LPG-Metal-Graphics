#!/bin/bash
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(uname -m)" = x86_64 ] || { echo 'Run on the Intel Dell.'; exit 2; }
COUNT=${1:-100}
PROFILE=${2:-full}
case "$PROFILE" in full|fresh-pools|fresh-staging) ;; *) echo 'Profile must be full, fresh-pools or fresh-staging'; exit 2 ;; esac
case "$COUNT" in ''|*[!0-9]*) echo 'Iteration count must be 1..10000'; exit 2 ;; esac
[ "$COUNT" -ge 1 ] && [ "$COUNT" -le 10000 ] || exit 2
OUT="$HERE/reports/readback-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT"
START_TIME=$(date '+%Y-%m-%d %H:%M:%S')
printf 'start=%s profile=%s\n' "$START_TIME" "$PROFILE" > "$OUT/capture-window.txt"
LIVE_CHILD=''
stop_live() {
  if [ -n "$LIVE_CHILD" ]; then kill -TERM "$LIVE_CHILD" 2>/dev/null || true; wait "$LIVE_CHILD" 2>/dev/null || true; LIVE_CHILD=''; fi
}
trap stop_live EXIT

# Refuse this suite if the custom framebuffer owns the physical display.
ioreg -r -c LatitudePanelClaim -l -w0 > "$OUT/panel-before.txt"
if grep -q 'LatitudePanelClaim' "$OUT/panel-before.txt"; then
  echo "Custom panel is present. Reinstall in readback-only mode and reboot first. Report: $OUT"; exit 2
fi
sysctl kern.boottime > "$OUT/boot.txt"
sysctl kern.bootargs > "$OUT/boot-arguments.txt" 2>&1
kmutil showloaded > "$OUT/loaded-kexts.txt" 2>&1
expected_gpu_uuid=$(cat "$HERE/NMXGPU-UUID.txt") || exit 2
[ -n "$expected_gpu_uuid" ] || exit 2
if ! grep -F 'com.nullmoth.NMXGPU (1.0.8)' "$OUT/loaded-kexts.txt" | grep -Fq "$expected_gpu_uuid"; then
  echo 'Expected newly loaded NMXGPU 1.0.8 UUID; install this candidate and reboot through USB first.'; exit 2
fi
cmp -s "$HERE/Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU" /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU || { echo 'Installed GPU kernel binary differs from candidate'; exit 2; }
# Match kernel event records, excluding log-filter headers and log command metadata.
kernel_faults() {
  grep -hE '^[0-9]{4}-.* kernel\[' "$@" | grep -Ei 'GPU HANG|Resetting (chip|rcs|engine)|GT[0-9 ]* reset|TLB invalidation response timed out|TLB invalidation did not complete'
}
USERCMD=(/usr/bin/env)
if [ "$(id -u)" = 0 ] && [ -n "${SUDO_USER:-}" ] && [ "$SUDO_USER" != root ]; then USERCMD=(sudo -u "$SUDO_USER"); fi
cp /Library/GPUBundles/NVMTLDriver.bundle/Contents/Info.plist "$OUT/plugin.plist" || exit 2
version=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleVersion' "$OUT/plugin.plist")
[ "$version" = 0.3.0 ] || { echo "Expected plugin 0.3.0, found $version"; exit 2; }
shasum -a 256 /Library/GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver /Library/GPUBundles/nvmtl/libdrm.2.dylib > "$OUT/binaries.txt"
for relative in NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver nvmtl/libdrm.2.dylib; do
  cmp -s "$HERE/Library/GPUBundles/$relative" "/Library/GPUBundles/$relative" || { echo "Installed binary differs from this candidate: $relative"; exit 2; }
done
echo 'Waiting up to 75 seconds for the Metal plugin registration...'
ready=0
for ((attempt=0; attempt<=37; attempt++)); do
  ioreg -r -c NMXAccel -l -w0 > "$OUT/accelerator-before.txt"
  if grep -Eq '"MetalPluginName" = "([^"[:space:]]*/)?NVMTLDriver"' "$OUT/accelerator-before.txt"; then ready=1; break; fi
  [ "$attempt" = 37 ] || sleep 2
done
[ "$ready" = 1 ] || { echo 'NMXAccel Metal plugin did not register. No GPU tests run.'; exit 2; }
# Run only one suite per GPU boot, especially after a failure.
BOOT_UUID=$(sysctl -n kern.bootsessionuuid) || exit 2
[ -n "$BOOT_UUID" ] || exit 2
printf '%s\n' "$BOOT_UUID" > "$OUT/boot-session.txt"
if true; then
  LAST_BOOT="$HERE/reports/readback-last-boot.txt"
  if [ -f "$LAST_BOOT" ] && [ "$(cat "$LAST_BOOT")" = "$BOOT_UUID" ]; then
    echo 'A suite already ran this boot. Prepare and reboot USB before another test.'; exit 2
  fi
  if nvram nullmoth-gpu-once >/dev/null 2>&1; then
    echo 'GPU opt-in is pending. Reboot through USB before running the comparison.'; exit 2
  fi
  printf '%s\n' "$BOOT_UUID" > "$LAST_BOOT"
fi
log stream --style compact --level debug --predicate 'process == "kernel" AND (eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "drmn" OR eventMessage CONTAINS[c] "GPU HANG" OR eventMessage CONTAINS[c] "GuC") OR subsystem == "org.nullmoth.libdrm"' > "$OUT/kernel-live.txt" 2>&1 &
LIVE_CHILD=$!
echo "Readback suite: $COUNT iterations per worker/case, profile=$PROFILE. Progress every 50 frames. Logs: $OUT"
echo 'Each case stops after at most 600 seconds. The suite stops on the first failure.'
run_case() {
  local name=$1 route=$2 pools=$3 stages=$4; shift 4
  local log="$OUT/$name.txt"
  echo "START $name"
  local envargs=(NVMTL_ENABLE=1 NVMTL_DIAGNOSTIC_STDERR=1 NMX_DRM_TEST_TRANSPORT="$route" NVMTL_POOL_REUSE=1 NVMTL_READBACK_TRACE=1)
  [ "$stages" = retained ] || envargs+=(NVMTL_READBACK_STAGE_FRESH=1)
  [ "$pools" != 0 ] || envargs+=(NVMTL_COMMAND_POOL_FRESH=1)
  local expected_policy=enforced
  [ "$stages" = retained ] || expected_policy=enforced-fresh-staging
  printf 'route=%s legacy-case-selector=%s command-pools=fresh-enforced readback-staging=%s witness=enabled address-page-cache=enabled\n' "$route" "$pools" "$stages" > "$OUT/$name-policy.txt"
  "${USERCMD[@]}" /usr/bin/env -u NVMTL_COMMAND_POOL_FRESH -u NVMTL_READBACK_STAGE_FRESH -u NVMTL_NO_POOL_REUSE -u INTEL_DEBUG -u VK_INSTANCE_LAYERS -u NVMTL_ASYNC_TEX_UPLOAD "${envargs[@]}" "$HERE/tools/latitude-readback-test" --iterations "$COUNT" --expect-policy "$expected_policy" "$@" > "$log" 2>&1 &
  local child=$!
  ( sleep 600; echo 'FAIL watchdog: case exceeded 600 seconds' >> "$log"; kill -TERM "$child" 2>/dev/null; sleep 2; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 &
  local watcher=$!
  wait "$child"; local result=$?
  kill "$watcher" 2>/dev/null; wait "$watcher" 2>/dev/null
  tail -n 5 "$log"
  if [ "$result" = 0 ]; then
    grep -q '^PASS command-pool cleanup:' "$log" || result=1
    grep -q '^PASS exact readback:' "$log" || result=1
    grep -q '^PASS effective backend policy:' "$log" || result=1
    if [ "$route" = direct ]; then
      grep -Eq 'NMX transport counters: requested=direct direct=[1-9][0-9]* iokit=0$' "$log" || result=1
    else
      grep -Eq 'NMX transport counters: requested=iokit direct=0 iokit=[1-9][0-9]*$' "$log" || result=1
    fi
  fi
  case "$name" in *-iosurface|*-bgra-surface)
    grep -q '^PASS IOSurface backing:' "$log" || result=1 ;;
  esac
  if kernel_faults "$OUT/kernel-live.txt" > "$OUT/$name-kernel-faults.txt"; then
    echo 'FAIL kernel GPU/TLB fault detected; stopping before another workload'; result=1
  fi
  echo "$name exit=$result" | tee -a "$OUT/results.txt"
  return "$result"
}
result=0
ROUTES=(iokit direct)
[ "$PROFILE" = full ] || ROUTES=(iokit)
for route in "${ROUTES[@]}"; do
  echo "Checking $route device discovery before Metal enumeration..."
  "${USERCMD[@]}" env NMX_DRM_TEST_TRANSPORT="$route" "$HERE/tools/latitude-drm-route-test" > "$OUT/$route-discovery.txt" 2>&1 &
  probe_child=$!
  ( sleep 60; echo 'FAIL discovery watchdog' >> "$OUT/$route-discovery.txt"; kill -TERM "$probe_child" 2>/dev/null; sleep 2; kill -KILL "$probe_child" 2>/dev/null ) >/dev/null 2>&1 &
  probe_watcher=$!
  wait "$probe_child"; probe=$?
  kill "$probe_watcher" 2>/dev/null; wait "$probe_watcher" 2>/dev/null
  cat "$OUT/$route-discovery.txt"
  if [ "$probe" != 0 ]; then echo "$route-discovery exit=$probe" >> "$OUT/results.txt"; result=1; break; fi
  run_case "$route-small-raw" "$route" 1 retained --size 64x64 --raw || { result=1; break; }
  run_case "$route-small-render" "$route" 1 retained --size 64x64 || { result=1; break; }
  run_case "$route-full-render" "$route" 1 retained --size 1920x1080 || { result=1; break; }
  if [ "$PROFILE" != full ]; then
    pool_mode=1;stage_mode=retained
    [ "$PROFILE" != fresh-pools ] || pool_mode=0
    [ "$PROFILE" != fresh-staging ] || stage_mode=fresh
    run_case "$route-padded-$PROFILE" "$route" "$pool_mode" "$stage_mode" --size 1920x1080 --padding 64 || result=1
    break
  fi
  run_case "$route-padded" "$route" 1 retained --size 1920x1080 --padding 64 || { result=1; break; }
  run_case "$route-fresh-pools" "$route" 0 retained --size 1920x1080 --padding 64 || { result=1; break; }
  run_case "$route-fresh-staging" "$route" 1 fresh --size 1920x1080 --padding 64 || { result=1; break; }
  run_case "$route-fresh-textures" "$route" 1 retained --size 1920x1080 --padding 64 --fresh-textures || { result=1; break; }
  run_case "$route-two-independent" "$route" 1 retained --size 1920x1080 --padding 64 --threads 2 || { result=1; break; }
  run_case "$route-two-handoff" "$route" 1 retained --size 1920x1080 --padding 64 --threads 2 --shared-texture || { result=1; break; }
  run_case "$route-iosurface" "$route" 1 retained --size 1920x1080 --padding 64 --iosurface || { result=1; break; }
  run_case "$route-bgra-surface" "$route" 1 retained --size 1920x1080 --padding 64 --iosurface --bgra || { result=1; break; }
done
# Capture only this run, starting at its recorded wall-clock timestamp.
# Primary failure evidence is already in each test's stderr and live stream.
stop_live
END_TIME=$(date '+%Y-%m-%d %H:%M:%S')
printf 'end=%s\n' "$END_TIME" >> "$OUT/capture-window.txt"
log show --start "$START_TIME" --end "$END_TIME" --style compact --predicate 'eventMessage CONTAINS[c] "i915" OR eventMessage CONTAINS[c] "GPU HANG" OR eventMessage CONTAINS[c] "first-submit" OR subsystem == "org.nullmoth.libdrm" OR process == "latitude-readback-test" OR process == "latitude-drm-route-test"' > "$OUT/gpu-history.txt" 2>&1 &
child=$!
( sleep 30; kill -TERM "$child" 2>/dev/null; sleep 2; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 &
watcher=$!
wait "$child"; capture_result=$?; kill "$watcher" 2>/dev/null; wait "$watcher" 2>/dev/null
if kernel_faults "$OUT/kernel-live.txt" "$OUT/gpu-history.txt" > "$OUT/hang-check.txt"; then
  echo 'FAIL kernel GPU hang/reset/TLB fault recorded during suite'; result=1
fi
if [ "$capture_result" != 0 ]; then echo 'FAIL bounded historical capture incomplete'; result=1; fi
printf 'READBACK SUITE exit=%s iterations=%s\n' "$result" "$COUNT" | tee -a "$OUT/results.txt"
if [ "$result" = 0 ]; then
  shasum -a 256 /Library/GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver /Library/GPUBundles/nvmtl/libdrm.2.dylib /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU > "$OUT/passing-binaries.txt"
  if [ "$PROFILE" = full ] && [ "$COUNT" -ge 1000 ] && [ "$(grep -c ' exit=0$' "$OUT/results.txt")" = 22 ]; then
    { printf 'candidate=0.3.1 cases=22 iterations=%s boot=%s\n' "$COUNT" "$BOOT_UUID"; cat "$OUT/passing-binaries.txt"; } > "$OUT/desktop-gate.txt"
    if [ -n "${READBACK_GATE_FILE:-}" ]; then cp "$OUT/desktop-gate.txt" "$READBACK_GATE_FILE" || exit 2; fi
    echo 'PASS full 22-case hardware gate, including effective policy and pool cleanup.'
  fi
fi
echo "Saved report: $OUT"
[ "$COUNT" -ge 1000 ] || echo 'This was a smoke test; the 1000-iteration gate remains pending.'
exit "$result"
