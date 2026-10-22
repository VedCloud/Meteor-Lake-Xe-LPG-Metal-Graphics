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
END=${2:-}
OUT="$HERE/Desktop/reports/failure-detail-$(date +%Y%m%d-%H%M%S)"
mkdir "$OUT" || exit 2
printf 'Reading retained logs from %s to %s. No GPU tests or system changes.\n' "$START" "${END:-end of live collection}"
printf 'This search has a 90-second limit. Partial output is retained.\n'
{ date; sysctl kern.boottime; sw_vers; } > "$OUT/collection-context.txt" 2>&1

sysctl machdep.cpu.features machdep.cpu.leaf7_features > "$OUT/cpu-features.txt" 2>&1

# Read-only active snapshot, exact icon bytes and decode checks. No cache clearing.
branding_helper="$HERE/Desktop/AboutThisMac/latitude-about-this-mac"
if [ -f "$branding_helper" ]; then
  echo 'Checking About This Mac icon and active snapshot (25-second limit).'
  "$branding_helper" diagnose "$HERE/Desktop/AboutThisMac" > "$OUT/about-this-mac.plist" 2> "$OUT/about-this-mac.stderr.txt" &
  branding_child=$!
  ( sleep 25; kill -TERM "$branding_child" 2>/dev/null; sleep 1; kill -KILL "$branding_child" 2>/dev/null ) >/dev/null 2>&1 & branding_watch=$!
  wait "$branding_child"; branding_result=$?
  kill "$branding_watch" 2>/dev/null; wait "$branding_watch" 2>/dev/null
  echo "branding diagnostics exit=$branding_result" > "$OUT/about-this-mac.status.txt"
fi

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
if [ -f /Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel ]; then
  shasum -a 256 /Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel > "$OUT/current-installed-accelerator-sha256.txt" 2>&1
  /usr/bin/plutil -p /Library/Extensions/NMXAccel.kext/Contents/Info.plist > "$OUT/current-installed-accelerator-info.txt" 2>&1 || true
fi
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
# Profile Weather independently when it is running; text corruption is not
# diagnosed by an idle WindowServer stack or the handled render-node denial.
weather_pid=$(/usr/bin/pgrep -x Weather | /usr/bin/head -n 1)
case "$weather_pid" in ''|*[!0-9]*) echo 'Weather is not running: its text fault cannot be sampled. Open Weather before the next collection.'; echo 'Weather not running; sample skipped.' > "$OUT/weather-sample.status.txt";;
 *) if [ "$weather_pid" -gt 1 ] && [ "$weather_pid" != "$current_ws_pid" ]; then
      echo 'Capturing Weather rendering activity (25-second limit).'
      /bin/ps -p "$weather_pid" -o pid,etime,%cpu,rss,time,comm > "$OUT/weather-process.txt" 2>&1
      /usr/bin/sample "$weather_pid" 3 10 -file "$OUT/weather-sample.txt" > "$OUT/weather-sample.stderr.txt" 2>&1 &
      weather_child=$!
      ( sleep 25; kill -TERM "$weather_child" 2>/dev/null; sleep 1; kill -KILL "$weather_child" 2>/dev/null ) >/dev/null 2>&1 & weather_watch=$!
      wait "$weather_child"; weather_result=$?
      kill "$weather_watch" 2>/dev/null; wait "$weather_watch" 2>/dev/null
      if [ "$weather_result" = 0 ] && [ ! -s "$OUT/weather-sample.txt" ]; then weather_result=2; fi
      echo "PID=$weather_pid exit=$weather_result" > "$OUT/weather-sample.status.txt"
    else echo 'Invalid or duplicate Weather PID; sample skipped.' > "$OUT/weather-sample.status.txt"; fi;;
