"""Private one-shot unified publisher. Stages all files, backs up, commits config last."""
from pathlib import Path
import sys,hashlib,json,plistlib,shutil,subprocess,datetime,zipfile
root=Path(__file__).resolve().parent.parent
usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port';per=port/'Peripherals'
stage=root/'port-artifacts/unified-20261021'
backup=root/'backups'/('unified-before-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
assert not (stage/'PUBLISHED.json').exists(),'Already published; do not replay'
sys.path.insert(0,str(root/'graphics-mode-sources'));from prepare import create_script

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def files(p):return [f for f in p.rglob('*') if f.is_file() and not f.name.startswith('._') and '__pycache__' not in f.parts]
def inventory(p):return {str(f.relative_to(p)):sha(f) for f in files(p)}
def run(args):subprocess.run([str(x) for x in args],check=True,capture_output=True)
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in manifest.items():assert sha(port/n)==h,n
assert (usb/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes(),'Active config is not base'
for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
 h,n=line.split(None,1);assert sha(port/n.strip())==h,n
run([sys.executable,root/'copilot-sources/retain.py',usb/'EFI','--check'])
validate=root/'research/OpenCore-release/matching/Utilities/ocvalidate/ocvalidate'
for p in stage.glob('*.plist'):
 if p.name.startswith(('base-','touchpad-')):run([validate,p])
with zipfile.ZipFile(root/'research/OpenCore-release/OpenCore-matching-1.0.9-DEBUG.zip') as archive:
 assert sha(usb/'EFI/OC/OpenCore.efi')==hashlib.sha256(archive.read('X64/EFI/OC/OpenCore.efi')).hexdigest()
before=inventory(usb/'EFI');writes={}
def put(src,dst):
 assert src.is_file(),src
 writes[dst]=src

def tree(src,dst):
 assert src.is_dir(),src
 for f in files(src):put(f,dst/f.relative_to(src))
for name in ['AudioDxe.efi','OpenCanopy.efi']:
 put(root/'research/OpenCore-release/matching/X64/EFI/OC/Drivers'/name,usb/'EFI/OC/Drivers'/name)
tree(root/'research/GoldenGateExt/GoldenGateExt',usb/'EFI/OC/Resources/Image/Acidanthera/GoldenGateExt')
put(root/'research/OpenCore-release/OCEFIAudio_VoiceOver_Boot.mp3',usb/'EFI/OC/Resources/Audio/OCEFIAudio_VoiceOver_Boot.mp3')
put(root/'port-artifacts/tablet-mode-v2/SSDT-LAT-TABLET.aml',usb/'EFI/OC/ACPI/SSDT-LAT-TABLET.aml')
tree(root/'desktop-build/tablet-state/LatitudeTabletState.kext',usb/'EFI/OC/Kexts/LatitudeTabletState.kext')
old={p:sha(per/p.capitalize()/'config.plist') for p in ['base','touchpad']}
trial=(per/'touchpad-trial.sh').read_text()
for p,h in old.items():
 assert h in trial,p
 trial=trial.replace(h,sha(stage/(p+'-accelerated.plist')))
(stage/'touchpad-trial.sh').write_text(trial)
(stage/'switch-graphics-mode.sh').write_text(create_script(stage,['base','touchpad']))
put(stage/'touchpad-trial.sh',per/'touchpad-trial.sh')
put(stage/'switch-graphics-mode.sh',port/'switch-graphics-mode.sh')
for profile in ['base','touchpad']:
 for mode in ['accelerated','recovery']:put(stage/(profile+'-'+mode+'.plist'),port/'GraphicsModes'/(profile+'-'+mode+'.plist'))
 put(stage/(profile+'-accelerated.plist'),per/profile.capitalize()/'config.plist')
put(stage/'base-accelerated.plist',per/'config.plist')
for src in (root/'tablet-sources').iterdir():
 if src.is_file() and not src.name.startswith(('publish-','._')):put(src,per/'Tablet'/src.name)
for name in ['latitude-tablet-state','latitude-rotate']:put(root/'desktop-build'/name,per/'Tablet'/name)
tree(root/'desktop-build/Latitude Tablet Keyboard.app',per/'Tablet/Latitude Tablet Keyboard.app')
tree(root/'desktop-build/tablet-state/LatitudeTabletState.kext',per/'Tablet/Candidate/LatitudeTabletState.kext')
put(root/'desktop-build/tablet-state/BUILD.json',per/'Tablet/Candidate/STATE-BUILD.json')
put(root/'port-artifacts/tablet-mode-v2/SSDT-LAT-TABLET.aml',per/'Tablet/Candidate/SSDT-LAT-TABLET.aml')
put(stage/'HIDD-PATCH.plist',per/'Tablet/Candidate/HIDD-PATCH.plist')
put(root/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
# Source plus Intel libraries; host test executables are deliberately not installed.
text=port/'Desktop/TextRendering'
for src in (root/'subpixel-sources').iterdir():
 if src.is_file():put(src,text/'Source'/src.name)
for name in ['libLatitudeLCD-x86_64.a','libLatitudeLCDCoreText-x86_64.dylib','VALIDATION.json']:
 put(root/'desktop-build/latitude-lcd'/name,text/name)
for name in ['FTL.TXT','GPLv2.TXT','LICENSE.TXT','SOURCE.json']:
 put(root/'desktop-build/freetype-static'/name,text/'FreeType'/name)
tree(root/'desktop-build/freetype-static/include',text/'FreeType/include')
put(root/'desktop-build/freetype-static/x86_64/libfreetype.a',text/'FreeType/libfreetype.a')
tree(root/'wireless-sources',per/'Continuity')
for name in ['README.md','configure.py','prepare.py']:
 put(root/'boot-ui-sources'/name,port/'BootUI'/name)
put(stage/'BOOT-UI-PROVENANCE.json',port/'BootUI/PROVENANCE.json')
put(root/'UNIFIED-CANDIDATE-WORK.md',port/'UNIFIED-CANDIDATE-WORK.md')
status={'candidate':'unified-20261021','graphicsBinariesChanged':False,'copilotPreserved':True,
 'normalBoot':'accelerated','HIDD':'scoped visibility patch + live state 0.2.0; Dell boot pending',
 'inputSuppressionEnabled':False,'automaticRotationEnabled':False,'ISHRuntimeImplemented':False,
 'keyboard':'0.2.0 qualified mode and editable-field integration; target test and Accessibility required',
 'FreeType':'real filtered LCD and CoreText frontend passed; global app interception/compositor integration incomplete',
 'AWDL':'source audit and diagnostics; radio data plane not implemented',
 'theme':'GoldenGateExt','hideAuxiliary':True,'auxiliaryReveal':'Spacebar',
 'chime':'enabled with matching AudioDxe; physical output not yet tested','backup':str(backup)}
(stage/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n')
put(stage/'STATUS.json',per/'Tablet/Candidate/STATUS.json')
put(stage/'STATUS.json',port/'UNIFIED-STATUS.json')
# Commit active config only after all dependencies and package hashes exist.
active=usb/'EFI/OC/config.plist'
put(stage/'base-accelerated.plist',active)
backup.mkdir(parents=True)
shutil.copytree(usb/'EFI',backup/'EFI',ignore=shutil.ignore_patterns('._*'))
shutil.copytree(per,backup/'Peripherals',ignore=shutil.ignore_patterns('._*'))
shutil.copytree(port/'GraphicsModes',backup/'GraphicsModes',ignore=shutil.ignore_patterns('._*'))
for name in ['SHA256SUMS.json','collect-desktop-failure.sh','switch-graphics-mode.sh']:shutil.copy2(port/name,backup/name)
# A per-file rollback journal also covers new Desktop/BootUI paths.
journal={}
for dst in list(writes)+[per/'TRIAL-SHA256SUMS.txt',port/'SHA256SUMS.json']:
 rel=dst.relative_to(usb);saved=backup/'journal'/rel
 if dst.exists():saved.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(dst,saved);journal[dst]=saved
 else:journal[dst]=None

def atomic(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.unified-tmp');shutil.copyfile(src,tmp);tmp.replace(dst)
try:
 for dst,src in writes.items():
  if dst!=active:atomic(src,dst)
 lines=[]
 for line in (per/'TRIAL-SHA256SUMS.txt').read_text().splitlines():
  _,n=line.split('  ',1);lines.append(sha(per/n)+'  '+n)
 (stage/'TRIAL-SHA256SUMS.txt').write_text('\n'.join(lines)+'\n');atomic(stage/'TRIAL-SHA256SUMS.txt',per/'TRIAL-SHA256SUMS.txt')
 for dst in list(writes)+[per/'TRIAL-SHA256SUMS.txt']:
  if dst.is_relative_to(port):manifest[str(dst.relative_to(port))]=sha(dst)
 (stage/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n');atomic(stage/'SHA256SUMS.json',port/'SHA256SUMS.json')
 for n,h in manifest.items():assert sha(port/n)==h,n
 for dst in [per/'Tablet/latitude-tablet-state',per/'Tablet/latitude-rotate',per/'Tablet/Latitude Tablet Keyboard.app',usb/'EFI/OC/Kexts/LatitudeTabletState.kext',text/'libLatitudeLCDCoreText-x86_64.dylib']:
  run(['codesign','--verify','--strict',dst])
 for dst in [port/'collect-desktop-failure.sh',port/'switch-graphics-mode.sh',per/'touchpad-trial.sh',*[p for p in (per/'Tablet').glob('*.sh') if not p.name.startswith('._')],per/'Continuity/collect-continuity.sh']:run(['/bin/bash','-n',dst])
 atomic(writes[active],active)
 after=inventory(usb/'EFI');modified=[n for n,h in before.items() if after.get(n)!=h];added=sorted(set(after)-set(before))
 allowed={'OC/config.plist','OC/Drivers/AudioDxe.efi','OC/Drivers/OpenCanopy.efi','OC/Resources/Audio/OCEFIAudio_VoiceOver_Boot.mp3'}
 prefixes=('OC/Kexts/LatitudeTabletState.kext/','OC/Resources/Image/Acidanthera/GoldenGateExt/')
 assert all(n in allowed or n.startswith(prefixes) for n in modified),modified
 assert all(n in allowed or n=='OC/ACPI/SSDT-LAT-TABLET.aml' or n.startswith(prefixes) for n in added),added
 run([sys.executable,root/'copilot-sources/retain.py',usb/'EFI','--check'])
 for dst in [active,*[p for p in (port/'GraphicsModes').glob('*.plist') if not p.name.startswith('._')]]:run([validate,dst])
 for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 result={'backup':str(backup),'modifiedEFI':modified,'addedEFI':added,'packageFiles':len(writes),'verified':True}
 (stage/'PUBLISHED.json').write_text(json.dumps(result,indent=2)+'\n')
 print('Published unified candidate, package signatures/hashes/configs verified. Backup:',backup)
except BaseException:
 for dst,saved in reversed(list(journal.items())):
  if saved is not None:atomic(saved,dst)
  else:dst.unlink(missing_ok=True)
 print('Publication rolled back from per-file journal.',file=sys.stderr);raise
