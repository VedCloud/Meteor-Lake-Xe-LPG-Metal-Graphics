#!/bin/bash
set -euo pipefail
MODE=${1:---readback-only}
case "$MODE" in --readback-only) ;; *) echo 'Candidate 0.3.1 supports --readback-only; desktop activation requires the full test-and-arm-desktop.sh gate.' >&2; exit 2 ;; esac
echo "Latitude candidate 0.3.2 — $MODE"
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(id -u)" = 0 ] || { echo "Run with sudo on the Dell after rebooting through the updated USB." >&2; exit 2; }
[ "$(uname -m)" = x86_64 ] || { echo 'This installer is only for the Intel Dell.' >&2; exit 2; }
[ "$(sw_vers -productVersion | cut -d. -f1)" = 15 ] || { echo 'This build targets macOS Sequoia 15.' >&2; exit 2; }
INSTALL_REPORT="$HERE/reports/install-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$INSTALL_REPORT"
cpu=$(sysctl -n machdep.cpu.brand_string)
case "$cpu" in *'Ultra 5 135U'*) ;; *) echo 'CPU does not match the Latitude profile.' >&2; exit 2 ;; esac
ioreg -r -n GFX0 -l -w0 > "$INSTALL_REPORT/GFX0-ioreg.txt"
grep -q '"device-id" = <457d0000>' "$INSTALL_REPORT/GFX0-ioreg.txt" || { echo 'Real 8086:7D45 GFX0 not visible.' >&2; exit 2; }
csrutil status > "$INSTALL_REPORT/sip-status.txt"
cat "$INSTALL_REPORT/sip-status.txt"
if ! grep -qE 'status: disabled|Kext Signing: disabled' "$INSTALL_REPORT/sip-status.txt"; then
  echo "Unsigned kext loading is still restricted; details: $INSTALL_REPORT" >&2; exit 2
fi
# Capture the whole producer output before searching: grep -q can otherwise
# close a pipe early and make pipefail treat a successful match as a failure.
if ! kmutil showloaded > "$INSTALL_REPORT/loaded-kexts.txt" 2> "$INSTALL_REPORT/kmutil-showloaded.stderr.txt"; then
  cat "$INSTALL_REPORT/kmutil-showloaded.stderr.txt" >&2
  echo "Cannot read loaded extensions; details: $INSTALL_REPORT" >&2; exit 2
fi
if ! grep -Fq 'com.dhinakg.AMFIPass' "$INSTALL_REPORT/loaded-kexts.txt"; then
  ioreg -r -c AMFIPass -l -w0 > "$INSTALL_REPORT/AMFIPass-ioreg.txt" 2>&1 || true
  sysctl -n kern.bootargs > "$INSTALL_REPORT/bootargs.txt" 2>&1 || true
  log show --last boot --style compact --predicate 'eventMessage CONTAINS[c] "AMFIPass" OR eventMessage CONTAINS[c] "Lilu"' > "$INSTALL_REPORT/amfipass-log.txt" 2>&1 || true
  echo "AMFIPass was not found in the complete loaded-extension list. No driver files changed." >&2
  echo "Bring back the USB reports folder: $INSTALL_REPORT" >&2
  exit 2
fi
cd "$HERE"
shasum -a 256 -c SHA256SUMS.txt > /tmp/latitude5350-package-check.txt || { cat /tmp/latitude5350-package-check.txt; exit 2; }
BKC=/System/Library/KernelCollections/BootKernelExtensions.kc
SKC=/System/Library/KernelCollections/SystemKernelExtensions.kc
AUX=/Library/KernelCollections/AuxiliaryKernelExtensions.kc
[ -f "$BKC" ] && [ -f "$SKC" ] || { echo 'Target kernel collections not found.' >&2; exit 2; }
WORK=$(mktemp -d /private/var/tmp/latitude5350-preflight.XXXXXX)
ditto "$HERE/Library/Extensions" "$WORK/Extensions"
chown -R root:wheel "$WORK/Extensions"
ditto "$HERE/EFI-Kexts/LatitudePanelClaim.kext" "$WORK/Extensions/LatitudePanelClaim.kext"
chown -R root:wheel "$WORK/Extensions/LatitudePanelClaim.kext"
chmod -R a+rX,go-w "$WORK/Extensions"
echo 'Checking the desktop candidate against this laptop kernel before installing...'
if ! kmutil create -n aux -B "$BKC" -S "$SKC" -A "$WORK/preflight.kc" \
  -p "$WORK/Extensions/NMXGPU.kext" -p "$WORK/Extensions/NMXAccel.kext" -p "$WORK/Extensions/NMXAGDC.kext" -p "$WORK/Extensions/LatitudeMTLFB.kext" -p "$WORK/Extensions/LatitudePanelClaim.kext" -x -M --allow-missing-kdk > "$WORK/kmutil-preflight.log" 2>&1; then
  cp "$WORK/kmutil-preflight.log" "$INSTALL_REPORT/kmutil-preflight-failed.log"
  cat "$WORK/kmutil-preflight.log"
  echo "Kernel validation failed. No live driver files changed. Logs: $WORK" >&2; exit 1
