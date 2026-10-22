from pathlib import Path
import copy,plistlib,hashlib
GUID='7C436110-AB2A-4BBB-A880-FE41995C9F82'
MARKERS=('nullmoth-gpu-once','nullmoth-panel-once')
def configure(document,accelerated):
 d=copy.deepcopy(document);nv=d['NVRAM'];add=nv.setdefault('Add',{}).setdefault(GUID,{});delete=nv.setdefault('Delete',{}).setdefault(GUID,[])
 for key in MARKERS:
  if key not in delete:delete.append(key)
  if accelerated:add[key]='1'
  else:add.pop(key,None)
 return d

def create_script(directory,pairs):
 lines=[]
 for profile in pairs:
  for mode in ['accelerated','recovery']:
   path=directory/(profile+'-'+mode+'.plist');h=hashlib.sha256(path.read_bytes()).hexdigest()
   lines.append(' '+h+') profile='+profile+' ;;')
 script='''#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/.." && pwd)"
CFG="$USB/EFI/OC/config.plist"
case "${1:-}" in accelerated|recovery) mode=$1;; *) echo 'Usage: switch-graphics-mode.sh accelerated|recovery'; exit 2;; esac
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "$(hash "$CFG")" in
'''+ '\n'.join(lines)+'''
 *) echo 'EFI configuration changed; refusing to replace unrelated changes. Regenerate graphics mode profiles.' >&2; exit 2;;
esac
SOURCE="$HERE/GraphicsModes/$profile-$mode.plist"
case "$profile-$mode:$(hash "$SOURCE")" in
'''
 patterns=[]
 for profile in pairs:
  for mode in ['accelerated','recovery']:
   h=hashlib.sha256((directory/(profile+'-'+mode+'.plist')).read_bytes()).hexdigest();patterns.append(profile+'-'+mode+':'+h)
 script+=' '+ '|'.join(patterns)+''') ;; *) echo 'Saved graphics profile failed verification.' >&2; exit 2;; esac
[ "$(hash "$CFG")" != "$(hash "$SOURCE")" ] || { echo "USB already selected: $mode ($profile)."; exit 0; }
# Retain a copy of exactly the active configuration before this reversible switch.
BACKUP=$(mktemp -d "$HERE/backups/graphics-mode-switch.XXXXXX")
cp -p "$CFG" "$BACKUP/config.plist"
cp "$SOURCE" "$CFG.graphics-tmp"
/usr/bin/plutil -lint "$CFG.graphics-tmp" >/dev/null
mv -f "$CFG.graphics-tmp" "$CFG"
echo "USB graphics mode: $mode ($profile). Reboot through this USB. Backup: $BACKUP"
'''
 return script
