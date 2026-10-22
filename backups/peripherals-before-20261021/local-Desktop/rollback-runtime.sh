#!/bin/bash
set -euo pipefail
[ "$(id -u)" = 0 ] || { echo 'Run rollback with sudo on the Dell.' >&2; exit 2; }
BACKUP="${1:-$(cat /Library/NullMoth/latitude5350-last-backup.txt)}"
case "$BACKUP" in /Library/NullMoth/latitude5350-backups/*) ;; *) echo 'Unexpected backup path' >&2; exit 2 ;; esac
[ -d "$BACKUP/original" ] || { echo 'Backup not found' >&2; exit 2; }
nvram -d nullmoth-gpu-once 2>/dev/null || true
nvram -d nullmoth-panel-once 2>/dev/null || true
launchctl bootout system/com.ved.latitude-panel-load 2>/dev/null || true
for path in /Library/Extensions/MyIntelGPU.kext /Library/Extensions/NMXGPU.kext /Library/Extensions/NMXAccel.kext /Library/Extensions/NMXAGDC.kext /Library/Extensions/LatitudeMTLFB.kext \
 /Library/GPUBundles/NVMTLDriver.bundle /Library/GPUBundles/nvmtl /Library/GPUBundles/nvmtl-allow.txt \
 /Library/NullMoth/firmware/i915 /Library/NullMoth/load-panel-framebuffer.sh /Library/LaunchDaemons/com.ved.latitude-panel-load.plist /Library/KernelCollections/AuxiliaryKernelExtensions.kc; do
  rm -rf "$path"
  if [ -e "$BACKUP/original$path" ] || [ -L "$BACKUP/original$path" ]; then
    mkdir -p "$(dirname "$path")"; ditto "$BACKUP/original$path" "$path"
  fi
done
if [ -f "$BACKUP/usb-efi/config.plist" ]; then
  USBROOT=$(cat "$BACKUP/usb-root.txt")
  [ "$USBROOT" = /Volumes/SEQOUIA ] && [ -d "$USBROOT/EFI/OC" ] || { echo 'Reconnect SEQOUIA to restore its EFI.' >&2; exit 2; }
  rm -rf "$USBROOT/EFI/OC/Kexts/LatitudeMTLFB.kext"
  if [ -d "$BACKUP/usb-efi/LatitudeMTLFB.kext" ]; then ditto "$BACKUP/usb-efi/LatitudeMTLFB.kext" "$USBROOT/EFI/OC/Kexts/LatitudeMTLFB.kext"; fi
  rm -rf "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext"
  if [ -d "$BACKUP/usb-efi/LatitudePanelClaim.kext" ]; then ditto "$BACKUP/usb-efi/LatitudePanelClaim.kext" "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext"; fi
  cp "$BACKUP/usb-efi/config.plist" "$USBROOT/EFI/OC/config.plist"
fi
kcditto
printf 'Runtime restored from %s. Reboot to use the restored collection.\n' "$BACKUP"
echo 'USB security settings are separate; their previous config is backed up under iGPU-Port/backups on the USB.'
