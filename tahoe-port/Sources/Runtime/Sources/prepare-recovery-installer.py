from pathlib import Path
b=Path(__file__).resolve().parent;p=b/'Tahoe-Combined-Runtime';s=(p/'install-windowserver.sh').read_text()
s=s.replace('# Combined native Tahoe graphics and ALC295 audio transaction.', '# Offline Recovery transaction for the installed Tahoe System/Data APFS volume group.')
start=s.index('case "$(sw_vers -productVersion)"');end=s.index("hash() {")
s=s[:start]+'''[ -d /System/Installation/CDIS/Recovery\\ Springboard.app ] || { echo 'Run this from Utilities > Terminal in the USB Recovery, not the installed desktop.';exit 2; }
TARGET=${1:-/Volumes/macOS}
REPORT="$HERE/reports/recovery-install-$(date +%Y%m%d-%H%M%S)";mkdir -p "$REPORT"
exec > >(tee "$REPORT/install.txt") 2>&1
(cd "$HERE"; /usr/bin/shasum -a 256 -c SHA256SUMS.txt) > "$REPORT/package-hashes-initial.txt"
"$HERE/recovery-target" "$TARGET" > "$REPORT/target.plist"
SYSTEM=$("$PB" -c 'Print :SystemRoot' "$REPORT/target.plist")
DATA=$("$PB" -c 'Print :DataRoot' "$REPORT/target.plist")
PREBOOT=$("$PB" -c 'Print :PrebootRoot' "$REPORT/target.plist")
GROUP=$("$PB" -c 'Print :VolumeGroup' "$REPORT/target.plist")
SYSTEMUUID=$("$PB" -c 'Print :SystemUUID' "$REPORT/target.plist")
BUILD=$("$PB" -c 'Print :Build' "$REPORT/target.plist")
echo "Installing into APFS group $GROUP: System=$SYSTEM Data=$DATA"
'''+s[end:]
s=s.replace('"$BUILD" /','"$BUILD" "$SYSTEM"')
start=s.index('/usr/bin/kmutil showloaded');end=s.index('/usr/sbin/ioreg',start)
s=s[:start]+'''# AMFIPass need not load in Recovery: the frozen target EFI retains its enabled entry and AMFI bypass.
/usr/bin/kmutil showloaded > "$REPORT/recovery-loaded.txt" 2>&1 || true
'''+s[end:]
# Keep ephemeral work on Recovery RAM storage; all persistent installation paths belong to the explicit target.
s=s.replace('/Library/', '$DATA/Library/')
# Shell assignments and paths that acquired whitespace must be quoted, while case patterns/array entries are already quoted below.
s=s.replace('BKC=/System$DATA/Library/KernelCollections/BootKernelExtensions.kc','BKC="$SYSTEM/System/Library/KernelCollections/BootKernelExtensions.kc"')
s=s.replace('SKC=/System$DATA/Library/KernelCollections/SystemKernelExtensions.kc','SKC="$SYSTEM/System/Library/KernelCollections/SystemKernelExtensions.kc"')
s=s.replace('AUX=$DATA/Library/KernelCollections/AuxiliaryKernelExtensions.kc','AUX="$DATA/Library/KernelCollections/AuxiliaryKernelExtensions.kc"')
s=s.replace('BACKUP="$DATA/Library/', 'BACKUP="$DATA/Library/')
# Quote literal target paths everywhere they are used as shell words.
import re
s=re.sub(r'(?<!["A-Za-z0-9_])\$DATA/Library/[^\s";|)]*',lambda m:'"'+m.group(0)+'"',s)
s=s.replace('"$DATA/Library/Extensions/*.kext"','"$DATA/Library/Extensions/"*.kext')
s=s.replace('src="$HERE$path"','src="$HERE${path#"$DATA"}"')
s=s.replace('/System/Volumes/Preboot','"$PREBOOT"')
s=s.replace('-name AuxiliaryKernelExtensions.kc','-name \'*.kc\'')
s=s.replace('--volume-root / ', '--volume-root "$SYSTEM" --arch x86_64 ')
s=s.replace('"$root/$name.kext" "$(cat "$HERE/$name-VERSION.txt")" "$(cat "$HERE/$name-ID.txt")" "$name"', '"$root/$name.kext" "$(cat "$HERE/$name-VERSION.txt")" "$(cat "$HERE/$name-ID.txt")" "$name" || {\n   [ "$root" = "$DATA/Library/Extensions" ] && "$HERE/validate-kernel-prelink" "$REPORT/$label-prelink.txt" "$(cat "$HERE/$name-UUID.txt")" "/Library/Extensions/$name.kext" "$(cat "$HERE/$name-VERSION.txt")" "$(cat "$HERE/$name-ID.txt")" "$name";\n  }')
s=s.replace('/usr/sbin/kcditto > "$REPORT/kcditto.txt" 2>&1', '''# kmutil's explicit target-volume route performs the correct Preboot publication; bare kcditto would target Recovery.
/usr/bin/kmutil install --volume-root "$SYSTEM" --arch x86_64 --force --allow-missing-kdk --update-preboot > "$REPORT/target-cache-install.txt" 2>&1
validate_kc "$AUX" "$DATA/Library/Extensions" published
find "$PREBOOT" -type f -name AuxiliaryKernelExtensions.kc -print0 > "$WORK/published-aux.nul"
preboot_verified=0
while IFS= read -r -d '' path;do
 case "$path" in "$PREBOOT/$GROUP/"*|"$PREBOOT/$SYSTEMUUID/"*) validate_kc "$path" "$DATA/Library/Extensions" preboot-published;preboot_verified=1;; *) continue;;esac
done < "$WORK/published-aux.nul"
[ "$preboot_verified" = 1 ] || { echo 'No exact four-kext AuxKC was verified in the target Preboot subject; rolling back.';exit 2; }''')
s=s.replace('cp "$HERE/rollback-windowserver.sh" "$BACKUP/rollback-windowserver.sh"','cp "$HERE/rollback-recovery-install.sh" "$BACKUP/rollback-windowserver.sh"\nprintf \'%s\\n\' "$DATA" > "$BACKUP/data-root.txt"\nprintf \'%s\\n\' "$PREBOOT" > "$BACKUP/preboot-root.txt"')
s=s.replace('Run with sudo on the Intel Dell, in installed Tahoe.', 'Run as root from USB Recovery Terminal on the Intel Dell.')
s=s.replace("echo 'Allow the graphics extensions if macOS requests it, then reboot through this USB.'", "echo 'Reboot through this USB and select macOS (the installed disk). Setup should resume; audio and graphics still need the real boot check.'")
s=s.replace('$HERE$DATA/Library/','$HERE/Library/')
s=s.replace('\"$name\";\n  }', '\"$name\" || return 2;\n  }')
s=s.replace('/usr/bin/shasum -a 256 -c SHA256SUMS.txt', '"$HERE/recovery-sha256" --check "$HERE" "$HERE/SHA256SUMS.txt"')
s=s.replace("hash() { /usr/bin/shasum -a 256 \"$1\" | /usr/bin/awk '{print $1}'; }", 'hash() { "$HERE/recovery-sha256" --file "$1"; }')
s=s.replace('cmp -s', '"$HERE/recovery-sha256" --same')
s=s.replace('cp "$HERE/USB-EFI-UUID.txt"', 'cp "$HERE/recovery-sha256" "$BACKUP/recovery-sha256"\ncp "$HERE/USB-EFI-UUID.txt"')
(p/'install-from-recovery.sh').write_text(s)
# Manual rollback is target-bound to the recorded Data/Preboot roots, never Recovery's own Library/cache.
r=(p/'rollback-windowserver.sh').read_text();r=r.replace('failed=0', '''DATA=$(cat "$BACKUP/data-root.txt")
PREBOOT=$(cat "$BACKUP/preboot-root.txt")
case "$DATA:$PREBOOT" in /Volumes/*:/Volumes/*|/Volumes/*:/System/Volumes/Preboot) ;; *) echo 'Invalid saved target roots.';exit 2;;esac
failed=0''',1)
r=r.replace('/Library/', '$DATA/Library/');r=re.sub(r'(?<!["A-Za-z0-9_])\$DATA/Library/[^\s";|)]*',lambda m:'"'+m.group(0)+'"',r)
r=r.replace('/System/Volumes/Preboot','"$PREBOOT"').replace('-name AuxiliaryKernelExtensions.kc',"-name '*.kc'")
# Repair the literal root validation pattern, which intentionally permits the normal Preboot mount.
r=r.replace('/Volumes/*:"$PREBOOT")','/Volumes/*:/System/Volumes/Preboot)')
r=r.replace('cmp -s', '"$BACKUP/recovery-sha256" --same')
(p/'rollback-recovery-install.sh').write_text(r)
print('Recovery installer and target-bound rollback generated.')
