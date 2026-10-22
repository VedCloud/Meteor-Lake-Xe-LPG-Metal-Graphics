from pathlib import Path
import shutil,json,hashlib,plistlib,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def files(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def copy(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);t=b.with_name(b.name+'.new');shutil.copy2(a,t);t.replace(b)
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
before=files(u/'EFI');b=r/'backups/native-keys-publication-before-0.3.17';assert not b.exists();b.mkdir()
shutil.copytree(u/'EFI',b/'EFI')
changed=['WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver','WindowServerPlugin/NVMTLDriver.bundle/Contents/Info.plist','WindowServerPlugin/NVMTLDriver.bundle/Contents/_CodeSignature/CodeResources','WindowServerPlugin/NVMTLDriver-UUID.txt','BrightnessKeys/SSDT-LAT-BRT.aml','BrightnessKeys/SSDT-LAT-BRT.dsl','install-windowserver.sh','BUILD-MANIFEST.json','README.md','SHA256SUMS.txt']
for n in changed:
 f=port/'Desktop'/n
 if f.is_file():copy(f,b/'Desktop'/n)
copy(port/'collect-desktop-failure.sh',b/'collect-desktop-failure.sh');copy(port/'SHA256SUMS.json',b/'SHA256SUMS.json')
usbbackup=port/'backups'/b.name;assert not usbbackup.exists();shutil.copytree(b,usbbackup)
bundle=p/'WindowServerPlugin/NVMTLDriver.bundle';copy(r/'desktop-build/NVMTLDriver-0.3.17',bundle/'Contents/MacOS/NVMTLDriver')
f=bundle/'Contents/Info.plist';info=plistlib.loads(f.read_bytes());info['CFBundleVersion']=info['CFBundleShortVersionString']='0.3.17';f.write_bytes(plistlib.dumps(info))
subprocess.run(['codesign','--force','-s','-',str(bundle)],check=True);subprocess.run(['codesign','--verify','--strict',str(bundle)],check=True)
uuid=subprocess.check_output(['xcrun','dwarfdump','--uuid',str(bundle/'Contents/MacOS/NVMTLDriver')],text=True).split()[1];(p/'WindowServerPlugin/NVMTLDriver-UUID.txt').write_text(uuid+'\n')
copy(r/'desktop-build/native-brightness-test.aml',p/'BrightnessKeys/SSDT-LAT-BRT.aml');copy(r/'desktop-sources/SSDT-LAT-BRT.dsl',p/'BrightnessKeys/SSDT-LAT-BRT.dsl')
f=p/'install-windowserver.sh';s=f.read_text().replace('Metal plugin0.3.15: three bounded publication staging slots shorten IOSurface lock duration; GPU fences and successful-copy publication remain mandatory. Targeted layout/timing diagnostics included; Weather and timeout fixes require target validation.','Metal plugin0.3.17: bounded publication/layout logs retained in release; monitor wait measured separately. Rendering synchronization preserved. Native panel brightness keys require target validation; Night Shift and timeout fixes remain under investigation.').replace('Graphics candidate 0.3.16','Graphics candidate 0.3.17');f.write_text(s)
subprocess.run(['/bin/bash','-n',str(f)],check=True)
f=p/'BUILD-MANIFEST.json';meta=json.loads(f.read_text());meta['stage']='candidate0.3.17: native panel brightness route and retained publication diagnostics; Night Shift/remaining timeouts unresolved';meta['built']['metal_plugin']='NVMTLDriver0.3.17: retained bounded publication/layout release logs and separate monitor wait; rendering policy unchanged';meta['candidate_0_3_17']={'plugin':'0.3.17','uuid':uuid,'brightness_route':'LCD 0x86 up / 0x87 down via existing BrightnessKeys','audio_output_user_confirmed':True,'slider_brightness_user_confirmed':True,'physical_keys_verified':False,'nightshift_fixed':False,'coreanimation_timeouts_fixed':False,'full_gpu_suite_rerun':False};f.write_text(json.dumps(meta,indent=2)+'\n')
copy(r/'research/native-keys-publication-0.3.17.md',p/'README.md')
package_files=[f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._')]
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in sorted(package_files)))
for f in package_files+[p/'SHA256SUMS.txt']:
 dest=port/'Desktop'/f.relative_to(p)
 if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
copy(r/'research/native-keys-publication-0.3.17.md',port/'native-keys-publication-0.3.17.md')
# Only the native brightness route changes in EFI; preserve configuration and audio.
copy(p/'BrightnessKeys/SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml')
after=files(u/'EFI');assert {n for n in set(before)|set(after) if before.get(n)!=after.get(n)}=={'OC/ACPI/SSDT-LAT-BRT.aml'}
subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
for f in package_files+[p/'SHA256SUMS.txt']:
 n='Desktop/'+str(f.relative_to(p));m[n]=sha(port/n)
for n in ['collect-desktop-failure.sh','native-keys-publication-0.3.17.md']:m[n]=sha(port/n)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
for n,h in m.items():assert sha(port/n)==h,n
for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
 h,n=line.split(None,1);assert sha(port/n.strip())==h,n
v={'candidate':'0.3.17','plugin_uuid':uuid,'plugin_sha256':sha(bundle/'Contents/MacOS/NVMTLDriver'),'efi_changes':['OC/ACPI/SSDT-LAT-BRT.aml'],'efi_before':before,'efi_after':after,'tests':['release logger ASan/UBSan','actual surface publication and event completion','native brightness route upstream listener/AML checksum','collector fixture','guarded installer regression'],'nightshift_fixed':False,'remaining_timeouts_fixed':False,'target_hardware_validation_pending':True,'full_gpu_suite_rerun':False}
a=r/'port-artifacts/native-keys-publication-0.3.17';a.mkdir(exist_ok=True);(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n')
print('Published0.3.17; root/package/frozen-baseline hashes verified; only native brightness SSDT changed in EFI. Backup',usbbackup,'Plugin UUID',uuid)
