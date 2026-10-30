# SPDX-License-Identifier: MIT
"""Publish a separately versioned, verified kernel experiment, never a boot claim."""
from pathlib import Path
import json,hashlib,subprocess,shutil,tarfile,plistlib
r=Path(__file__).resolve().parent;w=r.parent
research=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research');version='0.1.1'
stage=research/('Standalone-Engine-'+version)
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for data in iter(lambda:f.read(1024*1024),b''):h.update(data)
 return h.hexdigest()
def info(p):return plistlib.loads(subprocess.check_output(['diskutil','info','-plist',str(p)]))
valid=json.loads((r/'EXFAT-FIRMWARE-VALIDATION.json').read_text());admit=json.loads((r/'ADMISSION-VALIDATION.json').read_text());build=json.loads((r/'BUILD.json').read_text());digest=sha(r/'UnicornTest.efi')
assert valid['EFI_SHA256']==admit['EFI_SHA256']==build['EFI_SHA256']==digest
assert valid['OVMFCases']=={'valid':'PASS'} and admit['OVMFCases']=={'vm-tampered':'PASS'}
assert valid['VMAPPLEReplay']['darwinBannerObserved'] and valid['VMAPPLEReplay']['pmapStartupCompleted']
assert not valid['kernelBoot'] and not valid['DellHardware']
for name,h in build['sourceSHA256'].items():assert sha(r/name)==h,name
assert sha(w/'InstallerInputs/exfat_x64.efi')=='3f16a8b72ea81a6e7f9dea0fe92a108cd9b980e29c21f058ddb2f2730027ec81'
esp=Path('/Volumes/EFI');main=Path('/Volumes/macOS 27');a,b=info(esp),info(main)
assert a['VolumeUUID']=='6BC2C2D9-547C-33F0-BC92-5ABB1B3B123A' and b['VolumeUUID']=='7E12226F-FA27-3551-9825-9A04E2154289'
assert a['ParentWholeDisk']==b['ParentWholeDisk'] and a['BusProtocol']==b['BusProtocol']=='USB' and not a['Internal'] and not b['Internal']
assert a['FreeSpace']>(r/'UnicornTest.efi').stat().st_size+2*1024*1024
old=esp/'EFI/Tools/Dell27UnicornEngine.efi';oldhash=sha(old)
assert oldhash=='3415cc5b2ff5ad7f435560af5751e249255c836bc0204d21098b43e9ef969b22'
assert not stage.exists(), 'published versions are immutable'
stage.mkdir()
for sub in ['Evidence','Inputs','Source','Drivers']:(stage/sub).mkdir()
shutil.copy2(r/'UnicornTest.efi',stage/'Dell27VMApple-0.1.1.efi')
for name in ['reference-boot-state.bin','vmapple-reference-boot-state.bin']:shutil.copy2(r/name,stage/'Inputs'/name)
for name in ['BUILD.json','COMPILE.json','EXFAT-FIRMWARE-VALIDATION.json','ADMISSION-VALIDATION.json','MEMORY-PROFILE.json','VMAPPLE-HANDOFF.json','VMAPPLE-REFERENCE-BOOT.json','ovmf-sparse-valid.log','ovmf-sparse-vm-tampered.log','ovmf-watchdog-reset.log']:
 shutil.copy2(r/name,stage/'Evidence'/name)
shutil.copy2(w/'InstallerInputs/exfat_x64.efi',stage/'Drivers/exfat_x64.efi');shutil.copy2(w/'efifs-reference/LICENSE',stage/'Drivers/LICENSE-EfiFs.txt')
u=w/'unicorn-for-efi';patch=subprocess.check_output(['git','diff','--binary'],cwd=u)
for name in subprocess.check_output(['git','ls-files','--others','--exclude-standard'],cwd=u,text=True).splitlines():
 assert name in ['qemu/target/arm/vmapple-pac.c.inc','qemu/target/arm/monitor-gic.c.inc'],name
 p=subprocess.run(['git','diff','--no-index','--','/dev/null',name],cwd=u,capture_output=True);assert p.returncode==1;patch+=p.stdout
