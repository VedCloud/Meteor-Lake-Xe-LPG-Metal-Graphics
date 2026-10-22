#!/bin/bash
# Sourced by the guarded desktop installer. No GPU tests or automatic reboot.
prepare_windowserver_kernel() {
  KERNEL_DEST=/Library/Extensions/NMXGPU.kext
  FB_DEST=/Library/Extensions/LatitudeMTLFB.kext
  framebuffer_changed=0
  AUX=/Library/KernelCollections/AuxiliaryKernelExtensions.kc
  BKC=/System/Library/KernelCollections/BootKernelExtensions.kc
  SKC=/System/Library/KernelCollections/SystemKernelExtensions.kc
  kernel_changed=0
  kernel_update=0
  # Rebuild even for identical kernel bytes: a previous cache may embed a
  # staging path and cannot be validated by a binary comparison alone.
  kernel_update=1
  [ -f "$BKC" ] && [ -f "$SKC" ] || { echo 'System kernel collections missing; no runtime changes made.'; return 2; }
  ditto "$KERNEL_DEST" "$BACKUP/NMXGPU.kext"
  ditto "$FB_DEST" "$BACKUP/LatitudeMTLFB.kext"
  ditto "$HERE/WindowServerFramebuffer/LatitudeMTLFB.kext" "$WORK/LatitudeMTLFB.kext"
  chown -R root:wheel "$WORK/LatitudeMTLFB.kext"
  chmod -R a+rX,go-w "$WORK/LatitudeMTLFB.kext"
  codesign --verify --strict "$WORK/LatitudeMTLFB.kext"
  if [ -f "$AUX" ]; then cp -p "$AUX" "$BACKUP/previous-aux.kc"; else touch "$BACKUP/aux.absent"; fi
  ditto "$HERE/WindowServerKernel/NMXGPU.kext" "$WORK/NMXGPU.kext"
  chown -R root:wheel "$WORK/NMXGPU.kext"
  chmod -R a+rX,go-w "$WORK/NMXGPU.kext"
  codesign --verify --strict "$WORK/NMXGPU.kext"
  local args=() kext
  for kext in /Library/Extensions/*.kext; do
    [ ! -d "$kext" ] || [ "$kext" = "$KERNEL_DEST" ] || [ "$kext" = "$FB_DEST" ] || args+=( -p "$kext" )
  done
  args+=( -p "$WORK/NMXGPU.kext" -p "$WORK/LatitudeMTLFB.kext" )
  # Build the whole next AuxKC before changing installed binaries/cache.
  kmutil create -n aux -B "$BKC" -S "$SKC" -A "$WORK/next-aux.kc" "${args[@]}" -M --allow-missing-kdk > "$REPORT/kernel-preflight.txt" 2>&1
  [ -s "$WORK/next-aux.kc" ] || { echo 'No candidate auxiliary collection produced.'; return 2; }
}
commit_windowserver_kernel() {
  [ "$kernel_update" = 1 ] || return 0
  mv "$KERNEL_DEST" "$WORK/previous.kernel"
  kernel_changed=1
  mv "$WORK/NMXGPU.kext" "$KERNEL_DEST"
  codesign --verify --strict "$KERNEL_DEST"
  mv "$FB_DEST" "$WORK/previous.framebuffer"
  framebuffer_changed=1
  mv "$WORK/LatitudeMTLFB.kext" "$FB_DEST"
  codesign --verify --strict "$FB_DEST"
  mkdir -p /Library/KernelCollections
  local args=() kext
  for kext in /Library/Extensions/*.kext; do [ ! -d "$kext" ] || args+=( -p "$kext" ); done
  # Preflight used a temporary bundle. NEVER install that preflight KC: the
  # collection records bundle paths and the temporary directory is removed.
  kmutil create -n aux -B "$BKC" -S "$SKC" -A "$WORK/installed-aux.kc" "${args[@]}" -M --allow-missing-kdk > "$REPORT/kernel-final-build.txt" 2>&1
  kmutil inspect -B "$BKC" -S "$SKC" -A "$WORK/installed-aux.kc" --show-prelink-info > "$REPORT/kernel-final-path.txt" 2>&1
  "$HERE/validate-kernel-prelink" "$REPORT/kernel-final-path.txt" "$(cat "$HERE/WindowServerKernel/NMXGPU-UUID.txt")" "$KERNEL_DEST" "$(cat "$HERE/WindowServerKernel/NMXGPU-VERSION.txt")" > "$REPORT/kernel-final-validation.txt" 2>&1 || { echo 'Final kernel collection GPU record failed validation.'; return 2; }
  "$HERE/validate-kernel-prelink" "$REPORT/kernel-final-path.txt" "$(cat "$HERE/WindowServerFramebuffer/LatitudeMTLFB-UUID.txt")" "$FB_DEST" "$(cat "$HERE/WindowServerFramebuffer/LatitudeMTLFB-VERSION.txt")" com.nullmoth.LatitudeMTLFB LatitudeMTLFB > "$REPORT/framebuffer-final-validation.txt" 2>&1 || { echo 'Final kernel collection framebuffer record failed validation.'; return 2; }
  cp "$WORK/installed-aux.kc" "$AUX.latitude-tmp"
  chown root:wheel "$AUX.latitude-tmp"
  chmod 644 "$AUX.latitude-tmp"
  mv -f "$AUX.latitude-tmp" "$AUX"
  kcditto > "$REPORT/kernel-kcditto.txt" 2>&1
  cmp -s "$HERE/WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU" "$KERNEL_DEST/Contents/MacOS/NMXGPU"
  cmp -s "$HERE/WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB" "$FB_DEST/Contents/MacOS/LatitudeMTLFB"
  shasum -a 256 "$FB_DEST/Contents/MacOS/LatitudeMTLFB" "$KERNEL_DEST/Contents/MacOS/NMXGPU" "$AUX" > "$REPORT/installed-kernel-and-cache.txt"
}
rollback_windowserver_kernel() {
  if [ "$kernel_changed" != 1 ] && [ ! -d "$WORK/previous.kernel" ]; then return 0; fi
  # Never delete the installed bundle unless its replacement is available.
  [ -d "$WORK/previous.kernel" ] || { echo 'Previous kernel backup unavailable; installed bundle preserved.'; return 1; }
  if [ "$framebuffer_changed" = 1 ]; then [ -d "$WORK/previous.framebuffer" ] || { echo 'Framebuffer backup missing; installed bundles preserved.'; return 1; }; fi
  rm -rf "$KERNEL_DEST"
  mv "$WORK/previous.kernel" "$KERNEL_DEST" || return 1
  if [ "$framebuffer_changed" = 1 ]; then rm -rf "$FB_DEST"; mv "$WORK/previous.framebuffer" "$FB_DEST" || return 1; fi
  if [ -f "$BACKUP/aux.absent" ]; then rm -f "$AUX"; else cp -p "$BACKUP/previous-aux.kc" "$AUX" || return 1; fi
  kcditto > "$REPORT/kernel-rollback-kcditto.txt" 2>&1 || return 1
  echo 'Previous GPU kernel, framebuffer and auxiliary collection restored.'
}

# Only an explicit, validated installer retry may clear a failed attach marker.
# Keep the kernel guard itself active so another failed attach parks next boot.
reset_windowserver_attach_guard() {
  if nvram nullmoth-gpu-attaching > "$REPORT/attach-guard-before.txt" 2> "$REPORT/attach-guard-query.txt"; then
    echo 'Clearing the previous failed-attach marker for this explicit one-shot retry.'
    nvram -d nullmoth-gpu-attaching > "$REPORT/attach-guard-delete.txt" 2>&1 || {
      echo 'Cannot clear failed-attach marker; no desktop boot will be armed.'
      return 2
    }
    if nvram nullmoth-gpu-attaching > "$REPORT/attach-guard-after.txt" 2>&1; then
      echo 'Failed-attach marker is still present; no desktop boot will be armed.'
      return 2
    fi
    echo 'Previous failed-attach marker cleared; kernel boot guard remains enabled.' > "$REPORT/attach-guard-status.txt"
  else
    echo 'No failed-attach marker reported; kernel boot guard remains enabled.' > "$REPORT/attach-guard-status.txt"
  fi
}
