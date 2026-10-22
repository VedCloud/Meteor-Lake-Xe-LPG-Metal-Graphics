from pathlib import Path
import json,hashlib,shutil,subprocess,tempfile
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';per=port/'Peripherals';v=r/'port-artifacts/input-recovery-20261021';v.mkdir(exist_ok=True)
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.input-tmp');shutil.copy2(a,tmp);tmp.replace(b)
def tree(a,b):
 for f in a.rglob('*'):
  if f.is_file() and not f.name.startswith('._'):cp(f,b/f.relative_to(a))
def inv(a):return {str(f.relative_to(a)):sha(f) for f in a.rglob('*') if f.is_file() and not f.name.startswith('._')}
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
assert (u/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes(),'Restore input trial before publishing'
assert (u/'EFI/OC/ACPI/DSDT.aml').read_bytes()==(per/'Base/DSDT.aml').read_bytes()
b=r/'backups/input-recovery-before-20261021';assert not b.exists();b.mkdir()
tree(u/'EFI',b/'EFI');tree(per,b/'Peripherals');cp(port/'collect-desktop-failure.sh',b/'collect-desktop-failure.sh');cp(port/'SHA256SUMS.json',b/'SHA256SUMS.json')
before=inv(u/'EFI');base_hid=u/'EFI/OC/Kexts/VoodooI2CHID.kext';new_hid=r/'desktop-build/hid-readguard/VoodooI2CHID.kext';new_keys=r/'hotkey-build/LatitudeHotkeys.kext'
for bundle in [new_hid,new_keys]:subprocess.run(['codesign','--verify','--strict',str(bundle)],check=True)
# Build the replacement peripheral directory locally first.
stage=v/'Peripherals';tree(per,stage);tree(base_hid,stage/'Base/VoodooI2CHID.kext');tree(new_hid,stage/'Touchpad/VoodooI2CHID.kext');tree(new_keys,stage/'Kexts/LatitudeHotkeys.kext')
script=(stage/'touchpad-trial.sh').read_text()
script=script.replace('check_tree "$HERE" "$HERE/TRIAL-SHA256SUMS.txt"','''check_tree "$HERE" "$HERE/TRIAL-SHA256SUMS.txt"
for hid_file in Contents/Info.plist Contents/MacOS/VoodooI2CHID; do
 [ "$(hash "$OC/Kexts/VoodooI2CHID.kext/$hid_file")" = "$(hash "$HERE/$from/VoodooI2CHID.kext/$hid_file")" ] || { echo 'HID bundle changed; refusing overwrite.' >&2; exit 2; }
done''')
script=script.replace('rollback() {','''copy_hid() {
 /bin/rm -rf "$OC/Kexts/VoodooI2CHID.kext.input-new"
 /bin/cp -R "$HERE/$1/VoodooI2CHID.kext" "$OC/Kexts/VoodooI2CHID.kext.input-new"
 /bin/rm -rf "$OC/Kexts/VoodooI2CHID.kext"
 /bin/mv "$OC/Kexts/VoodooI2CHID.kext.input-new" "$OC/Kexts/VoodooI2CHID.kext"
}
rollback() {
 copy_hid "$from"''')
script=script.replace('/bin/cp "$HERE/$to/DSDT.aml"','copy_hid "$to"\n/bin/cp "$HERE/$to/DSDT.aml"',1)
(stage/'touchpad-trial.sh').write_text(script)
files=sorted(f for n in ['Base','Touchpad'] for f in (stage/n).rglob('*') if f.is_file() and not f.name.startswith('._') and f.suffix!='.dsl')
(stage/'TRIAL-SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(stage))+'\n' for f in files))
with tempfile.TemporaryDirectory(prefix='input-recovery-test-') as tmp:
 mock=Path(tmp);dest=mock/'iGPU-Port/Peripherals';tree(stage,dest);oc=mock/'EFI/OC';(oc/'ACPI').mkdir(parents=True);tree(base_hid,oc/'Kexts/VoodooI2CHID.kext');cp(stage/'Base/config.plist',oc/'config.plist');cp(stage/'Base/DSDT.aml',oc/'ACPI/DSDT.aml')
 for mode in ['apply','restore']:
  subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),mode],check=True,capture_output=True)
  expected=stage/('Touchpad' if mode=='apply' else 'Base')
  assert inv(oc/'Kexts/VoodooI2CHID.kext')==inv(expected/'VoodooI2CHID.kext')
  assert sha(oc/'config.plist')==sha(expected/'config.plist') and sha(oc/'ACPI/DSDT.aml')==sha(expected/'DSDT.aml')
 (oc/'Kexts/VoodooI2CHID.kext/Contents/Info.plist').write_bytes(b'changed')
 assert subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),'apply'],capture_output=True).returncode==2
 assert sha(oc/'config.plist')==sha(stage/'Base/config.plist')