(stage/'Source/unicorn-firmware-fixes.patch').write_bytes(patch)
with tarfile.open(stage/'Source/engine-sources.tar.gz','w:gz',compresslevel=6) as tar:
 for directory in ['unicorn-for-efi','edk2','nanoprintf','efifs-reference','exfat-tools']:
  root=w/directory
  for f in sorted(root.rglob('*')):
   rel=f.relative_to(w)
   if not f.is_file() or '.git' in rel.parts or '__pycache__' in rel.parts:continue
   if f.suffix in ['.o','.obj','.a','.pyc'] or '/Source/C/bin/' in str(rel) or f.name=='mkexfatfs':continue
   tar.add(f,arcname=str(rel),recursive=False)
 for f in sorted(r.iterdir()):
  if f.is_file() and f.name!='README.md' and (f.suffix in ['.c','.h','.py','.S','.inc','.md']):tar.add(f,arcname='UnicornBuild/'+f.name,recursive=False)
 for f in sorted((w/'Standalone').rglob('*')):
  if f.is_file() and 'VirtualUSB' not in f.parts and '__pycache__' not in f.parts and f.suffix in ['.c','.h','.py','.S','.ld','.md']:tar.add(f,arcname='Standalone/'+str(f.relative_to(w/'Standalone')),recursive=False)
