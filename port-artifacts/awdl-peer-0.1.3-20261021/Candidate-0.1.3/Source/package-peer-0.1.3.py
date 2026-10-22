"""Stage the unbound peer-policy build; never select it in the active EFI."""
from pathlib import Path
import json,hashlib,shutil,subprocess,difflib,datetime
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA/iGPU-Port')
out=r/'port-artifacts/awdl-peer-0.1.3-20261021';assert not out.exists()
c=out/'Candidate-0.1.3';c.mkdir(parents=True)
b=r/'desktop-build/airport-awdl-peer-objects';validation=r/'desktop-build/native-awdl-abi'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def snapshot(root):return {str(p.relative_to(root)):sha(p) for p in sorted(root.rglob('*')) if p.is_file() and not p.name.startswith('._')}
def cp(a,b):b.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(a,b)
efi=u.parent/'EFI';before=snapshot(efi)
assert before and json.loads((b/'BUILD.json').read_text())['privateRevision']=='0.1.3-peer-policy-unbound'
shutil.copytree(b/'AirportItlwm.kext',c/'AirportItlwm.kext')
cp(b/'BUILD.json',c/'BUILD.json');cp(r/'wireless-sources/AWDL-PEER-STATUS-0.1.3.md',c/'README.md')
for name in ['LatitudeAWDLPeerTransport.h','LatitudeAWDLNativeABI.h','generate-native-awdl-abi.py','audit-native-vtables.py','test-awdl-peer.cpp','test-peer-notification.py','test-native-awdl-abi.py','test-awdl-radio.cpp','package-peer-0.1.3.py']:
 cp(r/'wireless-sources'/name,c/'Source'/name)
for name in ['VALIDATION.json','NOTIFICATION-VALIDATION.json','PEER-VALIDATION.json','clang-vtables.txt']:
 cp(validation/name,c/'Validation'/name)
for name in ['native-awdl-24H32-vtables.json','native-awdl-24H32-methods.json','ma-api83-command-versions.json','kdk-24H32.json']:
 cp(r/'research'/name,c/'Evidence'/name)
mods=['AirportItlwm/AirportItlwmV2.cpp','itlwm/hal_iwx/ItlIwx.cpp','itlwm/hal_iwx/ItlIwx.hpp'];patch=[]
for name in mods:
 old=r/'desktop-build/airport-awdl-radio'/name;new=r/'desktop-build/airport-awdl-peer'/name
 cp(new,c/'Source/Modified'/name)
 patch.extend(difflib.unified_diff(old.read_text().splitlines(True),new.read_text().splitlines(True),fromfile='a/'+name,tofile='b/'+name))
(c/'Source/0.1.2-to-0.1.3.patch').write_text(''.join(patch))
(c/'SHA256SUMS.json').write_text(json.dumps(snapshot(c),indent=2)+'\n')
subprocess.run(['codesign','--verify','--strict',str(c/'AirportItlwm.kext')],check=True)
destination=u/'Peripherals/Continuity/Candidate-0.1.3';assert not destination.exists()
backup=r/'backups'/('awdl-peer-before-0.1.3-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
backup.mkdir(parents=True);cp(u/'SHA256SUMS.json',backup/'SHA256SUMS.json')
collector=u/'Peripherals/Continuity/collect-continuity.sh';cp(collector,backup/'collect-continuity.sh')
(backup/'EFI-SHA256SUMS.json').write_text(json.dumps(before,indent=2)+'\n')
temp=destination.with_name('.staging-Candidate-0.1.3');assert not temp.exists()
shutil.copytree(c,temp)
assert snapshot(c)==snapshot(temp),'USB candidate verification failed'
temp.rename(destination)
cp(r/'wireless-sources/collect-continuity.sh',collector)
assert sha(collector)==sha(r/'wireless-sources/collect-continuity.sh')
manifest=json.loads((u/'SHA256SUMS.json').read_text())
for p in destination.rglob('*'):
 if p.is_file() and not p.name.startswith('._'):manifest[str(p.relative_to(u))]=sha(p)
manifest[str(collector.relative_to(u))]=sha(collector)
(u/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n')
assert before==snapshot(efi),'Active EFI unexpectedly changed'
selected=json.loads(subprocess.check_output(['plutil','-convert','json','-o','-',str(efi/'OC/Kexts/AirportItlwm.kext/Contents/Info.plist')]))['LatitudeAWDLPrivateRevision']
report={'staged':'0.1.3-peer-policy-unbound','selectedDriver':selected,'activeEFIByteIdentical':True,'collectorUpdated':True,'backup':str(backup),'AirDropFunctional':False,'nativePeerDataConnected':False,'offChannelFirmwareResourcesBound':False,'filesVerified':len(snapshot(c))}
(out/'PUBLISHED.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
