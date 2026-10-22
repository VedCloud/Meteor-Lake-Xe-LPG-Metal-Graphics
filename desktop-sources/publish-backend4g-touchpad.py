from pathlib import Path
import hashlib,json,shutil,subprocess,plistlib
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';candidate=r/'touchpad-candidate';c=r/'checkpoints/usable-desktop-real-memory-0.2.5-20261019-232025';backup=port/'backups/backend4g-touchpad-before-20261020'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.publish-tmp');shutil.copy2(a,tmp);tmp.replace(b)
def verify(root,m):
 for line in m.read_text().splitlines():
  h,n=line.split('  ',1);assert sha(root/n)==h,n
verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
assert (u/'EFI/OC/config.plist').read_bytes()==(c/'EFI/OC/config.plist').read_bytes()
assert sha(u/'EFI/OC/ACPI/DSDT.aml')==sha(c/'EFI/OC/ACPI/DSDT.aml')
# Every pre-existing EFI payload must be the saved known-working one.
efi_hashes={str(f.relative_to(c/'EFI')):sha(f) for f in (c/'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
for n,h in efi_hashes.items():assert sha(u/'EFI'/n)==h,n
archive=port/'backups/usable-desktop-real-memory-0.2.5-20261019-232025.tar.gz';assert sha(archive)=='dc48db3f95fc1e2096e3661068d490660a13c94347bdc175b099327941ffc7c1'
fixed=['WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU','PreviousWindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU','WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB','Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU','Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel','WindowServerTransport/libdrm.2.dylib','Library/GPUBundles/nvmtl/libvulkan_intel.dylib','Library/GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver','Library/NullMoth/latitude-gpu-identity']
retained={n:sha(port/'Desktop'/n) for n in fixed}
for n,h in retained.items():assert sha(p/n)==h,n
for n in ['EFI','iGPU-Port/Desktop/WindowServerPlugin','iGPU-Port/Desktop/install-windowserver.sh','iGPU-Port/Desktop/Library/NullMoth/load-panel-framebuffer.sh','iGPU-Port/Desktop/BUILD-MANIFEST.json','iGPU-Port/Desktop/README.md','iGPU-Port/Desktop/SHA256SUMS.txt','iGPU-Port/collect-desktop-failure.sh','iGPU-Port/PORT-STATUS.md','iGPU-Port/SHA256SUMS.json']:
 a=u/n;b=backup/n
 if b.exists():continue
 if a.is_dir():shutil.copytree(a,b,ignore=shutil.ignore_patterns('._*'))
 else:copy(a,b)
for f in [p/'WindowServerBackend/libvulkan_intel.dylib',p/'WindowServerPlugin/NVMTLDriver.bundle',candidate/'LatitudeI2CMeteorLake.kext']:subprocess.run(['codesign','--verify','--strict',str(f)],check=True)
for f in [p/'install-windowserver.sh',p/'Library/NullMoth/load-panel-framebuffer.sh',r/'desktop-sources/collect-desktop-failure.sh']:subprocess.run(['/bin/bash','-n',str(f)],check=True)
subprocess.run(['plutil','-lint',str(candidate/'config.plist')],check=True)
gpu=json.loads((r/'port-artifacts/backend4g-plugin0.3.7/VALIDATION.json').read_text());input_config=json.loads((candidate/'CONFIG-VALIDATION.json').read_text());acpi=json.loads((r/'port-artifacts/touchpad-mtl-1.0.0/ACPI-VALIDATION.json').read_text())
validation={'baseline_checkpoint':c.name,'gpu':gpu,'touchpad':{'controller':'8086:7e7b subsystem1028:0cb6 PCI00:15.3','device':'ELAN04f3:328e, ACPI _SB.PC00.I2C3.TPD0','extension':'LatitudeI2CMeteorLake1.0.0','sha256':sha(candidate/'LatitudeI2CMeteorLake.kext/Contents/MacOS/LatitudeI2CMeteorLake'),'upstream':'https://github.com/watchthelight-labs/LegionI2CArrowLake','upstream_revision':'15ebc45d2e5aa63eebaff01a736acd402a7c3de1','adaptation':'Current USB binary lacks upstream PCILakeController; tiny subclass links to existing VoodooI2CPCIController, discovers PM capability and verifies D0; exact Dell ID/subsystem and Linux bxt timing; polling scoped to PCI15.3','acpi':acpi,'config':input_config,'checks':['x86_64 extension build/link/signature pass;9 Voodoo base imports resolve in current core','actual power helper65535 PMCSR words/capability bounds/device checks/stuck refusal ASan/UBSan pass','actual SSDT executed in ACPICA: HID address/support bits/resource bytes pass','DSDT decompile/checksum/length validation; only3 byte offsets changed:2 method initial letters+checksum'],'hardware_gestures_verified':False,'PS2_fallback_retained':True},'unchanged_gpu_and_runtime_payloads':retained,'hardware_trial_required':True,'full_gpu_suite_rerun':False}
(v:=r/'port-artifacts/backend4g-plugin0.3.7')/'VALIDATION.json'
(v/'COMBINED-VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n')
for f in (r/'touchpad-sources').glob('*'):
 if f.is_file():copy(f,r/'port-artifacts/touchpad-mtl-1.0.0/source'/f.name)
for f in ['CONFIG-VALIDATION.json','Legion-LICENSE','VoodooI2C-LICENSE']:copy(candidate/f,r/'port-artifacts/touchpad-mtl-1.0.0'/f)
doc=r/'research/backend4g-touchpad-20261020.md';doc.write_text('''# 20 October candidate: backend 4GiB, pipeline cache and Dell touchpad

Baseline is the verified usable-desktop-real-memory-0.2.5-20261019-232025 checkpoint. Its archive is preserved, not replaced by this unverified candidate. GPU kernel1.0.15, framebuffer0.2.5, accelerator0.2.4, IOKit transport and guarded HID1.0.1 stay unchanged.

## Backend limit and animation investigation

The user authorized a2GiB or4GiB backend limit;4GiB is selected as the first candidate. On Darwin integrated Intel8086:7d45, ANV's actual shared system heap is min(normal queried/policy size,4GiB). This is applied before creation of VkMemoryHeap records, not in the UI. Existing ANV memory budgets clamp to that heap and allocation checks use the same size/current accounted usage, including a raw-size check that prevents integer-wrap bypass of the aligned-size guard. The limit is per ANV physical-device heap/accounting domain, not a new kernel-wide aggregate quota or reserved4GiB physical allocation. Smaller actual heaps stay smaller. Other devices/platforms/local-memory heaps retain their policy. No environment override can raise this target's cap. GPU name remains queried automatically, and the existing root reporter should publish4096MB only if the backend actually exposes4GiB. Hardware query/allocator behavior is not yet validated on the Dell.

Yesterday's sample included pipeline/shader compilation during WindowServer composition. Plugin0.3.7 creates one device-lifetime Vulkan pipeline cache for this integrated GPU in WindowServer, using normal internally synchronized flags, and passes it to graphics pipeline creation. If unavailable/creation fails, the prior uncached path remains. No cache data is persisted to disk. It does not recycle command pools, change fresh-pool policy, relax barriers, alter submission timing or implement zero-copy. This may reduce repeated compilation; it does not establish a cure for the42 CoreAnimation synchronization/fence timeout lines or steady-state choppiness. The loader now captures both early and later stack samples so the next report can separate startup from persistent costs. Frame timings/FPS remain unmeasured.

## Touchpad port

Linux identifies the Dell touchpad as ELAN04F3:328E on00:15.3, Intel8086:7E7B, subsystem1028:0CB6, ACPI\\_SB.PC00.I2C3.TPD0. Existing USB adds only I2C0 touchscreen matching. The Legion project supplies Arrow Lake controller matching/timing/power handling; it is not a gesture engine. Linux maps the Dell's7E7B to the same bxt I2C timing:133MHz, SDA hold42ns, SDA fall171ns, SCL fall208ns. Those are used for this exact Dell controller.

The installed VoodooI2C2.9.1 binary lacks the Legion personality's PCILakeController class, so blindly copying its codeless kext would fail. LatitudeI2CMeteorLake1.0.0 supplies a small subclass of the existing exported PCI controller. It restricts vendor/device/subsystem, discovers the PCI PM capability instead of assuming its offset, requests and verifies D0, preserves PME status semantics and refuses a nonresponsive controller before base probe. Nine required Voodoo base symbols resolve in the existing core. Existing VoodooI2C/HID/VoodooInput binaries and the HID null guard remain intact; only one VoodooInput stays enabled.

Captured macOS logs also show TPD0._DSM fails with missing HIDG. The replacement methods bypass the absent shared helper: the HID DSM returns firmware TPDH; _CRS concatenates the device's own firmware SBFB serial-bus and SBFI interrupt buffers. No slave address, descriptor register or GPIO pin is guessed. The paired DSDT modifies only the initial byte of this single TPD0's _DSM and _CRS names (XDSM/XCRS) plus table checksum; length is unchanged and it decompiles. A small SSDT supplies the new methods. _STA/_INI and touchscreen methods are untouched. Polling is enabled only on PCI15.3 through DeviceProperties, avoiding an unverified Meteor Lake GPIO interrupt path. No global polling boot argument is added. Existing PS/2 fallback input and keyboard are retained for the first trial; if duplicate pointer events occur, use the report to decide whether the legacy mouse should be disabled.

The new controller should allow the existing precision-touchpad HID and VoodooInput gesture stack to attach. Multi-finger gestures have NOT yet been tested on this laptop. The collector captures the controller/HID/precision-touchpad/VoodooInput registry chain and exact backend hash. Gesture settings or further HID/ACPI issues may still need work; no claim that installing this port alone proves all gestures.

## Local validation and recovery

ANV cross-build/link and3 plugin variants pass; backend imports/exports are identical to baseline, plugin prior exports remain and no new external imports appear. ASan/UBSan checks run the actual heap policy/allocation guard, pipeline cache helpers and controller power helper (65535 PMCSR words). ACPICA executes the actual SSDT against simulated firmware objects and verifies descriptor/support/resource bytes. Installer fixtures cover backend replacement and rollback on late signature/arming/KC failures; loader fixtures retain active/fallback behavior and isolate reporting failure. Collector fixtures preserve crash/panic data. These are local checks, not a full hardware rendering suite rerun.

USB touchpad rollback: /bin/bash /Volumes/SEQOUIA/iGPU-Port/restore-touchpad.sh from a Mac with USB mounted. It verifies the candidate config/DSDT before restoring the saved pre-trial config/DSDT and removing only the added touchpad bundle/SSDT. It refuses to overwrite a subsequently changed config. This restores USB input configuration; it does not revert an installed graphics backend. Desktop installer retains prior plugin/backend/kernel/cache/helper copies in its normal target backup directory and rolls back on installation failure.

## Dell trial

Boot through the updated USB to the fallback desktop, then run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, restart USB once without ResetNVRAM. Let the usable desktop settle for about a minute, check two-finger scrolling/pinch and three/four-finger gestures, then collect:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

If the graphics trial fails, power off and boot USB fallback, then collect before reinstalling. Bring the USB back. No22-case suite required. No target installation, live NVRAM changes or automatic reboot were performed on this build host.

## Sources

- [LegionI2CArrowLake](https://github.com/watchthelight-labs/LegionI2CArrowLake), pinned15ebc45d2e5aa63eebaff01a736acd402a7c3de1. Its ISC license and VoodooI2C's GPL license are preserved with the port. The new subclass is a local Dell adaptation, not the original codeless Arrow Lake package.
- [Linux LPSS PCI IDs and timing](https://github.com/torvalds/linux/blob/master/drivers/mfd/intel-lpss-pci.c).
- [VoodooI2C source](https://github.com/VoodooI2C/VoodooI2C), local pinned2.9.1 headers/controller/resource parser.
- [Vulkan pipeline caches](https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineCache.html) and [graphics pipeline synchronization](https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateGraphicsPipelines.html).
- [Vulkan memory properties](https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceMemoryProperties.html). ANV local source heap creation, budget calculation and allocation checks were audited and changed locally; no upstream community submission was made.
''')
copy(doc,v/'RESEARCH.md');copy(doc,r/'port-artifacts/touchpad-mtl-1.0.0/RESEARCH.md')
intro='# Current candidate: backend4GiB + pipeline cache0.3.7 + Dell I2C3 touchpad\n\nBased on verified October19 checkpoint. Backend enforces4GiB shared heap on Darwin integrated8086:7d45; existing reporter reads its actual limit. Device-lifetime graphics pipeline cache targets repeated compilation; fresh command pools remain. Dell7e7b controller power/matching and scoped TPD0 ACPI fixes prepare multitouch attachment. Local checks pass; target smoothness/gestures/cap need trial confirmation. GPU1.0.15/FB0.2.5/accelerator/transport/HID guard retained. See ../backend4g-touchpad-20261020.md.\n\nBoot USB fallback; run install-windowserver.sh; exit0 then restart USB once without ResetNVRAM. Let desktop settle about1minute, check gestures, collect-desktop-failure.sh. No22-case suite. restore-touchpad.sh reverts only USB input changes. Latest verified checkpoint archive preserved. Historical instructions below are superseded.\n\n---\n\n'
f=p/'README.md'
if not f.read_text().startswith('# Current candidate: backend4GiB'):f.write_text(intro+f.read_text())
f=p/'BUILD-MANIFEST.json';m=json.loads(f.read_text());m['stage']='backend4GiB/plugin0.3.7 and Dell I2C3 touchpad candidate; target verification pending';m['backend4g_and_touchpad']=validation;f.write_text(json.dumps(m,indent=2)+'\n')
files=sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dst=port/'Desktop'/f.relative_to(p)
 if not dst.exists() or sha(dst)!=sha(f):copy(f,dst)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh');copy(doc,port/doc.name);copy(v/'COMBINED-VALIDATION.json',port/'backend4g-touchpad-validation.json')
# Prepare all new EFI files first; switch the validated config last.
shutil.copytree(candidate/'LatitudeI2CMeteorLake.kext',u/'EFI/OC/Kexts/LatitudeI2CMeteorLake.kext',dirs_exist_ok=True)
copy(candidate/'ACPI/SSDT-LAT-TPD0.aml',u/'EFI/OC/ACPI/SSDT-LAT-TPD0.aml');copy(candidate/'ACPI/DSDT.aml',u/'EFI/OC/ACPI/DSDT.aml');copy(candidate/'config.plist',u/'EFI/OC/config.plist')
# Guarded input rollback refuses to clobber any later user config/DSDT edit.
restore=port/'restore-touchpad.sh';cfg_sha=sha(u/'EFI/OC/config.plist');dsdt_sha=sha(u/'EFI/OC/ACPI/DSDT.aml')
restore.write_text('''#!/bin/bash
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
EFI="$HERE/../EFI/OC"
SAVE="$HERE/backups/backend4g-touchpad-before-20261020/EFI/OC"
check() { [ "$(shasum -a256 "$1" | awk '{print $1}')" = "$2" ]; }
check "$EFI/config.plist" CFG_SHA || { echo 'EFI config changed since this trial; refusing to overwrite it.'; exit 2; }
check "$EFI/ACPI/DSDT.aml" DSDT_SHA || { echo 'DSDT changed since this trial; refusing to overwrite it.'; exit 2; }
check "$SAVE/config.plist" OLD_CFG || { echo 'Saved config verification failed.'; exit 2; }
check "$SAVE/ACPI/DSDT.aml" OLD_DSDT || { echo 'Saved DSDT verification failed.'; exit 2; }
cp -p "$SAVE/ACPI/DSDT.aml" "$EFI/ACPI/DSDT.aml.restore-tmp"
cp -p "$SAVE/config.plist" "$EFI/config.plist.restore-tmp"
mv -f "$EFI/ACPI/DSDT.aml.restore-tmp" "$EFI/ACPI/DSDT.aml"
mv -f "$EFI/config.plist.restore-tmp" "$EFI/config.plist"
rm -f "$EFI/ACPI/SSDT-LAT-TPD0.aml"
rm -rf "$EFI/Kexts/LatitudeI2CMeteorLake.kext"
echo 'Previous USB touchpad configuration restored; graphics runtime installation is unchanged. No reboot performed.'
'''.replace('CFG_SHA',cfg_sha).replace('DSDT_SHA',dsdt_sha).replace('OLD_CFG',sha(backup/'EFI/OC/config.plist')).replace('OLD_DSDT',sha(backup/'EFI/OC/ACPI/DSDT.aml')))
restore.chmod(0o755)
for n in ['Legion-LICENSE','VoodooI2C-LICENSE','CONFIG-VALIDATION.json']:copy(candidate/n,port/'Touchpad'/n)
copy(r/'port-artifacts/touchpad-mtl-1.0.0/ACPI-VALIDATION.json',port/'Touchpad/ACPI-VALIDATION.json')
# Provide source and notices with the adapted extension.
for f in (r/'touchpad-sources').glob('*'):
 if f.is_file():copy(f,port/'Touchpad/source'/f.name)
f=port/'PORT-STATUS.md'
if not f.read_text().startswith('# Current candidate: backend4GiB'):f.write_text(intro.replace('../','')+f.read_text())
f=port/'SHA256SUMS.json';m=json.loads(f.read_text())
new=['collect-desktop-failure.sh',doc.name,'backend4g-touchpad-validation.json','restore-touchpad.sh','PORT-STATUS.md']+[str(f.relative_to(port)) for f in (port/'Touchpad').rglob('*') if f.is_file() and not f.name.startswith('._')]
for n in new:m[n]=sha(port/n)
for n in m:
 if n.startswith('Desktop/'):m[n]=sha(port/n)
f.write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt');verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for n,h in m.items():assert sha(port/n)==h,n
for n,h in retained.items():assert sha(port/'Desktop'/n)==h,n
for n,h in efi_hashes.items():
 if n not in ['OC/config.plist','OC/ACPI/DSDT.aml']:assert sha(u/'EFI'/n)==h,n
assert sha(u/'EFI/OC/config.plist')==input_config['config_sha256'];assert sha(u/'EFI/OC/ACPI/DSDT.aml')==acpi['patched_sha256']
subprocess.run(['plutil','-lint',str(u/'EFI/OC/config.plist')],check=True)
subprocess.run(['codesign','--verify','--strict',str(u/'EFI/OC/Kexts/LatitudeI2CMeteorLake.kext')],check=True)
subprocess.run(['/bin/bash','-n',str(restore)],check=True)
print('Published',len(files),'desktop package files plus scoped USB touchpad extension/ACPI/config. All manifests, signatures, syntax, frozen GPU evidence and retained payloads verified. Saved latest checkpoint preserved; no target install, NVRAM mutation, GPU suite or reboot.')
