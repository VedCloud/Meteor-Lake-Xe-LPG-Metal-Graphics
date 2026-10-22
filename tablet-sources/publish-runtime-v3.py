from pathlib import Path
import sys,shutil,hashlib,json,plistlib,subprocess,datetime
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port';per=port/'Peripherals';stage=root/'port-artifacts/unified-runtime-20261021'
assert not (stage/'PUBLISHED.json').exists(),'One-shot publisher already used'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def files(p):return [f for f in p.rglob('*') if f.is_file() and not f.name.startswith('._') and '__pycache__' not in f.parts and '.git' not in f.parts]
def inv(p):return {str(f.relative_to(p)):sha(f) for f in files(p)}
def run(args):subprocess.run([str(a) for a in args],check=True,capture_output=True)
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in manifest.items():assert sha(port/n)==h,n
assert (usb/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes()
before=inv(usb/'EFI');writes={}
def put(src,dst):assert src.is_file(),src;writes[dst]=src
def tree(src,dst):
 for f in files(src):put(f,dst/f.relative_to(src))
sys.path.insert(0,str(root/'graphics-mode-sources'));from prepare import create_script
oldcfg={p:sha(per/p/'config.plist') for p in ['Base','Touchpad']};olddsdt={p:sha(per/p/'DSDT.aml') for p in ['Base','Touchpad']};trial=(per/'touchpad-trial.sh').read_text()
for p in ['Base','Touchpad']:
 for old,new in [(oldcfg[p],sha(stage/(p.lower()+'-accelerated.plist'))),(olddsdt[p],sha(stage/p/'DSDT.aml'))]:assert old in trial;trial=trial.replace(old,new)
 put(stage/p/'DSDT.aml',per/p/'DSDT.aml')
 put(stage/p/'DSDT-original.aml',usb/'EFI/OC/ACPI'/('DSDT-'+p+'-Recovery.aml'))
 put(stage/(p.lower()+'-accelerated.plist'),per/p/'config.plist')
 for m in ['accelerated','recovery']:put(stage/(p.lower()+'-'+m+'.plist'),port/'GraphicsModes'/(p.lower()+'-'+m+'.plist'))
(stage/'touchpad-trial.sh').write_text(trial);(stage/'switch-graphics-mode.sh').write_text(create_script(stage,['base','touchpad']))
put(stage/'touchpad-trial.sh',per/'touchpad-trial.sh');put(stage/'switch-graphics-mode.sh',port/'switch-graphics-mode.sh');put(stage/'base-accelerated.plist',per/'config.plist')
put(stage/'Base/DSDT.aml',usb/'EFI/OC/ACPI/DSDT.aml')
put(root/'desktop-build/OCEFIAudio_VoiceOver_Boot-48k.mp3',usb/'EFI/OC/Resources/Audio/OCEFIAudio_VoiceOver_Boot.mp3')
tree(root/'desktop-build/tablet-state/LatitudeTabletState.kext',usb/'EFI/OC/Kexts/LatitudeTabletState.kext')
for typ in ['Keyboard','Mouse']:
 name='VoodooPS2'+typ+'.kext';target=usb/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns'/name
 # Save original plugin files as immutable sources before replacing any target.
 original=stage/'InputRecovery'/name;shutil.copytree(target,original,ignore=shutil.ignore_patterns('._*'))
 tree(original,per/'Tablet/InputRecovery'/name)
 tree(root/'desktop-build/tablet-input-hooks'/name,target)
 tree(root/'desktop-build/tablet-input-hooks'/name,per/'Tablet/InputCandidates'/name)
for f in (root/'tablet-sources').iterdir():
 if f.is_file() and not f.name.startswith(('publish-','._')):put(f,per/'Tablet'/f.name)
put(root/'desktop-build/tablet-state/BUILD.json',per/'Tablet/Candidate/STATE-BUILD.json');put(root/'desktop-build/tablet-input-hooks/STATUS.json',per/'Tablet/InputCandidates/BUILD.json')
put(stage/'ACPI-VALIDATION.json',per/'Tablet/Candidate/ACPI-VALIDATION.json')
# Latest private libraries are packaged; no blanket injection into applications.
for f in (root/'subpixel-sources').iterdir():
 if f.is_file():put(f,port/'Desktop/TextRendering/Source'/f.name)
for name in ['libLatitudeLCD-x86_64.a','libLatitudeLCDCoreText-x86_64.dylib','VALIDATION.json']:put(root/'desktop-build/latitude-lcd'/name,port/'Desktop/TextRendering'/name)
for f in (root/'wireless-sources').iterdir():
 if f.is_file():put(f,per/'Continuity'/f.name)
engine=per/'Continuity/Engine';put(root/'desktop-build/awdl-engine/x86_64/libLatitudeAWDLEngine.a',engine/'libLatitudeAWDLEngine.a');put(root/'desktop-build/awdl-engine/PROVENANCE.json',engine/'PROVENANCE.json')
tree(root/'research/owl/src',engine/'Source/src')
for f in (root/'research/owl/radiotap').iterdir():
 if f.is_file() and not f.name.startswith('.'):put(f,engine/'Source/radiotap'/f.name)
put(root/'research/owl/COPYING',engine/'COPYING');put(root/'research/owl/README.md',engine/'UPSTREAM-README.md')
# An explicit device-only rollback retains the new ACPI/graphics configuration.
restore='''#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../../.." && pwd)"
TARGET="$USB/EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
'''
for typ in ['Keyboard','Mouse']:
 name='VoodooPS2'+typ+'.kext';src=root/'desktop-build/tablet-input-hooks'/name
 for file in ['Contents/Info.plist','Contents/MacOS/VoodooPS2'+typ]:restore+='[ "$(hash "$TARGET/'+name+'/'+file+'")" = "'+sha(src/file)+'" ] || { echo "Input candidate changed; refusing overwrite." >&2; exit 2; }\n'
restore+='BACKUP=$(mktemp -d "$HERE/InputRecovery/applied-rollback.XXXXXX")\n'
for typ in ['Keyboard','Mouse']:
 name='VoodooPS2'+typ+'.kext'
 restore+='''/usr/bin/codesign --verify --strict "$HERE/InputRecovery/'''+name+'''"
/bin/cp -R "$TARGET/'''+name+'''" "$BACKUP/"
/bin/cp -R "$HERE/InputRecovery/'''+name+'''" "$TARGET/'''+name+'''.restore.kext"
/bin/rm -rf "$TARGET/'''+name+'''"
/bin/mv "$TARGET/'''+name+'''.restore.kext" "$TARGET/'''+name+'''"
'''
restore+='echo "Original PS2 keyboard/mouse restored. Reboot through this USB. Graphics configuration retained."\n';(stage/'restore-tablet-input.sh').write_text(restore);put(stage/'restore-tablet-input.sh',per/'Tablet/restore-tablet-input.sh')
status={'candidate':'unified-runtime-20261021-v3','effectiveHIDDVisibilityFix':'direct scoped rename in supplied DSDT; original recovery tables retained','tabletReader':'0.2.1','conditionalPS2SuppressionArmed':True,'inputUnknownOrStale':'enabled','DellRuntimeValidated':False,'automaticRotationEnabled':False,'ISHPCITransportImplemented':False,'FreeTypeSystemAppsIntegrated':False,'FreeTypeComponentAlphaLayerTests':'passed','AWDLRadioPathConnected':False,'AWDLEngineOfflineTests':'passed','chime':'48 kHz MP3, speaker mask 1; target playback pending','graphicsBinariesChanged':False,'CopilotMacrosPreserved':True}
(stage/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n');put(stage/'STATUS.json',port/'UNIFIED-STATUS.json');put(stage/'STATUS.json',per/'Tablet/Candidate/STATUS.json')
put(root/'UNIFIED-CANDIDATE-WORK.md',port/'UNIFIED-CANDIDATE-WORK.md')
active=usb/'EFI/OC/config.plist';put(stage/'base-accelerated.plist',active)
backup=root/'backups'/('runtime-v3-before-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
shutil.copytree(usb/'EFI',backup/'EFI',ignore=shutil.ignore_patterns('._*'));shutil.copytree(per,backup/'Peripherals',ignore=shutil.ignore_patterns('._*'))
for n in ['SHA256SUMS.json','switch-graphics-mode.sh']:shutil.copyfile(port/n,backup/n)
shutil.copytree(port/'GraphicsModes',backup/'GraphicsModes',ignore=shutil.ignore_patterns('._*'))
journal={}
for dst in list(writes)+[per/'TRIAL-SHA256SUMS.txt',port/'SHA256SUMS.json']:
 saved=backup/'journal'/dst.relative_to(usb)
 if dst.exists():saved.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(dst,saved);journal[dst]=saved
 else:journal[dst]=None
def atomic(src,dst):dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.runtime-tmp');shutil.copyfile(src,tmp);tmp.replace(dst)
try:
 for dst,src in writes.items():
  if dst!=active:atomic(src,dst)
 lines=[]
 for line in (per/'TRIAL-SHA256SUMS.txt').read_text().splitlines():_,n=line.split('  ',1);lines.append(sha(per/n)+'  '+n)
 (stage/'TRIAL-SHA256SUMS.txt').write_text('\n'.join(lines)+'\n');atomic(stage/'TRIAL-SHA256SUMS.txt',per/'TRIAL-SHA256SUMS.txt')
 for dst in [*writes,per/'TRIAL-SHA256SUMS.txt']:
  if dst.is_relative_to(port):manifest[str(dst.relative_to(port))]=sha(dst)
 (stage/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n');atomic(stage/'SHA256SUMS.json',port/'SHA256SUMS.json')
 for n,h in manifest.items():assert sha(port/n)==h,n
 for typ in ['Keyboard','Mouse']:run(['codesign','--verify','--strict',usb/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns'/('VoodooPS2'+typ+'.kext')])
 run(['codesign','--verify','--strict',usb/'EFI/OC/Kexts/LatitudeTabletState.kext'])
 run([sys.executable,root/'copilot-sources/retain.py',usb/'EFI','--check'])
 for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 atomic(writes[active],active)
 after=inv(usb/'EFI');modified=[n for n,h in before.items() if after.get(n)!=h];added=sorted(set(after)-set(before));prefixes=('OC/Kexts/LatitudeTabletState.kext/','OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/','OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Mouse.kext/')
 assert all(n.startswith(prefixes) or n in ['OC/config.plist','OC/ACPI/DSDT.aml','OC/Resources/Audio/OCEFIAudio_VoiceOver_Boot.mp3'] for n in modified),modified
 assert all(n.startswith(prefixes) or n in ['OC/ACPI/DSDT-Base-Recovery.aml','OC/ACPI/DSDT-Touchpad-Recovery.aml'] for n in added),added
 validate=root/'research/OpenCore-release/matching/Utilities/ocvalidate/ocvalidate'
 for f in [active,*[x for x in (port/'GraphicsModes').glob('*.plist') if not x.name.startswith('._')]]:run([validate,f])
 run([sys.executable,root/'tablet-sources/validate-candidate.py'])
 result={'backup':str(backup),'changedEFI':modified,'addedEFI':added,'filesPackaged':len(writes),'validation':'profiles, fixture switching/trial restore, manifest, signatures, Copilot and frozen graphics passed; Dell boot pending'};(stage/'PUBLISHED.json').write_text(json.dumps(result,indent=2)+'\n');print('Published/validated runtime candidate. Backup:',backup)
except BaseException:
 for dst,saved in reversed(list(journal.items())):
  if saved:atomic(saved,dst)
  else:dst.unlink(missing_ok=True)
 print('Rolled back publication from per-file journal.',file=sys.stderr);raise