fi
BACKUP="/Library/NullMoth/latitude5350-backups/$(date +%Y%m%d-%H%M%S)"
mkdir -p "$BACKUP/original"
PATHS=( /Library/Extensions/MyIntelGPU.kext /Library/Extensions/NMXGPU.kext /Library/Extensions/NMXAccel.kext /Library/Extensions/NMXAGDC.kext /Library/Extensions/LatitudeMTLFB.kext \
 /Library/GPUBundles/NVMTLDriver.bundle /Library/GPUBundles/nvmtl /Library/GPUBundles/nvmtl-allow.txt \
 /Library/NullMoth/firmware/i915 /Library/NullMoth/load-panel-framebuffer.sh /Library/LaunchDaemons/com.ved.latitude-panel-load.plist "$AUX" )
for path in "${PATHS[@]}"; do
  if [ -e "$path" ] || [ -L "$path" ]; then
    mkdir -p "$BACKUP/original$(dirname "$path")"
    ditto "$path" "$BACKUP/original$path"
  fi
done
USBROOT="$(cd "$HERE/../.." && pwd)"
[ "$USBROOT" = /Volumes/SEQOUIA ] && [ -f "$USBROOT/EFI/OC/config.plist" ] || { echo 'Desktop package must run from the SEQOUIA USB with its EFI present.' >&2; exit 2; }
mkdir -p "$BACKUP/usb-efi"
cp "$USBROOT/EFI/OC/config.plist" "$BACKUP/usb-efi/config.plist"
printf '%s\n' "$USBROOT" > "$BACKUP/usb-root.txt"
if [ -d "$USBROOT/EFI/OC/Kexts/LatitudeMTLFB.kext" ]; then ditto "$USBROOT/EFI/OC/Kexts/LatitudeMTLFB.kext" "$BACKUP/usb-efi/LatitudeMTLFB.kext"; fi
if [ -d "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext" ]; then ditto "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext" "$BACKUP/usb-efi/LatitudePanelClaim.kext"; fi
cp "$HERE/rollback-runtime.sh" "$BACKUP/rollback-runtime.sh"
cp "$WORK/kmutil-preflight.log" "$BACKUP/"
cp "$WORK/kmutil-preflight.log" "$INSTALL_REPORT/"
rollback_error() {
  trap - ERR
  cp "$BACKUP"/*.log "$INSTALL_REPORT/" 2>/dev/null || true
  echo "Installation failed; restoring the prior files from $BACKUP" >&2
  bash "$BACKUP/rollback-runtime.sh" "$BACKUP" || echo "Automatic rollback needs attention: $BACKUP" >&2
  exit 1
}
trap rollback_error ERR
# The old driver already failed authentication in the supplied reports; keep it in the backup.
if [ -d /Library/Extensions/MyIntelGPU.kext ]; then rm -rf /Library/Extensions/MyIntelGPU.kext; fi
for name in NMXGPU.kext NMXAccel.kext NMXAGDC.kext LatitudeMTLFB.kext; do
  rm -rf "/Library/Extensions/$name"
  ditto "$HERE/Library/Extensions/$name" "/Library/Extensions/$name"
  chown -R root:wheel "/Library/Extensions/$name"
  chmod -R a+rX,go-w "/Library/Extensions/$name"
done
mkdir -p /Library/GPUBundles /Library/NullMoth/firmware
chmod 755 /Library/GPUBundles
for name in NVMTLDriver.bundle nvmtl; do
  rm -rf "/Library/GPUBundles/$name"
  ditto "$HERE/Library/GPUBundles/$name" "/Library/GPUBundles/$name"
  chown -R root:wheel "/Library/GPUBundles/$name"
  chmod -R a+rX,go-w "/Library/GPUBundles/$name"
done
cp "$HERE/Library/GPUBundles/nvmtl-allow.txt" /Library/GPUBundles/nvmtl-allow.txt
chown root:wheel /Library/GPUBundles/nvmtl-allow.txt
chmod 644 /Library/GPUBundles/nvmtl-allow.txt
rm -rf /Library/NullMoth/firmware/i915
ditto "$HERE/Library/NullMoth/firmware/i915" /Library/NullMoth/firmware/i915
chown -R root:wheel /Library/NullMoth/firmware/i915
chmod -R a+rX,go-w /Library/NullMoth/firmware/i915
mkdir -p /Library/LaunchDaemons /Library/NullMoth
cp "$HERE/Library/NullMoth/load-panel-framebuffer.sh" /Library/NullMoth/load-panel-framebuffer.sh
chown root:wheel /Library/NullMoth/load-panel-framebuffer.sh
chmod 755 /Library/NullMoth/load-panel-framebuffer.sh
cp "$HERE/Library/LaunchDaemons/com.ved.latitude-panel-load.plist" /Library/LaunchDaemons/com.ved.latitude-panel-load.plist
chown root:wheel /Library/LaunchDaemons/com.ved.latitude-panel-load.plist
chmod 644 /Library/LaunchDaemons/com.ved.latitude-panel-load.plist
plutil -lint /Library/LaunchDaemons/com.ved.latitude-panel-load.plist
ARGS=()
for kext in /Library/Extensions/*.kext; do [ ! -d "$kext" ] || ARGS+=( -p "$kext" ); done
kmutil create -n aux -B "$BKC" -S "$SKC" -A "$WORK/installed.kc" "${ARGS[@]}" -M --allow-missing-kdk > "$BACKUP/kmutil-installed.log" 2>&1
mkdir -p /Library/KernelCollections
cp "$WORK/installed.kc" "$AUX.latitude-tmp"
chown root:wheel "$AUX.latitude-tmp"
chmod 644 "$AUX.latitude-tmp"
mv -f "$AUX.latitude-tmp" "$AUX"
kcditto > "$BACKUP/kcditto.log" 2>&1
PB=/usr/libexec/PlistBuddy
CFG="$WORK/desktop-config.plist"
cp "$USBROOT/EFI/OC/config.plist" "$CFG"
old_entry=0
while "$PB" -c "Print :Kernel:Add:$old_entry:BundlePath" "$CFG" >/dev/null 2>&1; do
  old_bundle=$("$PB" -c "Print :Kernel:Add:$old_entry:BundlePath" "$CFG")
  if [ "$old_bundle" = LatitudeMTLFB.kext ]; then "$PB" -c "Set :Kernel:Add:$old_entry:Enabled false" "$CFG"; fi
  old_entry=$((old_entry + 1))
done
entry=0
while "$PB" -c "Print :Kernel:Add:$entry:BundlePath" "$CFG" >/dev/null 2>&1; do
  bundle=$("$PB" -c "Print :Kernel:Add:$entry:BundlePath" "$CFG")
  [ "$bundle" != LatitudePanelClaim.kext ] || break
  entry=$((entry + 1))
done
if ! "$PB" -c "Print :Kernel:Add:$entry" "$CFG" >/dev/null 2>&1; then
  "$PB" -c "Add :Kernel:Add:$entry dict" "$CFG"
fi
for field in Arch BundlePath Comment Enabled ExecutablePath MaxKernel MinKernel PlistPath; do
  "$PB" -c "Delete :Kernel:Add:$entry:$field" "$CFG" >/dev/null 2>&1 || true
done
"$PB" -c "Add :Kernel:Add:$entry:Arch string x86_64" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:BundlePath string LatitudePanelClaim.kext" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:Comment string Latitude early panel reservation; one-shot opt-in" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:Enabled bool true" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:ExecutablePath string Contents/MacOS/LatitudePanelClaim" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:MinKernel string 24.0.0" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:MaxKernel string 24.99.99" "$CFG"
"$PB" -c "Add :Kernel:Add:$entry:PlistPath string Contents/Info.plist" "$CFG"
plutil -lint "$CFG"
rm -rf "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext"
ditto "$HERE/EFI-Kexts/LatitudePanelClaim.kext" "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext"
codesign --verify --strict "$USBROOT/EFI/OC/Kexts/LatitudePanelClaim.kext"
cp "$CFG" "$USBROOT/EFI/OC/config.plist.desktop-tmp"
mv -f "$USBROOT/EFI/OC/config.plist.desktop-tmp" "$USBROOT/EFI/OC/config.plist"
if [ "$MODE" = --desktop ]; then
  nvram nullmoth-panel-once=1
else
  # Probe sees no panel opt-in; retain firmware scanout for application tests.
  nvram -d nullmoth-panel-once 2>/dev/null || true
  if nvram nullmoth-panel-once >/dev/null 2>&1; then
    echo 'Panel one-shot is still present; isolated install refused.' >&2
    false
  fi
fi
# The patched driver consumes this variable once in probe(). No persistent nmxintel boot argument is added.
nvram nullmoth-gpu-once=1
trap - ERR
cp "$BACKUP"/*.log "$INSTALL_REPORT/"
printf '%s\n' "$BACKUP" > /Library/NullMoth/latitude5350-last-backup.txt
rm -rf "$WORK"
echo "Installed candidate 0.3.2 mode=$MODE. Backup: $BACKUP"
if [ "$MODE" = --readback-only ]; then
  echo 'Reboot through the same USB once. Keep the firmware desktop, then run:'
  echo 'sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh'
else
  echo 'Reboot through the same USB, then run collect-runtime.sh from this package.'
fi
echo 'If the test hangs, power-cycle and boot through the USB again; the one-shot opt-in should be consumed.'
echo 'The installer has not rebooted the laptop.'
