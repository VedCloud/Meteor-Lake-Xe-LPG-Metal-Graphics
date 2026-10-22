"""One-shot, backed-up USB publication; no target OS installation."""
from pathlib import Path
import json,hashlib,shutil,subprocess,tempfile
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';p=r/'latitude-desktop';v=r/'port-artifacts/peripherals-20261021'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inv(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.peripheral-tmp');shutil.copy2(a,tmp);tmp.replace(b)
validation=json.loads((v/'VALIDATION.json').read_text());m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
assert sha(u/'EFI/OC/config.plist')==validation['base_config_sha256']
assert sha(u/'EFI/OC/ACPI/DSDT.aml')==validation['base_dsdt_sha256']
before=inv(u/'EFI');b=r/'backups/peripherals-before-20261021';assert not b.exists()
b.mkdir();shutil.copytree(u/'EFI',b/'EFI',ignore=shutil.ignore_patterns('._*'))
shutil.copytree(p,b/'local-Desktop',ignore=shutil.ignore_patterns('._*'))
cp(port/'SHA256SUMS.json',b/'SHA256SUMS.json');cp(port/'collect-desktop-failure.sh',b/'collect-desktop-failure.sh')
# USB backup is an EFI/package rollback, not an installed OS snapshot.
shutil.copytree(b/'EFI',port/'backups/peripherals-before-20261021/EFI')
cp(b/'SHA256SUMS.json',port/'backups/peripherals-before-20261021/SHA256SUMS.json')
cp(b/'collect-desktop-failure.sh',port/'backups/peripherals-before-20261021/collect-desktop-failure.sh')
doc='''# Dell peripheral candidate — 21 October 2026

Bluetooth: exact Dell USB 8087:0033 match. IntelBluetoothFirmware and IntelBTPatcher 2.5.1 from lshbluesky release; BlueToolFixup 2.7.2 official release. Download SHA256 values match GitHub release asset digests. The reference README advertises 2.7.3, but the official available release is 2.7.2. Existing Lilu1.7.3 and USB map retained; physical port10 already internal255. Set the two Bluetooth NVRAM initial values via OpenCore Add/Delete. No host NVRAM writes. No IntelBluetoothInjector, Broadcom firmware, address-bypass debug option or Tahoe beta flag.

Brightness keys: LatitudeHotkeys0.1.0 handles only Dell WMI brightness events from the matching GUID on AMW0/D0. BRTKEYS revision5 supplies an ownership guard to prevent duplicate native routing. Refuses an already-owned WMI interface, releases ownership/enable state on stop/failure. Hardware event source remains unconfirmed: the last report had Q66=7, SMEE=0, BRT6=0. Host mocked-driver/AML tests pass; this is not a Dell hardware pass.

I2C touchpad is a SEPARATE opt-in trial. Dell ELAN04F3:328E and Meteor Lake8086:7e7b differ from the reference ELAN0A05/Raptor Lake51e8. Current Dell boot logs contain TPD0._DSM missing HIDG. Patch only the current DSDT's TPD0 _DSM/_CRS bodies using firmware TPDH/SBFB/SBFI, preserving all other methods and package lengths. Reuse the Dell-scoped controller subclass with verified D0 and Linux timings; polling only PCI15.3. Disable PS2Mouse/PS2Trackpad plugins while keeping PS2Keyboard and exactly one VoodooInput. Earlier ELAN attachment succeeded, but erratic motion was not diagnosed; competing pointer suppression is a testable hypothesis, not a proven fix. A USB mouse is recommended for this trial.

First reboot the Dell through this USB and check Bluetooth/brightness keys. No graphics installer or rendering suite is needed for these EFI additions. Then collect with the normal collect-desktop-failure.sh: it now captures Bluetooth services, USB enumeration, Intel firmware status, Dell WMI counters and I2C3/TPD0 registry state. Test F6/F7 with/without Fn before collection. Pair a Bluetooth device manually to establish real functionality.

To try ELAN gestures after the first check:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
Then reboot via USB. To restore PS2 pointer configuration:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh restore
Both operations refuse changed configs/DSDTs and preserve the new Bluetooth/brightness configuration. Scripts only modify the USB; no target system installs. Full pre-update EFI backup: iGPU-Port/backups/peripherals-before-20261021/EFI.

Sources:
- https://github.com/ductruongvu23/hackintosh-gigabyte-a16
- https://github.com/lshbluesky/IntelBluetoothFirmware/releases/tag/v2.5.1
- https://github.com/acidanthera/BrcmPatchRAM/releases/tag/2.7.2
- https://github.com/acidanthera/BrcmPatchRAM#bluetoolfixupkext
- https://github.com/torvalds/linux/blob/master/drivers/platform/x86/dell/dell-wmi-base.c
- https://github.com/VoodooI2C/VoodooI2C

Hardware support is pending the next Dell boot. No GPU binaries, Wi-Fi, audio, branding, security policy, XOSI or host boot configuration changed.
'''
(v/'README.md').write_text(doc)
# Create a hash-guarded, two-way optional trial using full prepared current configs.
script='''#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
USB="$(cd "$HERE/../.." && pwd)"
OC="$USB/EFI/OC"
hash() { /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'; }
case "${1:-}" in
 apply) from=Base; to=Touchpad; expect_cfg=BASECFG; expect_dsdt=BASEDSDT;;
 restore) from=Touchpad; to=Base; expect_cfg=TRIALCFG; expect_dsdt=TRIALDSDT;;
 *) echo 'Usage: /bin/bash touchpad-trial.sh apply|restore'; exit 2;;
esac
[ "$(hash "$OC/config.plist")" = "$expect_cfg" ] || { echo 'Config changed; refusing to overwrite it.' >&2; exit 2; }
[ "$(hash "$OC/ACPI/DSDT.aml")" = "$expect_dsdt" ] || { echo 'DSDT changed; refusing to overwrite it.' >&2; exit 2; }
check_tree() { ( cd "$1"; /usr/bin/shasum -a 256 -c "$2" >/dev/null ); }
check_tree "$HERE" "$HERE/TRIAL-SHA256SUMS.txt"
rollback() {
 /bin/cp "$HERE/$from/config.plist" "$OC/config.plist"
 /bin/cp "$HERE/$from/DSDT.aml" "$OC/ACPI/DSDT.aml"
 if [ "$from" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi
}
trap 'rollback; echo "Trial operation failed; previous config restored." >&2' ERR
if [ "$to" = Touchpad ]; then
 [ ! -e "$OC/Kexts/LatitudeI2CMeteorLake.kext" ] || { echo 'Unexpected existing controller bundle; refusing.' >&2; exit 2; }
 /bin/cp -R "$HERE/Touchpad/LatitudeI2CMeteorLake.kext" "$OC/Kexts/"
fi
/bin/cp "$HERE/$to/DSDT.aml" "$OC/ACPI/DSDT.aml"
/bin/cp "$HERE/$to/config.plist" "$OC/config.plist"
if [ "$to" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi
trap - ERR
/bin/sync
echo "USB $to configuration selected. Reboot through this USB."
'''
for key,val in [('BASECFG',validation['default_config_sha256']),('BASEDSDT',validation['base_dsdt_sha256']),('TRIALCFG',validation['trial_config_sha256']),('TRIALDSDT',validation['trial_dsdt_sha256'])]:script=script.replace(key,val)
(v/'touchpad-trial.sh').write_text(script)
(v/'Base').mkdir(exist_ok=True);cp(v/'config.plist',v/'Base/config.plist');cp(u/'EFI/OC/ACPI/DSDT.aml',v/'Base/DSDT.aml')
trialfiles=sorted(f for n in ['Base','Touchpad'] for f in (v/n).rglob('*') if f.is_file() and f.suffix not in ['.dsl'])
(v/'TRIAL-SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(v))+'\n' for f in trialfiles))
subprocess.run(['/bin/bash','-n',str(v/'touchpad-trial.sh')],check=True)
# Exercise the actual shipped switcher in a disposable USB layout.
with tempfile.TemporaryDirectory(prefix='latitude-peripheral-trial-') as tmp:
 mock=Path(tmp);dest=mock/'iGPU-Port/Peripherals';shutil.copytree(v,dest)
 oc=mock/'EFI/OC';(oc/'ACPI').mkdir(parents=True);(oc/'Kexts').mkdir()
 cp(v/'Base/config.plist',oc/'config.plist');cp(v/'Base/DSDT.aml',oc/'ACPI/DSDT.aml')
 for mode in ['apply','restore']:
  subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),mode],check=True,capture_output=True)
 assert sha(oc/'config.plist')==validation['default_config_sha256']
 assert sha(oc/'ACPI/DSDT.aml')==validation['base_dsdt_sha256']
 assert not (oc/'Kexts/LatitudeI2CMeteorLake.kext').exists()
 (oc/'config.plist').write_bytes(b'changed-config')
 assert subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),'apply'],capture_output=True).returncode==2
 assert (oc/'config.plist').read_bytes()==b'changed-config'
 print('PASS actual optional trial: apply/restore, retained base config/DSDT and changed-config refusal in disposable USB fixture.')
