#!/bin/bash
set -uo pipefail
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run on the Intel Dell.' >&2; exit 2; }
HERE="$(cd "$(dirname "$0")" && pwd)"
OUT="$HERE/reports/continuity-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$OUT" || exit 2
run_bounded() {
 local name=$1 seconds=$2; shift 2
 "$@" > "$OUT/$name.txt" 2> "$OUT/$name.stderr.txt" & local child=$!
 ( sleep "$seconds"; kill -TERM "$child" 2>/dev/null; sleep 1; kill -KILL "$child" 2>/dev/null ) >/dev/null 2>&1 & local watch=$!
 wait "$child"; local result=$?; kill "$watch" 2>/dev/null; wait "$watch" 2>/dev/null
 printf '%s exit=%s (an empty successful query is not proof of support)\n' "$name" "$result" >> "$OUT/status.txt"
}
run_bounded context 5 /bin/bash -c 'date; sw_vers; sysctl kern.boottime'
# Disk candidate identity is separate from kmutil's currently loaded UUID.
for revision in 0.1.0 0.1.1 0.1.2; do
 for candidate_info in BUILD.json IDENTITY.json README.md; do
  if [ -f "$HERE/Candidate-$revision/$candidate_info" ]; then
   /usr/bin/head -c 65536 "$HERE/Candidate-$revision/$candidate_info" > "$OUT/candidate-$revision-$candidate_info"
  fi
 done
done
run_bounded usb-wifi-binary 8 /usr/bin/shasum -a 256 "$HERE/../../../EFI/OC/Kexts/AirportItlwm.kext/Contents/MacOS/AirportItlwm"
run_bounded usb-wifi-info 5 /usr/bin/plutil -p "$HERE/../../../EFI/OC/Kexts/AirportItlwm.kext/Contents/Info.plist"
# Status only; never invoke send/listen from the collector. The query records a
# loaded-driver mismatch as an error instead of silently claiming the new API.
radio_helper="$HERE/Candidate-0.1.2/latitude-awdl-radio-x86_64"
if [ -f "$radio_helper" ]; then
 run_bounded native-radio-status 8 "$radio_helper" status
fi

run_bounded radio-profile 30 /usr/sbin/system_profiler SPBluetoothDataType SPAirPortDataType -json
run_bounded loaded-kexts 10 /usr/bin/kmutil showloaded
run_bounded network-hardware 8 /usr/sbin/networksetup -listallhardwareports
run_bounded awdl0 5 /sbin/ifconfig awdl0
run_bounded llw0 5 /sbin/ifconfig llw0
for cls in AirportItlwm AirportItlwmSkywalkInterface IO80211Interface IO80211AWDLPeerManager IO80211P2PInterface IOBluetoothHCIController IntelBluetoothFirmware IntelBTPatcher BlueToolFixup IOUSBHostDevice; do
 run_bounded "registry-$cls" 5 /usr/sbin/ioreg -r -c "$cls" -l -w0
done
run_bounded continuity-events 15 /usr/bin/log show --last 10m --style compact --predicate '(process == "sharingd" OR process == "bluetoothd" OR process == "airportd" OR process == "wifid") AND (eventMessage CONTAINS[c] "awdl" OR eventMessage CONTAINS[c] "AirDrop" OR eventMessage CONTAINS[c] "Handoff" OR eventMessage CONTAINS[c] "Continuity" OR eventMessage CONTAINS[c] "firmware")'
printf '%s\n' 'Read-only capture. No pairing, network changes, private capability flags, packet injection, file transfers or service restarts.' 'An awdl0 name or advertised capability alone does not establish peer synchronization or an AirDrop transfer.' > "$OUT/SCOPE.txt"
echo "Saved: $OUT"