provenance={'EfiFs':{'URL':'https://github.com/pbatard/EfiFs/releases/tag/v1.13','revision':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'efifs-reference',text=True).strip(),'submodules':subprocess.check_output(['git','submodule','status'],cwd=w/'efifs-reference',text=True),'SHA256':sha(stage/'Drivers/exfat_x64.efi')},'XNU':{'URL':'https://github.com/apple-oss-distributions/xnu','revision':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'xnu-reference',text=True).strip(),'optionalCTRRReference':'osfmk/arm64/platform_tests.c'},'DITReference':{'URL':'https://raw.githubusercontent.com/qemu/qemu/master/target/arm/helper.c','localSHA256':sha(w/'InstallerInputs/References/qemu-arm-helper.c')}}
provenance['ReferenceCapture']={'darwin-vm':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'darwin-vm',text=True).strip(),'qemu-sptm':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'darwin-vm/qemu-sptm',text=True).strip(),'macOSBuild':'26A428','kernel':'RELEASE_ARM64_VMAPPLE'}
provenance['Toolchain']={'clang':subprocess.check_output(['/Library/Developer/CommandLineTools/usr/bin/clang','--version'],text=True),'lld':subprocess.check_output([str(w/'deps/bin/ld.lld'),'--version'],text=True),'edk2':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'edk2',text=True).strip()}
(stage/'Evidence/SOURCES.json').write_text(json.dumps(provenance,indent=2)+'\n')
status={'version':version,'EFI_SHA256':digest,'runtime':'bare-metal x86-64 UEFI ARM64 software execution','logicalRAMBytes':17179869184,'hostAllocation':'adaptive ceiling from conventional UEFI RAM; half retained plus 128 MiB reserve','kernel':valid['VMAPPLEReplay'],'kernelBootComplete':False,'DellHardwareTested':False,'PVCTRRProtection':False,'SPTMPhysicalBoot':False,'firmwareWatchdogDisabled':True,'exFATInputRead':'PASS in x86 OVMF through verified EfiFs 1.13 driver','corruptInputRejected':'PASS before kernel execution','remaining':['Full kernel boot to root and launchd','Additional VMAPPLE devices requested during later initialization','Display/input/storage runtime connections','Dell hardware validation']}
(stage/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n')
readme=f'''# Dell27 standalone kernel experiment {version}

This is an x86-64 UEFI program that executes ARM64 software without a Linux or Sequoia host. It is not a working macOS installer or desktop. Testing is in x86 OVMF; the Dell has not been tested.

The VMAPPLE 27.0/26A428 kernel prints Darwin 27, completes pmap startup and advances into later initialization. The exact stop is recorded in Evidence/EXFAT-FIRMWARE-VALIDATION.json and its complete serial log. This candidate declares 16 GiB of guest RAM, backs physical RAM on demand, includes all 455,000,064 initialized bytes (kernel, DT, boot arguments, ramdisk and trust cache), and derives its host allocation ceiling from the UEFI memory map. It cannot dedicate all physical RAM to the guest while the runtime also needs memory.

The default five-minute UEFI watchdog is disabled through the real firmware service, and the test rejects unexpected resets.

Implemented and exercised: one-vCPU GICv3 interrupt state and FIQ delivery, virtual timer PPI 27, architectural guest exceptions and ERET, PL011 console, range TLB invalidation, DIT state and exception preservation, PAC services and system MMU permissions. DIT state emulation is not a host side-channel timing guarantee. Unsupported accesses stop explicitly. PV CTRR is not advertised: XNU's optional compatibility path is used; no CTRR memory-protection enforcement or physical SPTM boot is claimed.

Both the input's initial admission and the bytes loaded into guest RAM are SHA-256 checked before execution. The full input can reside on the main exFAT partition. The engine verifies the pinned EfiFs driver before loading it, connects filesystem controllers, and searches mounted EFI filesystems for the input.

## Files on the USB

Firmware Boot from File: `EFI\\Tools\\Dell27VMApple-0.1.1.efi` on the FAT partition. Its exFAT driver is `EFI\\Drivers\\exfat_x64.efi`. Its full input is `EngineProbe\\vmapple-reference-boot-state.bin` on the macOS 27 partition. The older SPTM diagnostic input remains on the FAT partition. No default BOOTX64 loader is installed; the partition's automatic removable-boot discovery remains unqualified.

This diagnostic first runs its firmware/CPU tests and then the VMAPPLE experiment. It will stop at the recorded limitation; do not interpret returning success from the test harness as a successful macOS boot. The 0.1.0 diagnostic and the original Sequoia backups remain preserved.

## Reproduce

`python3 compile.py`, `python3 link.py --vmapple`, then `python3 test-sparse-firmware.py` run the local firmware tests. The separately prepared disposable `test-inputs.raw` exFAT image is used by `--exfat`; `--tampered-only` validates corrupted capture rejection. Build paths are rooted at /Volumes/Dell27Build. BUILD.json binds the EFI to integration sources. Original GPL/LGPL/BSD/MIT notices remain in the complete source archive; EfiFs/GRUB sources and pinned provenance are included.
'''
(stage/'README.md').write_text(readme)
manifest={str(f.relative_to(stage)):sha(f) for f in sorted(stage.rglob('*')) if f.is_file()}
(stage/'SHA256.json').write_text(json.dumps(manifest,indent=2)+'\n')
usb=main/stage.name;assert not usb.exists();shutil.copytree(stage,usb)
for name,h in manifest.items():assert sha(usb/name)==h,name
(esp/'EFI/Drivers').mkdir(exist_ok=True);shutil.copy2(stage/'Drivers/exfat_x64.efi',esp/'EFI/Drivers/exfat_x64.efi');efi=esp/'EFI/Tools/Dell27VMApple-0.1.1.efi';shutil.copy2(r/'UnicornTest.efi',efi)
(main/'EngineProbe').mkdir(exist_ok=True);inputfile=main/'EngineProbe/vmapple-reference-boot-state.bin';shutil.copy2(r/'vmapple-reference-boot-state.bin',inputfile)
assert sha(inputfile)==sha(r/'vmapple-reference-boot-state.bin') and sha(efi)==digest and sha(old)==oldhash
assert sha(esp/'EFI/Drivers/exfat_x64.efi')==provenance['EfiFs']['SHA256']
published={'version':version,'package':str(usb),'EFI':str(efi),'EFI_SHA256':digest,'input':str(inputfile),'inputSHA256':sha(inputfile),'verifiedPackageFiles':len(manifest),'defaultBootloaderInstalled':False,'kernelBootComplete':False,'DellHardwareTested':False}
(research/'ENGINE-USB-PUBLISHED.json').write_text(json.dumps(published,indent=2)+'\n');(r/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n');(r/'README.md').write_text(readme)
(main/'READ-ME-FIRST.txt').write_text('Latest standalone candidate: 0.1.1. Read Standalone-Engine-0.1.1/README.md. Darwin kernel initialization is demonstrated in OVMF, not a completed macOS boot. Manual firmware file: EFI\\Tools\\Dell27VMApple-0.1.1.efi. Earlier diagnostic preserved.\n')
print(json.dumps(published,indent=2))