esac
# Observe delivered desktop update timestamps; no pixels are saved and no
# screen-recording permission is requested. Missing permission is an explicit skip.
pacing_helper="$HERE/Desktop/latitude-frame-pacing"
if [ -f "$pacing_helper" ]; then
  pacing_user=${SUDO_USER:-$(/usr/bin/stat -f %Su /dev/console)}
  pacing_uid=$(/usr/bin/id -u "$pacing_user" 2>/dev/null || true)
  case "$pacing_uid" in ''|*[!0-9]*) echo 'No desktop user for frame observation' > "$OUT/frame-pacing.status.txt";;
   *) if [ "$pacing_uid" -ge 501 ]; then
      echo 'FRAME PACING: keep moving/resizing Weather for the next 8 seconds while timing is observed.'
      { date; sysctl debug.nmxfb_front_copies debug.nmxfb_front_copy_ns debug.nmxfb_front_copy_max_ns debug.nmxfb_front_timer_skipped debug.nmxfb_front_late_refresh debug.nmxfb_front_incomplete_held debug.nmxfb_gamma_copies debug.nmxfb_gamma_refused debug.nmxfb_gamma_ns; } > "$OUT/front-copy-before.txt" 2>&1
      /bin/ps -p "$current_ws_pid" -o pid,etime,%cpu,rss,time,comm > "$OUT/frame-cpu-before.txt" 2>&1
      /bin/launchctl asuser "$pacing_uid" /usr/bin/sudo -H -u "$pacing_user" "$pacing_helper" > "$OUT/frame-pacing.json" 2> "$OUT/frame-pacing.stderr.txt" &
      pacing_child=$!
      ( sleep 25; kill -TERM "$pacing_child" 2>/dev/null; sleep 1; kill -KILL "$pacing_child" 2>/dev/null ) >/dev/null 2>&1 & pacing_watch=$!
      sleep 8
      { date; sysctl debug.nmxfb_front_copies debug.nmxfb_front_copy_ns debug.nmxfb_front_copy_max_ns debug.nmxfb_front_timer_skipped debug.nmxfb_front_late_refresh debug.nmxfb_front_incomplete_held debug.nmxfb_gamma_copies debug.nmxfb_gamma_refused debug.nmxfb_gamma_ns; } > "$OUT/front-copy-after.txt" 2>&1
      /bin/ps -p "$current_ws_pid" -o pid,etime,%cpu,rss,time,comm > "$OUT/frame-cpu-after.txt" 2>&1
      wait "$pacing_child"; pacing_result=$?
      kill "$pacing_watch" 2>/dev/null; wait "$pacing_watch" 2>/dev/null
      echo "frame observer exit=$pacing_result; see JSON for capture/skip reason" > "$OUT/frame-pacing.status.txt"
    else echo 'No desktop user for frame observation' > "$OUT/frame-pacing.status.txt"; fi;;
  esac
fi
/usr/sbin/ioreg -r -c LatitudeBacklightHandler -l -w0 > "$OUT/brightness-registry.txt" 2>&1
/usr/sbin/ioreg -r -c AppleBacklightDisplay -l -w0 > "$OUT/brightness-display-registry.txt" 2>&1
/usr/sbin/ioreg -r -c NMXIntelVGAFB -l -w0 > "$OUT/gamma-registry.txt" 2>&1
# Colour-capability and actual display-pipe counters; no sysctl writes.
sysctl debug.nmxfb_agdc_gamma debug.nmxfb_agdc_gamma_answers debug.nvaccel_iop_ok debug.nvaccel_iop_empty debug.nvaccel_iop_fail debug.nmxfb_front_hz debug.nmxfb_blob_ordered > "$OUT/display-path-counters.txt" 2>&1
mkdir -p "$OUT/audio"
# USB file/configuration identity is separate from current-loaded-kexts.txt.
for oc_root in "$HERE/../EFI/OC" "$HERE/../../EFI/OC"; do
  [ -f "$oc_root/config.plist" ] || continue
  shasum -a 256 "$oc_root/config.plist" > "$OUT/usb-config-sha256.txt" 2>&1
  /usr/bin/plutil -extract DeviceProperties.Add xml1 -o - "$oc_root/config.plist" > "$OUT/audio/usb-device-properties.plist" 2> "$OUT/audio/usb-device-properties.stderr.txt" || true
  if [ -f "$oc_root/Kexts/AppleALC.kext/Contents/Info.plist" ]; then
    /usr/bin/plutil -p "$oc_root/Kexts/AppleALC.kext/Contents/Info.plist" > "$OUT/audio/usb-applealc-info.txt" 2>&1 || true
    shasum -a 256 "$oc_root/Kexts/AppleALC.kext/Contents/MacOS/AppleALC" > "$OUT/audio/usb-applealc-sha256.txt" 2>&1
  fi
  for route in DSDT SSDT-LAT-BRT; do
    [ ! -f "$oc_root/ACPI/$route.aml" ] || shasum -a 256 "$oc_root/ACPI/$route.aml" >> "$OUT/usb-brightness-route-sha256.txt" 2>&1
  done
  break
