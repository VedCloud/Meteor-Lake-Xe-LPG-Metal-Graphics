#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
[ "$(uname -m)" = x86_64 ] && [ "$(id -u)" = 0 ] || { echo 'Run with sudo on the Intel Dell fallback desktop.'; exit 2; }
[ "$(sw_vers -buildVersion)" = 24H32 ] || { echo 'This candidate targets Sequoia24H32.'; exit 2; }
REPORT="$HERE/reports/windowserver-install-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$REPORT"
ioreg -r -n GFX0 -l -w0 > "$REPORT/GFX0.txt"
grep -q '"device-id" = <457d0000>' "$REPORT/GFX0.txt" || { echo 'Expected8086:7D45 is not visible.'; exit 2; }
ioreg -r -c LatitudePanelClaim -l -w0 > "$REPORT/panel.txt"
if grep -q LatitudePanelClaim "$REPORT/panel.txt"; then echo 'Boot the fallback desktop before updating WindowServer.'; exit 2; fi
cd "$HERE"
shasum -a 256 -c SHA256SUMS.txt > "$REPORT/package-check.txt"
cd "$HERE/.."
shasum -a 256 -c "$HERE/WINDOWSERVER-BASELINE-SHA256SUMS.txt" > "$REPORT/baseline-check.txt"
echo 'Verified the saved22case/26000frame baseline. No GPU tests will run.'
kmutil showloaded > "$REPORT/loaded-kexts.txt" 2>&1
for spec in 'NMXAccel 0.2.4'; do
  set -- $spec
  expected_uuid=$(cat "$HERE/$1-UUID.txt")
  grep -F "com.nullmoth.$1 ($2)" "$REPORT/loaded-kexts.txt" | grep -F "$expected_uuid" >/dev/null || { echo "Loaded $1 differs from the verified baseline; no changes made."; exit 2; }
  cmp -s "$HERE/Library/Extensions/$1.kext/Contents/MacOS/$1" "/Library/Extensions/$1.kext/Contents/MacOS/$1" || { echo "Installed $1 differs from baseline."; exit 2; }
done
# Accept only the frozen baseline kernel or this exact diagnostic kernel.
kernel_source=''
for candidate in baseline previous diagnostic; do
  if [ "$candidate" = baseline ]; then version=1.0.8; uuid_file="$HERE/NMXGPU-UUID.txt"; binary="$HERE/Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU"
  elif [ "$candidate" = previous ]; then version=1.0.14; uuid_file="$HERE/PreviousWindowServerKernel/NMXGPU-UUID.txt"; binary="$HERE/PreviousWindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU"
  else version=1.0.15; uuid_file="$HERE/WindowServerKernel/NMXGPU-UUID.txt"; binary="$HERE/WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU"; fi
  if grep -F "com.nullmoth.NMXGPU ($version)" "$REPORT/loaded-kexts.txt" | grep -F "$(cat "$uuid_file")" >/dev/null; then
    cmp -s "$binary" /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU || { echo 'Loaded/installed kernel mismatch; boot fallback once before installing.'; exit 2; }
    kernel_source=$candidate; break
  fi
done
if [ -z "$kernel_source" ] && ! grep -F 'com.nullmoth.NMXGPU (' "$REPORT/loaded-kexts.txt" >/dev/null && { cmp -s "$HERE/WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU" /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU || cmp -s "$HERE/PreviousWindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU" /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU; }; then
  kernel_source=cache-repair
  echo 'Known diagnostic kernel is installed but not loaded; repairing its kernel collection from permanent paths.'
  # The failed load may have left GPU opt-in unconsumed. Park it during repair.
  nvram -d nullmoth-gpu-once 2>/dev/null || true
  nvram -d nullmoth-panel-once 2>/dev/null || true
