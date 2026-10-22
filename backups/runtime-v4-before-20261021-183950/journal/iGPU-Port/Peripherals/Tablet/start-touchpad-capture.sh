#!/bin/bash
set -euo pipefail
[ "$(uname -s)" = Linux ] || { echo 'Run in the Dell Linux live session.'; exit 2; }
[ "$(id -u)" = 0 ] || { echo 'Run: sudo /bin/bash /run/initramfs/live/start-touchpad-capture.sh'; exit 2; }
mapfile -t devices < <(blkid -t LABEL=LATSENSORS -o device)
[ "${#devices[@]}" = 1 ] || { echo 'Expected exactly one LATSENSORS data partition; stopped.'; exit 2; }
# Only mount the deliberately prepared USB data partition, never an internal OS disk.
parent=$(lsblk -nro PKNAME "${devices[0]}" | head -1)
[ -n "$parent" ] && [ "$(lsblk -dnro TRAN "/dev/$parent")" = usb ] || { echo 'Data partition is not on a USB disk; stopped.'; exit 2; }
DEST=$(findmnt -nro TARGET -S "${devices[0]}" | head -1 || true)
if [ -z "$DEST" ]; then
 DEST=/mnt/latitude-sensors
 mkdir -p "$DEST"
 mount -o nosuid,nodev,noexec "${devices[0]}" "$DEST"
fi
[ -f "$DEST/collect-touchpad-linux.py" ] || { echo 'Collector files missing.'; exit 2; }
exec python3 "$DEST/collect-touchpad-linux.py"