done
for audio_class in AppleHDAController IOHDACodecDevice AppleHDACodecGeneric IOAudioDevice; do
  /usr/sbin/ioreg -r -c "$audio_class" -l -w0 > "$OUT/audio/$audio_class.txt" 2>&1 & audio_child=$!
  ( sleep 5; kill -TERM "$audio_child" 2>/dev/null; sleep 1; kill -KILL "$audio_child" 2>/dev/null ) >/dev/null 2>&1 & audio_watch=$!
  wait "$audio_child"; audio_result=$?;kill "$audio_watch" 2>/dev/null;wait "$audio_watch" 2>/dev/null
  echo "$audio_class exit=$audio_result" >> "$OUT/audio/status.txt"
done
/usr/sbin/system_profiler SPAudioDataType -json > "$OUT/audio/devices.json" 2> "$OUT/audio/devices.stderr.txt" & audio_child=$!
( sleep 15; kill -TERM "$audio_child" 2>/dev/null; sleep 1; kill -KILL "$audio_child" 2>/dev/null ) >/dev/null 2>&1 & audio_watch=$!
wait "$audio_child";audio_result=$?;kill "$audio_watch" 2>/dev/null;wait "$audio_watch" 2>/dev/null
echo "CoreAudio profiler exit=$audio_result" >> "$OUT/audio/status.txt"


# Bounded input-chain snapshots for the Meteor Lake touchpad trial.
mkdir -p "$OUT/input"
/usr/sbin/ioreg -p IOACPIPlane -r -n ECDV -l -w0 > "$OUT/input/ec-acpi.txt" 2>&1 & ec_child=$!
( sleep 5; kill -TERM "$ec_child" 2>/dev/null; sleep 1; kill -KILL "$ec_child" 2>/dev/null ) >/dev/null 2>&1 & ec_watch=$!
wait "$ec_child";ec_result=$?;kill "$ec_watch" 2>/dev/null;wait "$ec_watch" 2>/dev/null
echo "EC ACPI registry exit=$ec_result" > "$OUT/input/ec-acpi.status.txt"
/usr/sbin/ioreg -p IOACPIPlane -r -n GFX0 -l -w0 > "$OUT/input/graphics-acpi.txt" 2>&1 & panel_child=$!
( sleep 5; kill -TERM "$panel_child" 2>/dev/null; sleep 1; kill -KILL "$panel_child" 2>/dev/null ) >/dev/null 2>&1 & panel_watch=$!
wait "$panel_child";panel_result=$?;kill "$panel_watch" 2>/dev/null;wait "$panel_watch" 2>/dev/null
echo "Graphics ACPI registry exit=$panel_result" > "$OUT/input/graphics-acpi.status.txt"