changed=[]
def put(src,relative):
 target=port/relative
 if target.exists() and target.is_file():cp(target,b/'port'/relative)
 else:changed.append(relative)
 cp(src,target)
try:
 for f in sorted(v.rglob('*')):
  if f.is_file() and not f.name.startswith('._'):put(f,'Peripherals/'+str(f.relative_to(v)))
 for f in (r/'hotkey-sources').glob('*'):
  if f.is_file():put(f,'Peripherals/Source/Hotkeys/'+f.name)
 for name in ['SSDT-LAT-BRT.dsl','collect-desktop-failure.sh','prepare-peripherals.py','publish-peripherals.py']:
  put(r/'desktop-sources'/name,'Peripherals/Source/'+name)
 put(r/'desktop-sources/collect-desktop-failure.sh','collect-desktop-failure.sh')
 cp(v/'SSDT-LAT-BRT.aml',p/'BrightnessKeys/SSDT-LAT-BRT.aml');cp(r/'desktop-sources/SSDT-LAT-BRT.dsl',p/'BrightnessKeys/SSDT-LAT-BRT.dsl');cp(v/'config.plist',p/'BrightnessKeys/config.plist')
 (p/'README-peripherals.md').write_text(doc)
 manifest=json.loads((p/'BUILD-MANIFEST.json').read_text());manifest['peripherals_20261021']=validation;(p/'BUILD-MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
 files=sorted(f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
 (p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
 for f in files+[p/'SHA256SUMS.txt']:
  relative='Desktop/'+str(f.relative_to(p))
  if not (port/relative).exists() or sha(f)!=sha(port/relative):put(f,relative)
 # Payloads first, configuration last.
 for f in (v/'Kexts').rglob('*'):
  if f.is_file():cp(f,u/'EFI/OC/Kexts'/f.relative_to(v/'Kexts'))
 cp(v/'SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml');cp(v/'config.plist',u/'EFI/OC/config.plist')
 after=inv(u/'EFI');allowed={'OC/config.plist','OC/ACPI/SSDT-LAT-BRT.aml'}
 for n,h in before.items():assert n in allowed or after.get(n)==h,n
 new=set(after)-set(before)
 assert all(any(n.startswith('OC/Kexts/'+name+'.kext/') for name in ['LatitudeHotkeys','IntelBluetoothFirmware','IntelBTPatcher','BlueToolFixup']) for n in new)
 assert sha(u/'EFI/OC/ACPI/DSDT.aml')==validation['base_dsdt_sha256']
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 for folder in ['Peripherals','Desktop']:
  for f in (port/folder).rglob('*'):
   if f.is_file() and not f.name.startswith('._') and (folder!='Desktop' or str(f.relative_to(port)) in m or f.name=='README-peripherals.md'):m[str(f.relative_to(port))]=sha(f)
 m['collect-desktop-failure.sh']=sha(port/'collect-desktop-failure.sh')
 for n,h in m.items():assert sha(port/n)==h,n
 cp(v/'README.md',port/'PERIPHERALS-STATUS.md');m['PERIPHERALS-STATUS.md']=sha(port/'PERIPHERALS-STATUS.md')
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
except BaseException:
 for f in (b/'EFI').rglob('*'):
  if f.is_file():cp(f,u/'EFI'/f.relative_to(b/'EFI'))
 for f in (u/'EFI').rglob('*'):
  if f.is_file() and not f.name.startswith('._') and str(f.relative_to(u/'EFI')) not in before:f.unlink()
 for f in (b/'port').rglob('*'):
  if f.is_file():cp(f,port/f.relative_to(b/'port'))
 for n in changed:
  if (port/n).is_file():(port/n).unlink()
 cp(b/'SHA256SUMS.json',port/'SHA256SUMS.json')
 raise
subprocess.run(['/bin/sync'],check=True)
print('Published backed-up Bluetooth/WMI USB candidate plus opt-in ELAN trial; all',len(m),'tracked hashes, frozen graphics baseline and EFI diff verified.')