fi
[ -n "$kernel_source" ] || { echo 'Unexpected loaded GPU kernel; no changes made.'; exit 2; }
cmp -s "$HERE/Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB" /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB || cmp -s "$HERE/WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB" /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB || cmp -s "$HERE/PreviousWindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB" /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB || { echo 'Unexpected installed framebuffer; no changes made.'; exit 2; }
codesign --verify --strict "$HERE/WindowServerFramebuffer/LatitudeMTLFB.kext"
codesign --verify --strict "$HERE/validate-kernel-prelink"
codesign --verify --strict "$HERE/WindowServerKernel/NMXGPU.kext"
grep -F 'com.dhinakg.AMFIPass' "$REPORT/loaded-kexts.txt" >/dev/null || { echo 'AMFIPass is not loaded; no changes made.'; exit 2; }
TRANSPORT=/Library/GPUBundles/nvmtl/libdrm.2.dylib
NEW_TRANSPORT="$HERE/WindowServerTransport/libdrm.2.dylib"
cmp -s "$HERE/Library/GPUBundles/nvmtl/libdrm.2.dylib" "$TRANSPORT" || cmp -s "$NEW_TRANSPORT" "$TRANSPORT" || { echo 'Unexpected installed transport; no changes made.'; exit 2; }
codesign --verify --strict "$NEW_TRANSPORT"
BACKEND=/Library/GPUBundles/nvmtl/libvulkan_intel.dylib
NEW_BACKEND="$HERE/WindowServerBackend/libvulkan_intel.dylib"
cmp -s "$HERE/Library/GPUBundles/nvmtl/libvulkan_intel.dylib" "$BACKEND" || cmp -s "$NEW_BACKEND" "$BACKEND" || { echo 'Unexpected installed ANV backend; no changes made.'; exit 2; }
codesign --verify --strict "$NEW_BACKEND"
codesign --verify --strict "$HERE/WindowServerPlugin/NVMTLDriver.bundle"
codesign --verify --strict "$HERE/Library/NullMoth/latitude-desktop-journal"
/bin/bash -n "$HERE/Library/NullMoth/load-panel-framebuffer.sh"
DEST=/Library/GPUBundles/NVMTLDriver.bundle
HELPER=/Library/NullMoth/latitude-desktop-journal
LOADER=/Library/NullMoth/load-panel-framebuffer.sh
IDENTITY=/Library/NullMoth/latitude-gpu-identity
codesign --verify --strict "$HERE/Library/NullMoth/latitude-gpu-identity"
[ -d "$DEST" ] || { echo 'Existing runtime bundle missing; install the base runtime first.'; exit 2; }
BACKUP="/Library/NullMoth/latitude5350-backups/windowserver-$(date +%Y%m%d-%H%M%S)"
mkdir -p "$BACKUP"
cp -p "$TRANSPORT" "$BACKUP/libdrm.2.dylib"
cp -p "$BACKEND" "$BACKUP/libvulkan_intel.dylib"
WORK=$(mktemp -d /Library/GPUBundles/.latitude-windowserver.XXXXXX)
# Capture every file before changing any runtime component.
for file in "$HELPER" "$LOADER" "$IDENTITY"; do
  if [ -e "$file" ]; then cp -p "$file" "$BACKUP/$(basename "$file")"; else touch "$BACKUP/$(basename "$file").absent"; fi
done
changed=0
success=0
kernel_changed=0
source "$HERE/update-windowserver-kernel.sh"
cleanup() {
  result=$?
  trap - EXIT
  if [ "$success" != 1 ] && [ "$changed" = 1 ]; then
    nvram -d nullmoth-panel-once 2>/dev/null || true
    nvram -d nullmoth-gpu-once 2>/dev/null || true
    restore_failed=0
    if ! cp -p "$BACKUP/libdrm.2.dylib" "$TRANSPORT.rollback" || ! mv -f "$TRANSPORT.rollback" "$TRANSPORT"; then
      restore_failed=1
      echo "Transport rollback needs attention; backup: $BACKUP/libdrm.2.dylib"
    fi
    if ! cp -p "$BACKUP/libvulkan_intel.dylib" "$BACKEND.rollback" || ! mv -f "$BACKEND.rollback" "$BACKEND"; then
      restore_failed=1
      echo "ANV backend rollback needs attention: $BACKUP/libvulkan_intel.dylib"
    fi
    if ! rollback_windowserver_kernel; then
      restore_failed=1
      echo "Kernel rollback needs attention; retained work: $WORK; backup: $BACKUP"
    fi
    for file in "$HELPER" "$LOADER" "$IDENTITY"; do
      name=$(basename "$file")
      if [ -f "$BACKUP/$name.absent" ]; then rm -f "$file"; else cp -p "$BACKUP/$name" "$file"; fi
    done
    if [ -d "$WORK/previous.bundle" ]; then
      rm -rf "$DEST"
      mv "$WORK/previous.bundle" "$DEST"
      echo 'Desktop preparation failed; previous plugin and capture tools restored and both opt-ins cleared.'
    fi
  fi
  if [ "${restore_failed:-0}" = 1 ]; then exit 3; fi
  rm -rf "$WORK"
  exit "$result"
}
trap cleanup EXIT
prepare_windowserver_kernel
ditto "$HERE/WindowServerPlugin/NVMTLDriver.bundle" "$WORK/new.bundle"
chown -R root:wheel "$WORK/new.bundle"
chmod -R a+rX,go-w "$WORK/new.bundle"
codesign --verify --strict "$WORK/new.bundle"
mv "$DEST" "$WORK/previous.bundle"
changed=1
mv "$WORK/new.bundle" "$DEST"
cp -p "$NEW_TRANSPORT" "$TRANSPORT.new"
chown root:wheel "$TRANSPORT.new"
chmod 755 "$TRANSPORT.new"
mv -f "$TRANSPORT.new" "$TRANSPORT"
codesign --verify --strict "$TRANSPORT"
cmp -s "$NEW_TRANSPORT" "$TRANSPORT"
cp -p "$NEW_BACKEND" "$BACKEND.new"
chown root:wheel "$BACKEND.new"
chmod 755 "$BACKEND.new"
mv -f "$BACKEND.new" "$BACKEND"
codesign --verify --strict "$BACKEND"
cmp -s "$NEW_BACKEND" "$BACKEND"
codesign --verify --strict "$DEST"
cmp -s "$HERE/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver" "$DEST/Contents/MacOS/NVMTLDriver"
ditto "$WORK/previous.bundle" "$BACKUP/NVMTLDriver.bundle"
for file in "$HELPER" "$LOADER" "$IDENTITY"; do
  cp "$HERE/Library/NullMoth/$(basename "$file")" "$file.new"
  chown root:wheel "$file.new"
  chmod 755 "$file.new"
  mv -f "$file.new" "$file"
  cmp -s "$HERE/Library/NullMoth/$(basename "$file")" "$file"
