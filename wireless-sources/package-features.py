"""One-shot private USB packaging. Never switches the active EFI candidate."""
from pathlib import Path
import shutil,subprocess,hashlib,json,datetime,tempfile,difflib
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port'
stage=root/'port-artifacts/text-awdl-20261021';stage.mkdir(exist_ok=True)
assert not (stage/'PUBLISHED.json').exists(),'Already published; do not replay'
def files(p):return sorted(f for f in p.rglob('*') if f.is_file() and not f.name.startswith('._') and '__pycache__' not in f.parts)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inventory(p):return {str(f.relative_to(p)):sha(f) for f in files(p)}
def copy(src,dst):dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
def tree(src,dst):
 for f in files(src):copy(f,dst/f.relative_to(src))
def sums(p):return ''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files(p))
def run(args):subprocess.run([str(x) for x in args],check=True,capture_output=True)
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in manifest.items():assert sha(port/n)==h,n
before=inventory(usb/'EFI')
candidate=stage/'Peripherals/Continuity/Candidate-0.1.0';candidate.mkdir(parents=True,exist_ok=True)
tree(root/'desktop-build/airport-awdl-objects/AirportItlwm.kext',candidate/'AirportItlwm.kext')
tree(usb/'EFI/OC/Kexts/AirportItlwm.kext',candidate/'KnownWorking/AirportItlwm.kext')
for name in ['BUILD.json','COMPILE.json']:copy(root/'desktop-build/airport-awdl-objects'/name,candidate/name)
copy(root/'wireless-sources/RAW-CANDIDATE.md',candidate/'README.md');copy(root/'wireless-sources/trial-awdl.sh',candidate/'trial-awdl.sh')
(candidate/'CANDIDATE-BUNDLE-SHA256SUMS.txt').write_text(sums(candidate/'AirportItlwm.kext'))
(candidate/'WORKING-BUNDLE-SHA256SUMS.txt').write_text(sums(candidate/'KnownWorking/AirportItlwm.kext'))
for f in (root/'wireless-sources').iterdir():
 if f.is_file() and f.name not in ['package-features.py']:copy(f,candidate/'Source'/f.name)
tree(root/'wireless-sources/upstream-patches',candidate/'Source/upstream-patches')
tree(root/'desktop-build/airport-patch-tools',candidate/'Source/patch-tools')
for name in ['AirportItlwm/AirportItlwmV2.cpp','AirportItlwm/AirportItlwmV2.hpp','itlwm/hal_iwx/ItlIwx.cpp','itlwm/hal_iwx/ItlIwx.hpp','include/LatitudeAWDLFrame.h']:
 copy(root/'desktop-build/airport-awdl'/name,candidate/'Source/Modified'/name)
patch=[]
for name in ['AirportItlwm/AirportItlwmV2.cpp','AirportItlwm/AirportItlwmV2.hpp','itlwm/hal_iwx/ItlIwx.cpp','itlwm/hal_iwx/ItlIwx.hpp']:
 a=(root/'research/itlwm-runtime'/name).read_text().splitlines(keepends=True);b=(root/'desktop-build/airport-awdl'/name).read_text().splitlines(keepends=True)
 patch.extend(difflib.unified_diff(a,b,fromfile='a/'+name,tofile='b/'+name))
(candidate/'Source/pinned-base-to-private.patch').write_text(''.join(patch))
for name in ['LICENSE','LICENSE.md','COPYING']:
 p=root/'research/itlwm-runtime'/name
 if p.exists():copy(p,candidate/'Source'/name)
(candidate/'IDENTITY.json').write_text(json.dumps({'candidateUUID':subprocess.check_output(['xcrun','dwarfdump','--uuid',str(candidate/'AirportItlwm.kext/Contents/MacOS/AirportItlwm')],text=True).strip(),'workingBinarySHA256':sha(candidate/'KnownWorking/AirportItlwm.kext/Contents/MacOS/AirportItlwm'),'activeEFIReplaced':False,'rawGateEnabled':False,'graphicsChanged':False},indent=2)+'\n')
(candidate/'SHA256SUMS.txt').write_text(sums(candidate))
text=stage/'Desktop/TextRendering';text.mkdir(parents=True,exist_ok=True)
for name in ['libLatitudeLCD-x86_64.a','libLatitudeLCDCoreText-x86_64.dylib','libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64','test-appkit-x86_64','NATIVE-VALIDATION.json','VALIDATION.json']:copy(root/'desktop-build/latitude-lcd'/name,text/name)
for f in (root/'subpixel-sources').iterdir():
 if f.is_file():copy(f,text/'Source'/f.name)
