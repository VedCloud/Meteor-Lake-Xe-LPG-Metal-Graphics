# SPDX-License-Identifier: MIT
"""Publish only the OVMF-qualified diagnostic. No default bootloader installation."""
from pathlib import Path
import json,hashlib,shutil,subprocess,plistlib
r=Path(__file__).resolve().parent;research=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research');usb=Path('/Volumes/macOS 27');efi=Path('/Volumes/EFI');pkg=usb/'Standalone-UEFI-Preflight'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
build=json.loads((r/'BUILD.json').read_text());validation=json.loads((r/'VALIDATION.json').read_text());assert validation['exactEFISHA256']==sha(r/'Dell27Preflight.efi')==build['SHA256'];assert validation['OVMFCases']=={'valid':'PASS','tampered':'PASS'}
a=plistlib.loads(subprocess.check_output(['diskutil','info','-plist',str(usb)]));b=plistlib.loads(subprocess.check_output(['diskutil','info','-plist',str(efi)]));assert a['ParentWholeDisk']==b['ParentWholeDisk'] and not a['Internal'] and a['VolumeName']=='macOS 27' and b['VolumeName']=='EFI'
old=efi/'EFI/Tools/Dell27Preflight.efi';oldbuild=json.loads((pkg/'BUILD.json').read_text());oldversion=oldbuild.get('version','0.0.1')
if oldversion!=build['version']:
 archive=research/('Standalone-'+oldversion+'-archive')
 if not archive.exists():
  archive.mkdir();shutil.copytree(pkg,archive/'Package');shutil.copy2(old,archive/'Dell27Preflight.efi')
 assert sha(archive/'Dell27Preflight.efi')==oldbuild['SHA256']
source=pkg/'Source';source.mkdir(exist_ok=True);tests=source/'RuntimeTests';tests.mkdir(exist_ok=True)
for name in ['Dell27Preflight.efi','README.md','BUILD.json','VALIDATION.json','RUNTIME-VALIDATION.json','ovmf-valid.log','ovmf-tampered.log']:
 shutil.copy2(r/name,pkg/name)
for pattern in ['*.c','*.h','*.py']:
 for p in r.glob(pattern):shutil.copy2(p,source/p.name)
for pattern in ['*.c','*.S','*.ld']:
 for p in (r/'RuntimeTests').glob(pattern):shutil.copy2(p,tests/p.name)
for name in ['probe.bin','data.bin','qemu-stdout.bin','qemu-stderr.bin','interpreter-results.bin','privileged.bin','privileged-data.bin','privileged-qemu.bin','privileged-result.bin','sptm-entry.log']:
 shutil.copy2(r/'RuntimeTests'/name,tests/name)
shutil.copy2(research/'FIRMWARE-ENGINE-RESEARCH.md',pkg/'FIRMWARE-ENGINE-RESEARCH.md');shutil.copy2(r.parent/'UnicornBuild/STATUS.json',pkg/'FIRMWARE-ENGINE-STATUS.json')
shutil.copy2(r/'Dell27Preflight.efi',old);assert sha(old)==build['SHA256']
files={str(p.relative_to(pkg)):sha(p) for p in pkg.rglob('*') if p.is_file() and not p.name.startswith('._') and p.name not in ['SHA256SUMS.json','EFI-FILES-SHA256SUMS.json']}
(pkg/'SHA256SUMS.json').write_text(json.dumps(files,indent=2)+'\n')
for name,hash in files.items():assert sha(pkg/name)==hash
firmware={str(p.relative_to(efi)):sha(p) for p in (efi/'macOS27').rglob('*') if p.is_file() and not p.name.startswith('._')};firmware['EFI/Tools/Dell27Preflight.efi']=sha(old);(pkg/'EFI-FILES-SHA256SUMS.json').write_text(json.dumps(firmware,indent=2)+'\n')
report={'USBVolume':str(usb),'EFI':str(old),'EFISHA256':sha(old),'version':build['version'],'PackageFilesVerified':len(files),'EFIFilesVerified':len(firmware),'selectedEL2Probe':'PASS QEMU and x86 OVMF','SPTMEntryProbe':'Five real instructions executed; expected unmapped boot-argument stop','completeARMRuntime':False,'DefaultBootloaderInstalled':False,'SequoiaEFIRestored':False,'LinuxHostUsed':False,'DellBootTested':False}
for target in [r/'USB-PUBLISHED.json',research/'USB-PUBLISHED.json']:target.write_text(json.dumps(report,indent=2)+'\n')
snapshot=research/'Standalone-UEFI-Source'
for p in pkg.rglob('*'):
 if p.is_file() and not p.name.startswith('._'):
  dest=snapshot/p.relative_to(pkg);dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,dest)
p=research/'RESEARCH-STATUS.json';d=json.loads(p.read_text());d.update(standaloneEFI='0.0.3 selected EL2 and SPTM entry probes, OVMF qualified',standaloneARMExecution='Bounded scalar/EL2 execution; complete runtime incomplete',SPTMEntry='Five instructions executed then expected boot-argument data fault',broaderEngine='Intel Unicorn EFI: 80 source files compiled, 28 unresolved firmware helpers, not runtime qualified',USBPublishedReport=report);p.write_text(json.dumps(d,indent=2)+'\n')
(usb/'READ-ME-FIRST.txt').write_text('Dell macOS 27 work — 2026-10-22\n\nStandalone-UEFI-Preflight is version 0.0.3. It executes scalar and selected EL2 probes, then five instructions from the real pinned SPTM image in a bounded private mapping. EXPECTED STOP at the unmapped boot-argument read. It cannot boot the kernel or desktop. Firmware Boot from File path: EFI\\Tools\\Dell27Preflight.efi. No default bootloader installed.\n\nThe broader Unicorn firmware CPU engine compiles but has not linked or run; it is not included in this EFI. macOS27-Shell remains a separate hosted QEMU shell reference, requiring a macOS host. No Sequoia EFI restored. Full original USB backup remains on Corsair.\n')
print(json.dumps(report,indent=2))
