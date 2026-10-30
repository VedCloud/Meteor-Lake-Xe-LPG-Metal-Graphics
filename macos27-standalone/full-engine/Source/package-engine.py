# SPDX-License-Identifier: MIT
from pathlib import Path
import json,hashlib,shutil,subprocess,tarfile,plistlib
r=Path(__file__).resolve().parent;w=r.parent
research=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research')
version='0.1.0';stage=research/('Standalone-Engine-'+version)
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
validation=json.loads((r/'FIRMWARE-VALIDATION.json').read_text());vm=json.loads((r/'VMAPPLE-FIRMWARE-EXPLORATORY.json').read_text())
assert validation['OVMFCases']=={'valid':'PASS','tampered':'PASS'}
assert vm['OVMFCases']=={'valid':'PASS','tampered':'PASS','vm-tampered':'PASS'}
assert validation['EFI_SHA256']==sha(r/'UnicornTest.efi')
assert vm['EFI_SHA256']==sha(r/'Dell27VMAppleExperiment.efi')
assert not validation['kernelBoot'] and not vm['kernelBoot']
assert vm['VMAPPLEReplay']['instructionsVisited']>=30000000
stage.mkdir(exist_ok=True)
for sub in ['Evidence','Inputs','Source']: (stage/sub).mkdir(exist_ok=True)
shutil.copy2(r/'UnicornTest.efi',stage/'Dell27UnicornEngine.efi');shutil.copy2(r/'Dell27VMAppleExperiment.efi',stage/'Dell27VMAppleExperiment.efi');shutil.copy2(r/'README.md',stage/'README.md')
# Include new source files as well as tracked modifications in the patch.
u=w/'unicorn-for-efi'
patch=subprocess.check_output(['git','diff','--binary'],cwd=u)
new=u/'qemu/target/arm/vmapple-pac.c.inc'
p=subprocess.run(['git','diff','--no-index','--','/dev/null',str(new.relative_to(u))],cwd=u,capture_output=True);assert p.returncode==1
patch+=p.stdout;(r/'unicorn-firmware-fixes.patch').write_bytes(patch)
status={'version':version,'engine':'Intel Unicorn EFI with local system profiles','upstream':'https://github.com/intel/unicorn-for-efi','revision':'40a8d07ba3cef067d9a6455c163dbed8d94f8e38','sourceModified':True,'sourcesCompiled':80,'linkStage':'x86-64 UEFI PE application, ELF static link and EDK2 GenFw conversion','unresolvedServices':[],'runtimeTested':'x86 OVMF: real firmware service bridge and ARM64 execution','standaloneKernelBoot':False,'DellHardwareTested':False,'EFISHA256':validation['EFI_SHA256'],'SPRR':validation['SPRRPermissions'],'PAC':validation['VMAPPLEPAC'],'SPTMNextStop':validation['SPTMReplay'],'VMAPPLEReplay':vm['VMAPPLEReplay'],'remaining':['APSTS/key/guarded-mode semantics for physical profile','complete guest RAM and initialized ramdisk/trust-cache loading','valid VMAPPLE board/device tree','interrupt/timer/device delivery and guest exception routing','full boot lifecycle, storage, graphics and input']}
(r/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n');(r/'UNRESOLVED.txt').write_text('No undefined symbols in UnicornTest.elf. See FIRMWARE-VALIDATION.json for runtime qualification and remaining platform work.\n')
for name in ['STATUS.json','COMPILE.json','FIRMWARE-VALIDATION.json','VMAPPLE-FIRMWARE-EXPLORATORY.json','REFERENCE-BOOT.json','VMAPPLE-REFERENCE-BOOT.json','ovmf-engine-valid.log','ovmf-engine-tampered.log','ovmf-vmapple-valid.log','ovmf-vmapple-tampered.log','ovmf-vmapple-vm-tampered.log']:
 shutil.copy2(r/name,stage/'Evidence'/name)
for name in ['reference-boot-state.bin','vmapple-reference-boot-state.bin']:shutil.copy2(r/name,stage/'Inputs'/name)
shutil.copy2(r/'unicorn-firmware-fixes.patch',stage/'Source/unicorn-firmware-fixes.patch')
# Preserve complete corresponding sources and original licenses. Omit build
# products, repository metadata and private Apple firmware from source archive.
archive=stage/'Source/engine-sources.tar.gz'
with tarfile.open(archive,'w:gz',compresslevel=6) as tar:
 for directory in ['unicorn-for-efi','edk2','nanoprintf']:
  root=w/directory
  for f in sorted(root.rglob('*')):
   rel=f.relative_to(w)
   if not f.is_file() or '.git' in rel.parts or '__pycache__' in rel.parts:continue
   if f.suffix in ['.o','.obj','.a','.pyc'] or '/Source/C/bin/' in str(rel):continue
   tar.add(f,arcname=str(rel),recursive=False)
 for f in sorted(r.iterdir()):
  if f.is_file() and not f.name.startswith('kernel-') and (f.suffix in ['.c','.h','.py','.S','.inc','.md'] or f.name=='unicorn-firmware-fixes.patch'):
   tar.add(f,arcname='UnicornBuild/'+f.name,recursive=False)
 for f in sorted((w/'Standalone').rglob('*')):
  if f.is_file() and 'VirtualUSB' not in f.parts and '__pycache__' not in f.parts and f.suffix in ['.c','.h','.py','.S','.ld','.md']:
   tar.add(f,arcname='Standalone/'+str(f.relative_to(w/'Standalone')),recursive=False)
manifest={str(f.relative_to(stage)):sha(f) for f in sorted(stage.rglob('*')) if f.is_file() and f.name!='SHA256.json'}
(stage/'SHA256.json').write_text(json.dumps(manifest,indent=2)+'\n')
# Verify both mounted partitions belong to the same external USB.
def info(path):return plistlib.loads(subprocess.check_output(['diskutil','info','-plist',str(path)]))
esp=Path('/Volumes/EFI');main=Path('/Volumes/macOS 27');a=info(esp);b=info(main)
assert a['ParentWholeDisk']==b['ParentWholeDisk'] and not a['Internal'] and not b['Internal'] and a['BusProtocol']=='USB' and b['BusProtocol']=='USB'
assert a['VolumeUUID']=='6BC2C2D9-547C-33F0-BC92-5ABB1B3B123A' and b['VolumeUUID']=='7E12226F-FA27-3551-9825-9A04E2154289'
assert a['FreeSpace']>40*1024*1024
usb=main/stage.name
shutil.copytree(stage,usb,dirs_exist_ok=True)
for name,digest in manifest.items():assert sha(usb/name)==digest,name
(esp/'EngineProbe').mkdir(exist_ok=True);shutil.copy2(r/'reference-boot-state.bin',esp/'EngineProbe/reference-boot-state.bin');shutil.copy2(r/'UnicornTest.efi',esp/'EFI/Tools/Dell27UnicornEngine.efi')
assert sha(esp/'EFI/Tools/Dell27UnicornEngine.efi')==validation['EFI_SHA256'];assert sha(esp/'EngineProbe/reference-boot-state.bin')==sha(r/'reference-boot-state.bin')
assert sha(esp/'EFI/Tools/Dell27Preflight.efi')=='931ade60f39b34adfd282aef1faa8075ebcff7e0030d50d953a7c1ce9cbda5be'
assert not (esp/'EFI/BOOT/BOOTX64.EFI').exists()
published={'version':version,'package':str(usb),'verifiedPackageFiles':len(manifest),'EFI':str(esp/'EFI/Tools/Dell27UnicornEngine.efi'),'EFI_SHA256':validation['EFI_SHA256'],'SPTMCapture_SHA256':sha(r/'reference-boot-state.bin'),'defaultBootloaderInstalled':False,'kernelBoot':False,'DellHardwareTested':False}
(research/'ENGINE-USB-PUBLISHED.json').write_text(json.dumps(published,indent=2)+'\n')
(main/'READ-ME-FIRST.txt').write_text('Dell macOS 27 work — 2026-10-22\n\nStandalone-Engine-0.1.0 contains the running full Unicorn firmware engine, source and verified OVMF evidence. It is a diagnostic, not a macOS bootloader. Firmware Boot from File path: EFI\\Tools\\Dell27UnicornEngine.efi. It performs PAC/MMU/SPRR tests and stops at the known APSTS SPTM boundary. No default bootloader is installed.\n\nThe separate VMAPPLE experiment advances past PAC initialization and kernel pointer fixups, then stops at the bounded RAM window. It is not a valid complete VMAPPLE machine. See the package README and reports for exact remaining work.\n\nStandalone-UEFI-Preflight 0.0.3 and its old EFI are preserved. macOS27-Shell remains a separate hosted QEMU shell reference. No Sequoia EFI restored. Full original USB backup remains on Corsair.\n')
print(json.dumps(published,indent=2))
