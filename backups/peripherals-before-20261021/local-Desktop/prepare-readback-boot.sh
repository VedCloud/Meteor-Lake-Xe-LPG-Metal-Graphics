#!/bin/bash
set -euo pipefail
[ "$(id -u)" = 0 ] && [ "$(uname -m)" = x86_64 ] || { echo 'Run with sudo on the Intel Dell.' >&2; exit 2; }
nvram -d nullmoth-panel-once 2>/dev/null || true
if nvram nullmoth-panel-once >/dev/null 2>&1; then echo 'Panel opt-in could not be removed.' >&2; exit 1; fi
nvram nullmoth-gpu-once=1
echo 'Prepared ONE GPU-only boot. Reboot through USB before the next comparison.'
echo 'This script has not rebooted the laptop or enabled the custom panel.'
