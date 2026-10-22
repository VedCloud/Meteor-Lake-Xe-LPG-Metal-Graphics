from pathlib import Path
import shutil,hashlib,json,plistlib,subprocess
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
b=r/'backups/input-system-sync-20261021';ub=port/'backups/input-system-sync-20261021'
assert not b.exists() and not ub.exists(), 'One-shot publisher: backup already exists'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
baseline=port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt'
for line in baseline.read_text().splitlines():
 h,name=line.split('  ',1);assert sha(port/name)==h,name
manifest=json.loads((port/'SHA256SUMS.json').read_text())
info=Path('EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist')
candidate=r/'port-artifacts/copilot-command/VoodooPS2Keyboard.kext/Contents/Info.plist'
original=plistlib.loads((u/info).read_bytes());new=plistlib.loads(candidate.read_bytes())
# Ensure the candidate changes only profile macro arrays, not identity, mappings or code.
import copy
a=copy.deepcopy(original);c=copy.deepcopy(new)
def clean(d):
 if isinstance(d,dict):
  d.pop('Macro Inversion',None)
  for v in d.values():clean(v)
 elif isinstance(d,list):
  for v in d:clean(v)
clean(a);clean(c);assert a==c
b.mkdir(parents=True);shutil.copytree(u/'EFI',b/'EFI')
shutil.copytree(port/'Peripherals/Brightness',b/'Brightness')
shutil.copy2(port/'collect-desktop-failure.sh',b/'collect-desktop-failure.sh')
shutil.copy2(port/'SHA256SUMS.json',b/'SHA256SUMS.json')
shutil.copytree(b,ub)
changed=[]
def put(src,dest):
 dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dest);changed.append(dest)
bright=port/'Peripherals/Brightness'
put(r/'desktop-build/latitude-brightness',bright/'latitude-brightness')
put(r/'brightness-sources/install-brightness-keys.sh',bright/'install-brightness-keys.sh')
for p in (r/'brightness-sources').iterdir():
 if p.is_file():put(p,bright/'Source'/p.name)
put(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
cp=port/'Peripherals/CopilotCommand';cp.mkdir(parents=True,exist_ok=True)
for p in [r/'copilot-sources/prepare.py',r/'copilot-sources/test-macros.py',r/'port-artifacts/copilot-command/macros.json']:
 put(p,cp/p.name)
# Install only the unsigned keyboard plugin's Info.plist. No executable/signature changes.
shutil.copy2(candidate,u/info)
assert sha((u/info).parent/'MacOS/VoodooPS2Keyboard')==sha((b/info).parent/'MacOS/VoodooPS2Keyboard')
assert sha(u/'EFI/OC/config.plist')==sha(b/'EFI/OC/config.plist')
restore=cp/'restore-copilot.sh'
restore.write_text('''#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
REL="'''+str(info)+'''"
EXPECTED="'''+sha(candidate)+'''"
[ "$(shasum -a 256 "$ROOT/$REL" | cut -d ' ' -f1)" = "$EXPECTED" ] || { echo 'Keyboard configuration has changed; refusing to overwrite it.' >&2; exit 2; }
cp "$ROOT/iGPU-Port/backups/input-system-sync-20261021/$REL" "$ROOT/$REL"
echo 'Original keyboard profile restored. Reboot through this USB.'
''');restore.chmod(0o755);changed.append(restore)
doc=cp/'README.md'
doc.write_text('''# Copilot → Command keyboard candidate
Applied to this USB's VoodooPS2Keyboard Info.plist only; existing executable, OpenCore config and other kexts are unchanged. Boot through this USB to test. This is PS/2 driver translation before macOS userspace, not BIOS/EC flashing and does not modify Windows.

The candidate recognizes the standard Shift+Windows+F23 sequence and emits a balanced Command press/release. Tested the upstream inversion functions under ASan/UBSan for all 36 make/release orders, Command+C, normal Shift+C, and timeout flush. The Dell's actual chord timing and hold behaviour remain unverified. First test Copilot+C and Copilot+V, then check ordinary Shift typing and modifier release. If the firmware emits only a tap, holding it as Command may require a different implementation.

Revert with `/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/CopilotCommand/restore-copilot.sh` then reboot through USB. No touchpad trial is enabled.

Source: https://github.com/acidanthera/VoodooPS2 ; standard Copilot chord reports: https://github.com/microsoft/PowerToys/issues/36692
''');changed.append(doc)
doc=bright/'README-system-sync.md'
doc.write_text('''# System brightness helper 0.2.0 (Sequoia)
The earlier helper physically adjusted linear brightness. This candidate uses DisplayServicesGetBrightness/SetBrightness for system state and calls the native Sequoia OSDManager brightness HUD. It derives the unique display ID from the Latitude panel registry vendor/product IDs. Missing APIs, ambiguous displays and failed writes are reported rather than treated as successful changes. No MMIO, keyboard content logging, TCC editing or graphics changes.

On the Dell, run without sudo:
`/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Brightness/install-brightness-keys.sh`

If Accessibility needs reapproval after the binary update, enable the installed latitude-brightness binary under Privacy & Security → Accessibility and rerun the installer. Test F6/F7, compare the HUD and Control Centre slider. Physical dimming worked in 0.1.0; system slider and OSD in 0.2.0 require this hardware test. Copilot is independent and requires a USB reboot.

The existing collect-desktop-failure.sh now records the installed helper's read-only diagnose JSON plus recent helper errors. Diagnose never changes brightness. The working 0.1.0 helper and previous collector are backed up under iGPU-Port/backups/input-system-sync-20261021.

References: https://github.com/nriley/brightness/blob/master/brightness.c and https://github.com/MonitorControl/MonitorControl/blob/main/MonitorControl/Support/OSDUtils.swift . These private APIs are for the current Sequoia installation; Tahoe needs separate validation.
''');changed.append(doc)
for p in changed:manifest[str(p.relative_to(port))]=sha(p)
(port/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n')
for p in changed:assert sha(p)==manifest[str(p.relative_to(port))]
for line in baseline.read_text().splitlines():
 h,name=line.split('  ',1);assert sha(port/name)==h,name
print('Published brightness0.2.0, collector diagnostics and active USB Info-only Copilot candidate. Full EFI backup saved; baseline verified. Changed files:',len(changed))