/usr/sbin/ioreg -r -c LatitudeHotkeys -l -w0 > "$OUT/input/dell-wmi-hotkeys.txt" 2>&1 & wmi_child=$!
( sleep 5; kill -TERM "$wmi_child" 2>/dev/null; sleep 1; kill -KILL "$wmi_child" 2>/dev/null ) >/dev/null 2>&1 & wmi_watch=$!
wait "$wmi_child";wmi_result=$?;kill "$wmi_watch" 2>/dev/null;wait "$wmi_watch" 2>/dev/null
echo "Dell WMI bridge registry exit=$wmi_result" > "$OUT/input/dell-wmi-hotkeys.status.txt"
/usr/sbin/ioreg -p IOACPIPlane -r -n AMW0 -l -w0 > "$OUT/input/dell-wmi-acpi.txt" 2>&1 & wmi_child=$!
( sleep 5; kill -TERM "$wmi_child" 2>/dev/null; sleep 1; kill -KILL "$wmi_child" 2>/dev/null ) >/dev/null 2>&1 & wmi_watch=$!
wait "$wmi_child";wmi_result=$?;kill "$wmi_watch" 2>/dev/null;wait "$wmi_watch" 2>/dev/null
echo "Dell WMI ACPI registry exit=$wmi_result" > "$OUT/input/dell-wmi-acpi.status.txt"
key_helper="$HERE/Desktop/latitude-key-state"
if [ -f "$key_helper" ]; then
  "$key_helper" > "$OUT/input/firmware-key-counters.json" 2> "$OUT/input/firmware-key-counters.stderr.txt" & key_child=$!
  ( sleep 10; kill -TERM "$key_child" 2>/dev/null; sleep 1; kill -KILL "$key_child" 2>/dev/null ) >/dev/null 2>&1 & key_watch=$!
  wait "$key_child";key_result=$?;kill "$key_watch" 2>/dev/null;wait "$key_watch" 2>/dev/null
  echo "Firmware key counter snapshot exit=$key_result" > "$OUT/input/firmware-key-counters.status.txt"
fi
for input_class in ApplePS2Controller VoodooI2CControllerDriver LatitudeI2CMeteorLakeController VoodooI2CHIDDevice VoodooI2CPrecisionTouchpadHIDEventDriver VoodooInput BrightnessKeys ApplePS2Keyboard; do
  /usr/sbin/ioreg -r -c "$input_class" -l -w0 > "$OUT/input/$input_class.txt" 2>&1 &
  input_child=$!
  ( sleep 5; kill -TERM "$input_child" 2>/dev/null; sleep 1; kill -KILL "$input_child" 2>/dev/null ) >/dev/null 2>&1 & input_watch=$!
  wait "$input_child"; input_result=$?
  kill "$input_watch" 2>/dev/null; wait "$input_watch" 2>/dev/null
  printf '%s exit=%s\n' "$input_class" "$input_result" >> "$OUT/input/status.txt"
done
# Bluetooth/I2C diagnostics: bounded, read-only; no pairing or firmware commands.
mkdir -p "$OUT/bluetooth"
/usr/sbin/system_profiler SPBluetoothDataType -json > "$OUT/bluetooth/system-profiler.json" 2>&1 & bt_child=$!
( sleep 15; kill -TERM "$bt_child" 2>/dev/null; sleep 1; kill -KILL "$bt_child" 2>/dev/null ) >/dev/null 2>&1 & bt_watch=$!
wait "$bt_child"; bt_result=$?; kill "$bt_watch" 2>/dev/null; wait "$bt_watch" 2>/dev/null
echo "Bluetooth profiler exit=$bt_result" > "$OUT/bluetooth/status.txt"
for bt_class in IOUSBHostDevice IntelBluetoothFirmware IntelBTPatcher IOBluetoothHCIController; do
  /usr/sbin/ioreg -r -c "$bt_class" -l -w0 > "$OUT/bluetooth/$bt_class.txt" 2>&1 & bt_child=$!
  ( sleep 5; kill -TERM "$bt_child" 2>/dev/null; sleep 1; kill -KILL "$bt_child" 2>/dev/null ) >/dev/null 2>&1 & bt_watch=$!
  wait "$bt_child"; bt_result=$?; kill "$bt_watch" 2>/dev/null; wait "$bt_watch" 2>/dev/null
  printf '%s exit=%s\n' "$bt_class" "$bt_result" >> "$OUT/bluetooth/status.txt"
