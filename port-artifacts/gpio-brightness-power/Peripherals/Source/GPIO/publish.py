from pathlib import Path
import shutil,json,hashlib,subprocess,tempfile
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';per=port/'Peripherals';v=r/'port-artifacts/gpio-brightness-power'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.gpio-tmp');shutil.copy2(a,tmp);tmp.replace(b)
def tree(a,b):
 for f in a.rglob('*'):
  if f.is_file() and not f.name.startswith('._'):cp(f,b/f.relative_to(a))
def inv(a):return {str(f.relative_to(a)):sha(f) for f in a.rglob('*') if f.is_file() and not f.name.startswith('._')}
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
assert (u/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes(),'Restore previous trial first'
assert (u/'EFI/OC/ACPI/DSDT.aml').read_bytes()==(per/'Base/DSDT.aml').read_bytes()
b=r/'backups/gpio-brightness-power-before';assert not b.exists();b.mkdir();tree(u/'EFI',b/'EFI');tree(per,b/'Peripherals');cp(port/'collect-desktop-failure.sh',b/'collect-desktop-failure.sh');cp(port/'SHA256SUMS.json',b/'SHA256SUMS.json');before=inv(u/'EFI')
stage=v/'Peripherals';tree(per,stage)
for name in ['config.plist','DSDT.aml']:cp(v/name,stage/'Touchpad'/name)
tree(r/'desktop-build/gpio-mtl/LatitudeGPIO.kext',stage/'Touchpad/LatitudeGPIO.kext');tree(r/'desktop-build/hid-gpio/VoodooI2CHID.kext',stage/'Touchpad/VoodooI2CHID.kext')
# Update the existing two-way switcher: copy GPIO before enabling its config.
s=(stage/'touchpad-trial.sh').read_text();oldcfg=sha(per/'Touchpad/config.plist');olddsdt=sha(per/'Touchpad/DSDT.aml');s=s.replace(oldcfg,sha(v/'config.plist')).replace(olddsdt,sha(v/'DSDT.aml'))
s=s.replace('if [ "$from" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi','if [ "$from" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext" "$OC/Kexts/LatitudeGPIO.kext"; else /bin/rm -rf "$OC/Kexts/LatitudeGPIO.kext"; /bin/cp -R "$HERE/Touchpad/LatitudeGPIO.kext" "$OC/Kexts/"; fi')
s=s.replace('/bin/cp -R "$HERE/Touchpad/LatitudeI2CMeteorLake.kext" "$OC/Kexts/"','[ ! -e "$OC/Kexts/LatitudeGPIO.kext" ] || { echo "Unexpected GPIO bundle; refusing." >&2; exit 2; }\n /bin/cp -R "$HERE/Touchpad/LatitudeGPIO.kext" "$OC/Kexts/"\n /bin/cp -R "$HERE/Touchpad/LatitudeI2CMeteorLake.kext" "$OC/Kexts/"')
s=s.replace('if [ "$to" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext"; fi','if [ "$to" = Base ]; then /bin/rm -rf "$OC/Kexts/LatitudeI2CMeteorLake.kext" "$OC/Kexts/LatitudeGPIO.kext"; fi')
(stage/'touchpad-trial.sh').write_text(s)
files=sorted(f for n in ['Base','Touchpad'] for f in (stage/n).rglob('*') if f.is_file() and not f.name.startswith('._') and f.suffix!='.dsl');(stage/'TRIAL-SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(stage))+'\n' for f in files))
for source,folder in [(r/'gpio-sources','Source/GPIO'),(r/'brightness-sources','Brightness/Source'),(r/'power-sources','Power/Source')]:tree(source,stage/folder)
cp(r/'desktop-build/latitude-brightness',stage/'Brightness/latitude-brightness');cp(r/'brightness-sources/install-brightness-keys.sh',stage/'Brightness/install-brightness-keys.sh');cp(r/'desktop-build/latitude-power-snapshot',stage/'Power/latitude-power-snapshot')
cp(r/'research/voodooi2c-core/Dependencies/VoodooGPIO/LICENSE.md',stage/'Source/GPIO/VoodooGPIO-LICENSE.md');cp(r/'research/pinctrl-meteorlake.c',stage/'Source/GPIO/pinctrl-meteorlake.c')
for n in ['VoodooI2CHIDDevice.cpp','VoodooI2CHIDDevice.hpp']:cp(r/'research/voodooi2c-core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID'/n,stage/'Source/HIDReadGuard'/n)
cp(r/'desktop-sources/build-hid-gpio.py',stage/'Source/HIDReadGuard/build-hid-gpio.py')
with tempfile.TemporaryDirectory(prefix='latitude-gpio-trial-') as tmp:
 mock=Path(tmp);dest=mock/'iGPU-Port/Peripherals';tree(stage,dest);oc=mock/'EFI/OC';(oc/'ACPI').mkdir(parents=True);tree(stage/'Base/VoodooI2CHID.kext',oc/'Kexts/VoodooI2CHID.kext');cp(stage/'Base/config.plist',oc/'config.plist');cp(stage/'Base/DSDT.aml',oc/'ACPI/DSDT.aml')
 for mode in ['apply','restore']:
  subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),mode],check=True,capture_output=True)
  expected=stage/('Touchpad' if mode=='apply' else 'Base');assert inv(oc/'Kexts/VoodooI2CHID.kext')==inv(expected/'VoodooI2CHID.kext');assert sha(oc/'config.plist')==sha(expected/'config.plist')
  assert (oc/'Kexts/LatitudeGPIO.kext').exists()==(mode=='apply')
 (oc/'config.plist').write_bytes(b'changed');assert subprocess.run(['/bin/bash',str(dest/'touchpad-trial.sh'),'apply'],capture_output=True).returncode==2
