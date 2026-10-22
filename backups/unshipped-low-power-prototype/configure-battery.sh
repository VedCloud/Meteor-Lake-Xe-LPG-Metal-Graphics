#!/bin/bash
set -euo pipefail
[ "$(uname -m)" = x86_64 ] || { echo 'Run this on the Dell.' >&2; exit 2; }
[ "$(id -u)" = 0 ] || { echo 'Run with sudo.' >&2; exit 2; }
STATE='/Library/Application Support/LatitudeInput'
current() { /usr/bin/pmset -g custom | /usr/bin/awk '/Battery Power:/{b=1;next}/AC Power:/{b=0} b&&$1=="lowpowermode"{print $2;exit}'; }
old=$(current)
case "$old" in 0|1) ;; *) echo 'Battery Low Power Mode is not exposed on this system; no changes.' >&2; exit 2;; esac
case "${1:-apply}" in
 apply)
  /bin/mkdir -p "$STATE"
  if [ ! -e "$STATE/battery-lowpower-before" ]; then
   /usr/bin/pmset -g custom > "$STATE/pmset-before.txt"
   printf '%s\n' "$old" > "$STATE/battery-lowpower-before"
  fi
  if ! /usr/bin/pmset -b lowpowermode 1 || [ "$(current)" != 1 ]; then
   /usr/bin/pmset -b lowpowermode "$old" || true
   echo 'Low Power Mode failed; previous value restored.' >&2; exit 1
  fi
  echo 'Battery-only Low Power Mode enabled. AC, sleep and hibernation settings retained.';;
 restore)
  saved=$(cat "$STATE/battery-lowpower-before")
  case "$saved" in 0|1) ;; *) echo 'Invalid saved value; refusing.' >&2; exit 2;; esac
  [ "$old" = 1 ] || { echo 'Setting changed since apply; refusing overwrite.' >&2; exit 2; }
  /usr/bin/pmset -b lowpowermode "$saved"
  [ "$(current)" = "$saved" ] || exit 1
  echo 'Previous battery Low Power Mode value restored.';;
 *) echo 'Usage: configure-battery.sh apply|restore'; exit 2;;
esac