done
codesign --verify --strict "$HELPER"
codesign --verify --strict "$IDENTITY"
"$HELPER" prepare
commit_windowserver_kernel
"$HELPER" stage installer-ready scanout-reservation-1.0.15 0
shasum -a 256 "$HELPER" "$LOADER" "$IDENTITY" > "$REPORT/installed-capture-tools.txt"
shasum -a 256 "$DEST/Contents/MacOS/NVMTLDriver" > "$REPORT/installed-plugin.txt"
shasum -a 256 "$TRANSPORT" > "$REPORT/installed-transport.txt"
shasum -a 256 "$BACKEND" > "$REPORT/installed-backend.txt"
printf '%s\n' "$BACKUP" > "$REPORT/backup.txt"
date +%s > "$HERE/reports/windowserver-attempt-start.txt"
reset_windowserver_attach_guard
nvram nullmoth-gpu-once=1
nvram nullmoth-panel-once=1
nvram nullmoth-gpu-once > "$REPORT/armed-markers.txt"
nvram nullmoth-panel-once >> "$REPORT/armed-markers.txt"
success=1
echo "WindowServer scanout-reservation-1.0.15 installed. Backup: $BACKUP"
echo 'Includes NMXGPU1.0.15: validated firmware scanout prefix reserved; remaining stolen memory restored for i915. Completion-context correction retained. LatitudeMTLFB0.2.5 publishes driver-queried PCI model and shared-memory capacity. Reporting candidate needs next-boot validation.'
echo 'Metal plugin0.3.12: separate sampled swizzles from identity attachment/storage views; tight readbacks use one bulk copy. Cache visibility/fresh pools/fences unchanged. Bounded Weather and slow-part/callback diagnostics added; Weather text and CoreAnimation timeout fixes require target validation.'
echo 'Includes WindowServer IOKit transport correction: denied writable render opens use an IOKit token; device identity uses the established DRM file. Startup ioctl results captured.'
echo 'ONE desktop boot armed. Reboot through this USB once when ready.'
echo 'No hardware suite was rerun. The installer has not rebooted the laptop.'
echo 'If the panel fails, power off and boot this USB again for fallback; run collect-desktop-failure.sh.'

# Branding owns its snapshot/backup transaction. A cosmetic failure must not undo
# the already committed and armed graphics candidate.
if ! /bin/bash "$HERE/install-about-this-mac.sh" > "$REPORT/about-this-mac.txt" 2>&1; then
  cat "$REPORT/about-this-mac.txt"
  echo 'Graphics 0.3.12 installed and ONE boot armed; About This Mac branding failed. See the separate report above.'
  echo 'Retry cosmetics with sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/Desktop/install-about-this-mac.sh; no graphics tests required.'
  exit 4
fi
cat "$REPORT/about-this-mac.txt"