done
for acpi_device in I2C3 TPD0; do
  /usr/sbin/ioreg -p IOACPIPlane -r -n "$acpi_device" -l -w0 > "$OUT/input/$acpi_device-acpi.txt" 2>&1 & bt_child=$!
  ( sleep 5; kill -TERM "$bt_child" 2>/dev/null; sleep 1; kill -KILL "$bt_child" 2>/dev/null ) >/dev/null 2>&1 & bt_watch=$!
  wait "$bt_child"; bt_result=$?; kill "$bt_watch" 2>/dev/null; wait "$bt_watch" 2>/dev/null
  printf '%s ACPI exit=%s\n' "$acpi_device" "$bt_result" >> "$OUT/input/status.txt"
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
# Bootloader injection status is separate from an enabled config entry.
mkdir -p "$OUT/opencore"
for oc_root in "$HERE/.." "$HERE/../.."; do
  [ -d "$oc_root/EFI/OC" ] || continue
  shasum -a 256 "$oc_root/EFI/OC/config.plist" > "$OUT/opencore/config-sha256.txt" 2>&1
  for oc_name in LatitudeHotkeys VoodooI2CHID LatitudeI2CMeteorLake; do
    oc_info="$oc_root/EFI/OC/Kexts/$oc_name.kext/Contents/Info.plist"
    [ ! -f "$oc_info" ] || /usr/bin/plutil -p "$oc_info" > "$OUT/opencore/$oc_name-info.txt" 2>&1
  done
  # OpenCore uses generated names with no whitespace. Retain only latest three.
  for oc_log in $(/bin/ls "$oc_root"/opencore-*.txt 2>/dev/null | /usr/bin/sort | /usr/bin/tail -3); do
    /bin/cp "$oc_log" "$OUT/opencore/" & oc_child=$!
    ( sleep 5; kill -TERM "$oc_child" 2>/dev/null; sleep 1; kill -KILL "$oc_child" 2>/dev/null ) >/dev/null 2>&1 & oc_watch=$!
    wait "$oc_child"; oc_result=$?; kill "$oc_watch" 2>/dev/null; wait "$oc_watch" 2>/dev/null
    printf '%s exit=%s\n' "${oc_log##*/}" "$oc_result" >> "$OUT/opencore/status.txt"
  done
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

# Resolve the default end after live samples, and retain the exact requested range.
[ -n "$END" ] || END=$(date '+%Y-%m-%d %H:%M:%S')
printf 'start=%s\nend=%s\n' "$START" "$END" > "$OUT/log-window.txt"
/usr/bin/log show --start "$START" --end "$END" --style compact --info --debug --predicate 'process == "kernel" OR process == "WindowServer" OR process == "bluetoothd" OR process == "BlueTool" OR process == "Weather" OR process == "coreaudiod" OR process == "corebrightnessd" OR process == "displaypolicyd" OR process == "ReportCrash" OR process == "kernelmanagerd" OR process == "kernelmanager_helper" OR eventMessage CONTAINS[c] "NVMTL" OR eventMessage CONTAINS[c] "NMX" OR eventMessage CONTAINS[c] "Latitude" OR eventMessage CONTAINS[c] "AppleALC" OR eventMessage CONTAINS[c] "AppleHDA"' > "$OUT/failed-boot.txt" 2>&1 &
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
grep -Ei 'publication0\.3\.15|glyph-layout0\.3\.15|gamma|AppleALC|AppleHDA|coreaudiod|CoreBrightness|timed out.*fence|fence.*timed out' "$OUT/failed-boot.txt" > "$OUT/new-feature-log-summary.txt" || true
grep -i 'Wall Clock adjustment' "$OUT/failed-boot.txt" > "$OUT/log-clock-warnings.txt" || true
printf 'Saved: %s\n' "$OUT"
exit "$result"
