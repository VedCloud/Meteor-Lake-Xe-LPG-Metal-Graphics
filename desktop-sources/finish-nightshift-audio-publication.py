from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';b=port/'backups/nightshift-audio-before-0.3.16';a=r/'port-artifacts/nightshift-audio-0.3.16'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inventory(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def copy(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.publish-tmp');shutil.copy2(src,tmp);tmp.replace(dst)
meta=json.loads((b/'BACKUP.json').read_text());efi=meta['efi_hashes'];assert inventory(u/'EFI')==efi,'EFI changed after backup'
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
v=json.loads((a/'VALIDATION.json').read_text())
# Add only the reviewed audio configuration and AppleALC bundle.
import plistlib
oldconfig=(u/'EFI/OC/config.plist').read_bytes();oc=plistlib.loads(oldconfig);nc=plistlib.loads((p/'Audio/config.plist').read_bytes())
entry=nc['Kernel']['Add'].pop();props=nc['DeviceProperties']['Add'].pop('PciRoot(0x0)/Pci(0x1F,0x3)')
assert entry['BundlePath']=='AppleALC.kext' and entry['Enabled'] and nc==oc
assert props=={'device-id':bytes.fromhex('ca510000'),'layout-id':(33).to_bytes(4,'little'),'alc-layout-id':(33).to_bytes(4,'little')}
old_alc_exists=(u/'EFI/OC/Kexts/AppleALC.kext').exists();old_alc_backup=b/'AppleALC-at-commit.kext'
assert not old_alc_backup.exists()
try:
 if old_alc_exists:(u/'EFI/OC/Kexts/AppleALC.kext').rename(old_alc_backup)
 shutil.copytree(p/'Audio/AppleALC.kext',u/'EFI/OC/Kexts/AppleALC.kext')
 copy(p/'Audio/config.plist',u/'EFI/OC/config.plist')
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 subprocess.run(['codesign','--verify','--strict',str(u/'EFI/OC/Kexts/AppleALC.kext')],check=True)
 after=inventory(u/'EFI');changed={n for n in set(efi)|set(after) if efi.get(n)!=after.get(n)}
 expected={'OC/Kexts/AppleALC.kext/'+n for n in inventory(p/'Audio/AppleALC.kext')}|{'OC/config.plist'}
 allowed=expected|{n for n in efi if n.startswith('OC/Kexts/AppleALC.kext/')}
 assert changed<=allowed and 'OC/config.plist' in changed,changed
 assert inventory(u/'EFI/OC/Kexts/AppleALC.kext')==inventory(p/'Audio/AppleALC.kext')
except BaseException:
 copy(b/'EFI/OC/config.plist',u/'EFI/OC/config.plist')
 if (u/'EFI/OC/Kexts/AppleALC.kext').exists():shutil.rmtree(u/'EFI/OC/Kexts/AppleALC.kext')
 if old_alc_exists and old_alc_backup.exists():old_alc_backup.rename(u/'EFI/OC/Kexts/AppleALC.kext')
 raise
v['efi_before_hashes']=efi;v['efi_after_hashes']=after
(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'nightshift-audio-0.3.16-validation.json')
m['nightshift-audio-0.3.16-validation.json']=sha(port/'nightshift-audio-0.3.16-validation.json');(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
print('Published0.3.16: scanout gamma/Night Shift candidate and ALC295 AppleALC configuration. Prior0.3.15 full EFI/replaced package files backed up locally and on USB. Manifests/baseline/signatures/config verified; only planned audio EFI differences. No target installation, arming, reboot or GPU suite.')