print('PASS actual GPIO/HID/config switcher apply/restore and stale-config refusal in disposable fixture.')
doc='''# GPIO, brightness shortcuts and normal-performance power candidate

Dell Latitude5350, Sequoia15.8.1. This is a hardware-test candidate, not a verified fix.

Touchpad: Dell runtime SBFG reports GPIO387. Linux INTC1083 Meteor Lake-P table maps gpio_base384+3 to physical pad232/GPP_D_3, community5 (memory resource4). New LatitudeGPIO0.1.0 subclasses the installed VoodooGPIO base, supports only that pin and active-low level mode, maps bounded firmware resources, checks revision/PADBAR, pad ownership/lock/enable state, and uses Meteor Lake status0x204/enable0x214/host-own0x144. It alters only the target pin's bits, refuses another enabled owner and restores its pin configuration on stop. It never initializes every GPIO community. A filtered interrupt schedules work on a dedicated workloop. Current upstream TigerLakeLP is a reference, not a copied register layout.

DSDT changes only TPD0 _DSM/_CRS bodies and checksum, preserves package lengths, returns firmware SBFB+SBFG. Removes only the I2C3 force-polling property, retains keyboard, and disables competing PS2 pointer plugins for the trial. HID1.0.3 keeps the read/status/length fixes; this ELAN refuses silent polling fallback if IRQ registration fails. Touchscreen remains on its existing setup. No claim of interrupt delivery, gestures, sleep/wake or keyboard stability until Dell test.

Brightness: install a user-session shortcut helper through the existing IODisplay linear-brightness API. No raw MMIO access, second PWM writer, key-content logging or GPU changes. F6/F7, Fn+F6/F7 where delivered, or Control+Option+F6/F7 on an external keyboard step brightness; NX brightness events are recognized too. Events that fail to adjust are passed through. Needs Accessibility permission for the installed helper, then rerun installer. A frozen keyboard cannot generate shortcuts. Manual + and - commands need no Accessibility permission. No native login-window key support is claimed.

Power: NORMAL performance settings preserved. No Low Power Mode, CPU caps, altered sleep/hibernation policy, CPUFriend profile or forced frequency. ELAN IRQ avoids its high-frequency polling when functional; the untouched touchscreen still uses polling and remains a future power investigation. Collector now captures signed battery current/voltage/discharge watts, pmset settings/assertions, top CPU users, bounded CPU power sampler, GPIO/IRQ mode/counters and input failures. Unsupported readings are errors, not zero watts; external-power/charging samples do not assert battery discharge power. These are diagnostic snapshots, not a measured Windows-runtime comparison. Matching Windows requires matched workload/brightness/radio conditions and actual discharge tests.

Use on Dell:
1. Run the usual install-windowserver.sh only if an accelerated one-shot boot needs arming, and confirm exit0.
2. /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/touchpad-trial.sh apply
3. Reboot through USB. Test typing/volume, pointer and gestures. Keep external input available.
4. /bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Brightness/install-brightness-keys.sh
   Allow ~/Library/Application Support/LatitudeInput/latitude-brightness in Privacy & Security > Accessibility, then rerun step4. Without permission you may directly run that binary with + or -.
5. sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
Restore input with touchpad-trial.sh restore then USB reboot. The default USB remains the restored PS2 setup until apply. No power-settings command is needed or included. Remove shortcut helper using launchctl bootout gui/$(id -u)/com.ved.latitude-brightness and remove its named LaunchAgent if desired.

Validation: GPIO firmware/Linux mapping and bounds/owner/lock tests; actual driver ASan/UBSan mocked MMIO/start failures/restore; exact AML method-body execution; brightness65536 keycode/modifier and system-event mapping tests; HID65536 wire-length tests/negative control; collector fixture; revised actual switcher fixture; x86 builds/signatures, dependency symbols, OpenCore validator and frozen graphics baseline. Mocked tests cannot prove physical IRQ delivery or hardware power savings. No full GPU test suite rerun.

Sources:
- https://github.com/torvalds/linux/blob/master/drivers/pinctrl/intel/pinctrl-meteorlake.c
- https://github.com/VoodooI2C/VoodooGPIO
- https://github.com/JulioNectar/Hackintosh-XPS13Plus-IrisXe-Tahoe/blob/main/CHANGELOG.md (reports polling-related input freezes fixed with interrupts on Raptor Lake)
- https://github.com/AppleBreak1/Dell-Latitude-5300-2-in-1-Hackintosh (older working Dell VoodooI2C setup; not a Meteor Lake register map)
- https://github.com/MokkaSchnalle/macOS-Dell-5411-5511 (older Latitude interrupt/CPU-load experience)

Backup: iGPU-Port/backups/gpio-brightness-power-before; includes previous EFI/peripherals/collector. Graphics, Bluetooth, audio, security and host/target power settings unchanged.
'''
(stage/'README.md').write_text(doc);(stage/'GPIO-BRIGHTNESS-POWER.md').write_text(doc);(v/'README.md').write_text(doc)
for p in [stage/'Touchpad/LatitudeGPIO.kext',stage/'Touchpad/VoodooI2CHID.kext',stage/'Brightness/latitude-brightness',stage/'Power/latitude-power-snapshot']:subprocess.run(['codesign','--verify','--strict',str(p)],check=True)
for p in [stage/'touchpad-trial.sh',stage/'Brightness/install-brightness-keys.sh',r/'desktop-sources/collect-desktop-failure.sh']:subprocess.run(['/bin/bash','-n',str(p)],check=True)
# Backup first; only package/collector mutations are authorized here, not active EFI.
tree(b,port/'backups/gpio-brightness-power-before')
try:
 tree(stage,per);cp(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
 assert inv(u/'EFI')==before
 for line in (r/'latitude-desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 for f in per.rglob('*'):
  if f.is_file() and not f.name.startswith('._'):m[str(f.relative_to(port))]=sha(f)
 m['collect-desktop-failure.sh']=sha(port/'collect-desktop-failure.sh')
 for n,h in m.items():assert sha(port/n)==h,n
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
except BaseException:
 shutil.rmtree(per);tree(b/'Peripherals',per);cp(b/'collect-desktop-failure.sh',port/'collect-desktop-failure.sh');cp(b/'SHA256SUMS.json',port/'SHA256SUMS.json');raise
subprocess.run(['/bin/sync'],check=True)
print('Published GPIO/brightness/power-diagnostics candidate:',len(m),'verified hashes; active EFI and power settings unchanged.')