for name in ['NATIVE-RUNTIME.md','INTEGRATION-STATUS.md','validate-native-text.sh']:copy(root/'subpixel-sources'/name,text/name)
for name in ['FTL.TXT','GPLv2.TXT','LICENSE.TXT','SOURCE.json']:copy(root/'desktop-build/freetype-static'/name,text/'FreeType'/name)
(text/'NATIVE-SHA256SUMS.txt').write_text(''.join(sha(text/name)+'  '+name+'\n' for name in ['libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64','test-appkit-x86_64']))
status=json.loads((port/'UNIFIED-STATUS.json').read_text());status.update({'featurePackage':'text-awdl-20261021','AWDLRawActionCandidate':'0.1.0 complete kext compiled/linked/signed, staged only','AWDLRadioPathConnected':False,'AWDLNativeActionSubmissionCode':True,'AWDLHardwareValidated':False,'FreeTypeNativeAppKit':'startup-loaded glyph route and San Francisco variable fonts tested','FreeTypeTransparentCPUImages':'deferred coverage through CGImage and opaque composition tested','FreeTypeLateActivation':'unsupported, explicitly returns zero','FreeTypeSystemAppsIntegrated':False,'activeEFIChangedByFeaturePackage':False})
(stage/'UNIFIED-STATUS.json').write_text(json.dumps(status,indent=2)+'\n');copy(root/'UNIFIED-CANDIDATE-WORK.md',stage/'UNIFIED-CANDIDATE-WORK.md')
(stage/'NEW-FEATURE-CANDIDATE.md').write_text('''# New text + AWDL package, 21 October 2026

All existing tablet, graphics, theme, chime and Copilot settings are retained.
The active EFI Wi-Fi driver and graphics binaries were not replaced by packaging.

Text rendering: ordinary CoreText and AppKit glyph routes, native San Francisco
variable fonts, deferred transparent CPU text-image composition, clipping and
ownership pass host sanitizer/release tests. Intel binaries are ready for a Dell
fixture run. This does not deploy FreeType into all existing Apple apps or
GPU-backed CoreAnimation surfaces. Late dyld activation is explicitly unsupported.

Run on the Dell (no sudo or graphics reinstall needed):

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Desktop/TextRendering/validate-native-text.sh

AWDL: Peripherals/Continuity/Candidate-0.1.0 contains the complete compiled kext,
build identity, private changes, bounded parser/ownership tests and exact working
Wi-Fi bundle. No full AirDrop claim and no hardware transmission proof exist.
The raw current-channel action gate remains off. First qualify ordinary Wi-Fi
compatibility before enabling any future radio transmission experiment.

To select the staged AWDL build for a USB boot (does not arm transmission):

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/Candidate-0.1.0/trial-awdl.sh apply

To restore the saved working Wi-Fi bundle:

    /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/Candidate-0.1.0/trial-awdl.sh restore

These operate on this USB only, verify exact source/target hashes, save the
previous bundle and preserve configuration/boot arguments. Reboot through the
USB after selection. They never unload a live driver.

Read-only radio collection on the Dell:

    sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/collect-continuity.sh

The ordinary desktop collector includes ISH state and native text fixture logs.
Do not run install-windowserver.sh expecting it to activate this AWDL/text work;
the existing working graphics installer is preserved.
''')
# Test the actual guarded bundle swap and restore in an isolated USB fixture.
run(['/bin/bash','-n',candidate/'trial-awdl.sh']);run(['codesign','--verify','--strict',candidate/'AirportItlwm.kext'])
with tempfile.TemporaryDirectory(prefix='awdl-package-fixture-') as temp:
 fixture=Path(temp);dest=fixture/'iGPU-Port/Peripherals/Continuity/Candidate-0.1.0';tree(candidate,dest)
 tree(candidate/'KnownWorking/AirportItlwm.kext',fixture/'EFI/OC/Kexts/AirportItlwm.kext');copy(usb/'EFI/OC/config.plist',fixture/'EFI/OC/config.plist')
 config=sha(fixture/'EFI/OC/config.plist');working=inventory(fixture/'EFI/OC/Kexts/AirportItlwm.kext')
 for action in ['apply','apply','restore','restore']:run(['/bin/bash',dest/'trial-awdl.sh',action])
 assert working==inventory(fixture/'EFI/OC/Kexts/AirportItlwm.kext') and config==sha(fixture/'EFI/OC/config.plist')
 # An unexpected active binary must be refused without changing any files.
 binary=fixture/'EFI/OC/Kexts/AirportItlwm.kext/Contents/MacOS/AirportItlwm';binary.write_bytes(b'changed fixture')
 r=subprocess.run(['/bin/bash',str(dest/'trial-awdl.sh'),'apply'],capture_output=True);assert r.returncode==2 and binary.read_bytes()==b'changed fixture'
run(['codesign','--verify','--strict',text/'libLatitudeLCDNative-x86_64.dylib']);run(['/bin/bash','-n',text/'validate-native-text.sh'])
backup=root/'backups'/('text-awdl-package-before-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
shutil.copy2(port/'SHA256SUMS.json',backup/'SHA256SUMS.json');writes={port/f.relative_to(stage):f for f in files(stage)};journal={}
for dst in writes:
 saved=backup/'journal'/dst.relative_to(port)
 if dst.exists():copy(dst,saved);journal[dst]=saved
 else:journal[dst]=None
def atomic(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.feature-tmp');shutil.copy2(src,tmp);tmp.replace(dst)
try:
 for dst,src in writes.items():atomic(src,dst)
 for dst in writes:manifest[str(dst.relative_to(port))]=sha(dst)
 out=stage/'SHA256SUMS.json';out.write_text(json.dumps(manifest,indent=2)+'\n');atomic(out,port/'SHA256SUMS.json')
 for n,h in manifest.items():assert sha(port/n)==h,n
 assert before==inventory(usb/'EFI'),'Active EFI unexpectedly changed'
 run(['codesign','--verify','--strict',port/'Peripherals/Continuity/Candidate-0.1.0/AirportItlwm.kext'])
 result={'backup':str(backup),'filesPackaged':len(writes),'activeEFIUnchanged':True,'validation':'package hashes, signatures, actual apply/restore/idempotence/unexpected-target fixtures passed','AWDLHardwareValidated':False,'systemAppFreeTypeDeployed':False}
 (stage/'PUBLISHED.json').write_text(json.dumps(result,indent=2)+'\n');print('Packaged and verified. Backup:',backup)
except BaseException:
 for dst,saved in reversed(list(journal.items())):
  if saved:atomic(saved,dst)
  else:dst.unlink(missing_ok=True)
 atomic(backup/'SHA256SUMS.json',port/'SHA256SUMS.json');raise
