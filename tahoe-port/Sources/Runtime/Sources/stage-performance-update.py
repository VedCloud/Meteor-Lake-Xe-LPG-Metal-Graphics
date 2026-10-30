from pathlib import Path
import shutil,hashlib,plistlib,subprocess,json
b=Path(__file__).resolve().parent;p=b/'Tahoe-Combined-Runtime';old=b/'Performance-before-0.3.1/NMXAccel.kext';built=b/'Performance-build/NMXAccel.kext';dest=p/'Library/Extensions/NMXAccel.kext'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
# Keep all working hardware runtime files intact except the narrowly changed accelerator.
subprocess.run(['ditto',str(old),str(dest)],check=True);shutil.copy2(built/'Contents/MacOS/NMXAccel',dest/'Contents/MacOS/NMXAccel');d=plistlib.loads((dest/'Contents/Info.plist').read_bytes());d['CFBundleVersion']=d['CFBundleShortVersionString']='0.3.1';(dest/'Contents/Info.plist').write_bytes(plistlib.dumps(d));subprocess.run(['codesign','-f','-s','-',str(dest)],check=True);subprocess.run(['codesign','--verify','--strict',str(dest)],check=True)
(p/'NMXAccel-UUID.txt').write_text(subprocess.check_output([str(p/'tahoe-runtime-check'),'--uuid',str(dest/'Contents/MacOS/NMXAccel')],text=True));(p/'NMXAccel-VERSION.txt').write_text('0.3.1\n');(p/'ACCELERATOR-UPDATE-FROM-SHA256.txt').write_text(sha(old/'Contents/MacOS/NMXAccel')+'\n')
common=[]
for subtree in ['Library/Extensions/NMXGPU.kext','Library/Extensions/LatitudeMTLFB.kext','Library/Extensions/NMXAGDC.kext','Library/GPUBundles']:
 common.extend(f for f in (p/subtree).rglob('*') if f.is_file() and not f.name.startswith('._'))
(p/'ACCELERATOR-COMPANION-SHA256.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in sorted(common)))
f=p/'install-windowserver.sh';s=f.read_text();s=s.replace('PB=/usr/libexec/PlistBuddy', '''PB=/usr/libexec/PlistBuddy
MODE=full
case "${1:-}" in '') ;; --accelerator-only) MODE=accelerator-only;; *) echo 'Usage: install-windowserver.sh [--accelerator-only]';exit 2;;esac''',1)
s=s.replace("if grep -Fq 'class LatitudePanelClaim'", "if [ \"$MODE\" = full ] && grep -Fq 'class LatitudePanelClaim'",1)
needle='BKC=/System/Library/KernelCollections/BootKernelExtensions.kc'
s=s.replace(needle,'''if [ "$MODE" = accelerator-only ];then
 # This is the specific source-compatible 0.3.0 -> 0.3.1 on-disk update. It never unloads the live driver.
 [ "$(hash /Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel)" = "$(cat "$HERE/ACCELERATOR-UPDATE-FROM-SHA256.txt")" ] || { echo 'Accelerator-only update requires the exact installed0.3.0 baseline.';exit 2; }
 while read -r expected rel;do
  [ "$(hash "/$rel")" = "$expected" ] || { echo "Companion runtime differs: /$rel. Use the full installer in the explicit recovery profile instead.";exit 2; }
 done < "$HERE/ACCELERATOR-COMPANION-SHA256.txt"
 echo 'Qualified on-disk accelerator-only update; live renderer and firmware stay untouched until reboot.'
fi
'''+needle,1)
needle='backup_path() {';s=s.replace(needle,'if [ "$MODE" = accelerator-only ];then PATHS=( /Library/Extensions/NMXAccel.kext "$AUX" );fi\n'+needle,1)
needle='# Prepare configuration and CoreMedia changes before changing installed paths.';s=s.replace(needle,needle+'\nif [ "$MODE" = full ];then',1)
needle="echo 'Preflight includes all four exact kext identities. Installing the Tahoe runtime...'";s=s.replace(needle,'fi\n'+needle,1)
needle='mkdir -p /Library/Preferences/FeatureFlags/Domain';s=s.replace(needle,'if [ "$MODE" = full ];then\n'+needle,1)
needle='args=(${OTHER[@]+"${OTHER[@]}"})';# close the feature-flag block before the second args construction
at=s.index(needle,s.index('changed=1'));s=s[:at]+'fi\n'+s[at:]
needle='# Commit the audio kext before enabling its EFI entry.';s=s.replace(needle,'if [ "$MODE" = full ];then\n'+needle,1)
needle='mkdir -p /Library/KernelCollections';s=s.replace(needle,'fi\n'+needle,1)
s=s.replace('echo "Tahoe graphics installed/prelinked and ALC295 audio staged in EFI. Backup: $BACKUP"','echo "Tahoe update mode=$MODE installed and prelinked. Backup: $BACKUP"')
f.write_text(s);subprocess.run(['/bin/bash','-n',str(f)],check=True)
(p/'PERFORMANCE-UPDATE.json').write_text(json.dumps({'accelerator':'0.3.1','base':'verified installed0.3.0','upstreamFix':'https://github.com/watchthelight-labs/NvGfx/commit/7f9e464d7b5c9477c01dc8a9acc12f5eef748884','change':'Skip proven identical scanout swaps; use existing bounded front-copy helper for distinct legacy sources; expose counts and duration','commonLibraryFilesPinned':len(common),'commandPoolsStillAlwaysFresh':True,'liveUnloading':False,'onDellSpeedupMeasured':False,'otherKnownIssuesRemain':['CoreAnimation fence timeouts','steady-state GPU/publication latency','BLE pen discovery needs an actual scan']},indent=2)+'\n')
print('Prepared accelerator0.3.1 and a guarded kernel-only updater;',len(common),'companion files pinned.')