print('PASS actual revised trial fixture: HID/config/DSDT apply and restore, changed-HID refusal.')
doc='''# Input recovery candidate — 21 October 2026

Evidence: failure-detail-20261021-002150 and opencore-2026-10-20-231422.txt. OpenCore explicitly rejected LatitudeHotkeys0.1.0 injection as Invalid Parameter. Its binary had no _kmod_info/start/stop metadata. 0.1.1 supplies explicit matching module identity/version, kernel startup/shutdown and compiler markers, and OSBundleRequired Root. Build checks require defined metadata/entry-point symbols; signature verified. AML revision5 and WMI event routing unchanged. No claim of successful target injection until next boot.

ELAN attached but final read was kIOReturnTimeout0xe00002d6. The installed HID1.0.1 getInputReport ignored readI2C's result, parsed uninitialized/partial buffers, did not reject length1 before length-2 underflow, and handed the HID parser a buffer two bytes longer than its initialized payload. HID1.0.2 checks read status and allocations, validates both length bounds, preserves reset completion on successful length0, dispatches exactly length-2 initialized bytes, and never dispatches failed input. Repeated ELAN polling failures back off50ms then250ms; normal scheduling resumes on the next successful read. This fixes concrete parser defects, not a demonstrated root cause of the controller timeout or keyboard freeze. No speculative controller/MMIO/EC resets.

Tests: actual compiled report/poll functions under ASan/UBSan over all65536 wire lengths, failed reads/no false reset, allocation/short-write failures, exact payload/readiness/capacity, bounded backoff/recovery. Negative control reproduces previous length1 underflow. Existing framebuffer-transform null guard remains and its200000-case test passes. WMI mock tests pass. Actual trial switcher tested with apply/restore and changed-HID refusal. X86 kext builds/signatures pass. No hardware access during host tests.

Default USB retains restored PS2 pointer and original HID1.0.1. Only LatitudeHotkeys0.1.1 is replaced in active EFI. Revised touchpad trial includes HID1.0.2 and preserves keyboard/touchscreen support; gestures and built-in keyboard stability need a Dell boot. Apply with:
  /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
Restore with the same command ending restore, then reboot via USB. Restore now restores the previous HID binary as well as config/DSDT/PS2 pointer plugins.

Use existing WindowServer installer only to arm an accelerated boot, then apply the touchpad trial before that reboot. No graphics binaries changed and no rendering suite needed. After testing keys/touchpad, run the normal collect-desktop-failure.sh. It now retains latest3 OpenCore logs and input kext info so an enabled entry cannot be mistaken for successful injection. HID registry counters expose failed/malformed/delivered reports. External-keyboard volume working supports an input-path problem; exact built-in keyboard freeze cause remains unproven.

Sources:
- Linux I2C-HID receive checks and payload stripping: https://github.com/torvalds/linux/blob/master/drivers/hid/i2c-hid/i2c-hid-core.c
- VoodooI2C HID code: https://github.com/VoodooI2C/VoodooI2CHID
- OpenCore prelinked kext implementation: https://github.com/acidanthera/OpenCorePkg/blob/master/Library/OcAppleKernelLib/PrelinkedKext.c
- Upstream similar symptom reports (not proof of this Dell's cause): https://github.com/VoodooI2C/VoodooI2C/issues/321 and https://github.com/VoodooI2C/VoodooI2C/issues/456

Backup: iGPU-Port/backups/input-recovery-before-20261021 (EFI, Peripherals, collector; not an installed OS snapshot). Bluetooth, GPU, audio, XOSI, NVRAM and security policy retained.
'''
(v/'README.md').write_text(doc);(stage/'INPUT-RECOVERY.md').write_text(doc)
for f in (r/'hotkey-sources').glob('*'):
 if f.is_file():cp(f,stage/'Source/Hotkeys'/f.name)
src=r/'research/voodooi2c-core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID'
for f in ['VoodooI2CHIDDevice.cpp','VoodooI2CHIDDevice.hpp']:cp(src/f,stage/'Source/HIDReadGuard'/f)
for f in ['build-hid-readguard.py','test-hid-readguard.py','collect-desktop-failure.sh','publish-input-recovery.py']:cp(r/'desktop-sources'/f,stage/'Source/HIDReadGuard'/f)
validation={'hotkeys_version':'0.1.1','hotkeys_uuid':'29860718-17D4-3DE3-B62F-6A626AB5CEF2','hid_version':'1.0.2','hid_uuid':'19A6E418-8463-321D-8AD8-8B7358BBAC55','hardware_verified':False,'controller_timeout_root_cause_proven':False,'keyboard_freeze_root_cause_proven':False,'default_touchpad_trial_enabled':False,'active_config_changed':False,'graphics_payloads_changed':False,'tests':['actual HID65536-length ASan/UBSan and previous malformed-report negative control','actual transform200000-case regression','actual WMI mock lifecycle/ownership/dispatch','actual trial fixture apply/restore/changed-HID refusal','x86 builds/signatures and defined module metadata']}
(v/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n');cp(v/'VALIDATION.json',stage/'INPUT-RECOVERY-VALIDATION.json')
tree(b,port/'backups/input-recovery-before-20261021')
try:
 tree(stage,per);tree(new_keys,u/'EFI/OC/Kexts/LatitudeHotkeys.kext');cp(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
 after=inv(u/'EFI')
 for n,h in before.items():assert n.startswith('OC/Kexts/LatitudeHotkeys.kext/') or after.get(n)==h,n
 assert set(after)-set(before) <= {'OC/Kexts/LatitudeHotkeys.kext/_CodeSignature/CodeResources'}
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 for line in (r/'latitude-desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 for f in per.rglob('*'):
  if f.is_file() and not f.name.startswith('._'):m[str(f.relative_to(port))]=sha(f)
 m['collect-desktop-failure.sh']=sha(port/'collect-desktop-failure.sh')
 for n,h in m.items():assert sha(port/n)==h,n
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
except BaseException:
 tree(b/'EFI',u/'EFI');shutil.rmtree(per);tree(b/'Peripherals',per);cp(b/'collect-desktop-failure.sh',port/'collect-desktop-failure.sh');cp(b/'SHA256SUMS.json',port/'SHA256SUMS.json');raise
subprocess.run(['/bin/sync'],check=True)
print('Published input recovery candidate;',len(m),'root hashes, exact EFI change, signatures and frozen graphics baseline verified. Default pointer setup retained.')
