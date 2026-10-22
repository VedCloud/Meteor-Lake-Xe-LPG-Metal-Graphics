from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';b=port/'backups/publication-keys-before-0.3.15'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.publish-tmp');shutil.copy2(src,tmp);tmp.replace(dst)
def inventory(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def verify(root,sums):
 for line in sums.read_text().splitlines():
  digest,name=line.split('  ',1);assert sha(root/name)==digest,name
assert u.is_mount() and not b.exists(),'USB missing or backup already exists'
m=json.loads((port/'SHA256SUMS.json').read_text())
for name,digest in m.items():assert sha(port/name)==digest,name
verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
assert sha(port/'Desktop/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver')=='3eb0fc04b45c0f03bc130d3ee152eb4c0dd5713ba30dd1196cabaa278adc8526'
assert sha(port/'Desktop/WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')==sha(p/'WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')
frozen=['WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU','WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB','Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel','WindowServerBackend/libvulkan_intel.dylib','WindowServerTransport/libdrm.2.dylib']
identity={name:sha(port/'Desktop'/name) for name in frozen};assert all(sha(p/name)==digest for name,digest in identity.items())
assert sha(p/'PreviousWindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')=='4ff96f638d9942e9a3a6b642b443b3a1ab257d88692aa51cf2efa2dde84c104f'
efi=inventory(u/'EFI')
changed=[]
files=sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
for f in files:
 name='Desktop/'+str(f.relative_to(p));dest=port/name
 if not dest.exists() or sha(dest)!=sha(f):changed.append(name)
for name in changed+['Desktop/SHA256SUMS.txt','collect-desktop-failure.sh','SHA256SUMS.json','PORT-STATUS.md']:
 src=port/name
 if src.exists():copy(src,b/name)
shutil.copytree(u/'EFI',b/'EFI')
(b/'BACKUP.json').write_text(json.dumps({'candidate':'0.3.14','basis_report':'failure-detail-20261020-172853','scope':'EFI and files replaced by 0.3.15; not an OS/AuxKC backup','prior_file_hashes':{name:sha(port/name) for name in changed if (port/name).is_file()},'new_paths_previously_absent':[name for name in changed if not (port/name).exists()],'frozen_runtime_hashes':identity,'efi_hashes':efi},indent=2)+'\n')
local=r/'backups/publication-keys-before-0.3.15';assert not local.exists();shutil.copytree(b,local)
for name in ['install-windowserver.sh','update-windowserver-kernel.sh']:subprocess.run(['/bin/bash','-n',str(p/name)],check=True)
subprocess.run(['/bin/bash','-n',str(r/'desktop-sources/collect-desktop-failure.sh')],check=True)
for name in ['WindowServerAccelerator/NMXAccel.kext','WindowServerFramebuffer/LatitudeMTLFB.kext','WindowServerPlugin/NVMTLDriver.bundle','latitude-frame-pacing']:subprocess.run(['codesign','--verify','--strict',str(p/name)],check=True)
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dest=port/'Desktop'/f.relative_to(p)
 if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
doc='publication-and-keys-0.3.15.md';copy(r/'research'/doc,port/doc)
status=port/'PORT-STATUS.md';status.write_text((r/'research'/doc).read_text()+'\n---\n\n'+status.read_text())
v={'candidate':'0.3.15','accelerator':'0.2.6','plugin':'0.3.15','framebuffer':'0.2.7 unchanged','gpu_kernel':'1.0.15 unchanged','accelerator_uuid':(p/'WindowServerAccelerator/NMXAccel-UUID.txt').read_text().strip(),'plugin_uuid':(p/'WindowServerPlugin/NVMTLDriver-UUID.txt').read_text().strip(),'tests':['three plugin variants and x86 accelerator compile/sign','3-slot publication helper ASan/UBSan;516 tight/padded/odd-width layouts','actual surface-out before-lock readback, failure cleanup and slot-saturation fallback','actual final repeated-plane publication marker','actual front timer/arm late-completion/incomplete/idle/retirement/legacy/disabled cases','existing readback/cache/quarantine/swizzle/event-completion fixtures','installer known previous0.2.5/current0.2.6 acceptance and rollback/prelink validation','collector failure retention and new fields','brightness AML compile, exact DSDT definition/checksum and retained references, official ocvalidate1.0.8'],'new_plugin_imports':[],'new_accelerator_imports':[],'missing_prior_exports':[],'full_hardware_suite_rerun':False,'early_completion_or_frame_drop':False,'dell_installation_or_arming_or_reboot':False,'dell_keys_verified':False,'dell_weather_fixed':False,'dell_wallpaper_fixed':False,'dell_timeouts_fixed':False,'efi_changes':['OC/ACPI/DSDT.aml definition/checksum','OC/ACPI/SSDT-LAT-BRT.aml','OC/config.plist one ACPI/Add entry'],'keyboard_touchpad_binaries_or_profiles_modified':False,'frozen_runtime_hashes':identity}
a=r/'port-artifacts/publication-keys-0.3.15';a.mkdir(exist_ok=True);(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'publication-keys-0.3.15-validation.json')
for f in files+[p/'SHA256SUMS.txt']:
 name='Desktop/'+str(f.relative_to(p));m[name]=sha(port/name)
for name in ['collect-desktop-failure.sh',doc,'publication-keys-0.3.15-validation.json','PORT-STATUS.md']:m[name]=sha(port/name)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt');verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name,digest in m.items():assert sha(port/name)==digest,name
assert identity=={name:sha(port/'Desktop'/name) for name in frozen}
# Install only the staged keys route, after preserving the complete prior EFI.
oldconfig=(u/'EFI/OC/config.plist').read_bytes();olddsdt=(u/'EFI/OC/ACPI/DSDT.aml').read_bytes()
assert sha(u/'EFI/OC/ACPI/DSDT.aml')=='42d65940f44de1482427a022f254e86c9f5c5b40b360418c4aefbc33facec063'
assert not (u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml').exists()
import plistlib
oc=plistlib.loads(oldconfig);nc=plistlib.loads((p/'BrightnessKeys/config.plist').read_bytes());entry=nc['ACPI']['Add'].pop();assert entry['Path']=='SSDT-LAT-BRT.aml' and entry['Enabled'] and nc==oc
try:
 copy(p/'BrightnessKeys/DSDT-brightness.aml',u/'EFI/OC/ACPI/DSDT.aml')
 copy(p/'BrightnessKeys/SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml')
 copy(p/'BrightnessKeys/config.plist',u/'EFI/OC/config.plist')
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 after=inventory(u/'EFI');changed={n for n in set(efi)|set(after) if efi.get(n)!=after.get(n)}
 assert changed=={'OC/ACPI/DSDT.aml','OC/ACPI/SSDT-LAT-BRT.aml','OC/config.plist'},changed
 assert sum((u/'EFI/OC/ACPI/DSDT.aml').read_bytes())%256==0
except BaseException:
 copy(b/'EFI/OC/ACPI/DSDT.aml',u/'EFI/OC/ACPI/DSDT.aml');copy(b/'EFI/OC/config.plist',u/'EFI/OC/config.plist')
 if (u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml').exists():(u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml').unlink()
 raise
v['efi_before_hashes']=efi;v['efi_after_hashes']=after
(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'publication-keys-0.3.15-validation.json')
m['publication-keys-0.3.15-validation.json']=sha(port/'publication-keys-0.3.15-validation.json');(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
print('Published0.3.15: plugin publication staging, accelerator completed-frame refresh, and brightness-key ACPI route. Old0.3.14 package files/full EFI backed up locally and on USB. Manifests/baseline/signatures/config verified; only3 intended EFI files changed. No Dell installation, arming, reboot or hardware suite.')
