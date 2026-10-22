from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;port=Path('/Volumes/SEQOUIA/iGPU-Port');efi=port.parent/'EFI/OC';v=r/'port-artifacts/touchpad-inline-1.0.1';backup=port/'backups/touchpad-inline-before-20261020'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.publish-tmp');shutil.copy2(a,tmp);tmp.replace(b)
old_cfg=sha(efi/'config.plist');old_dsdt=sha(efi/'ACPI/DSDT.aml');old=json.loads((port/'backend4g-touchpad-validation.json').read_text());assert old_cfg==old['touchpad']['config']['config_sha256'];assert old_dsdt==old['touchpad']['acpi']['patched_sha256']
for src,name in [(efi/'config.plist','config.plist'),(efi/'ACPI/DSDT.aml','DSDT.aml'),(efi/'ACPI/SSDT-LAT-TPD0.aml','SSDT-LAT-TPD0.aml'),(port/'restore-touchpad.sh','restore-touchpad.sh'),(port/'collect-desktop-failure.sh','collect-desktop-failure.sh')]:
 if not (backup/name).exists():copy(src,backup/name)
kept={str(f.relative_to(efi)):sha(f) for f in efi.rglob('*') if f.is_file() and not f.name.startswith('._') and str(f.relative_to(efi)) not in ['config.plist','ACPI/DSDT.aml','ACPI/SSDT-LAT-TPD0.aml']}
copy(v/'DSDT.aml',efi/'ACPI/DSDT.aml');copy(v/'config.plist',efi/'config.plist');(efi/'ACPI/SSDT-LAT-TPD0.aml').unlink()
f=port/'restore-touchpad.sh';s=f.read_text().replace(old_cfg,sha(efi/'config.plist')).replace(old_dsdt,sha(efi/'ACPI/DSDT.aml'));f.write_text(s);subprocess.run(['/bin/bash','-n',str(f)],check=True)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh');subprocess.run(['/bin/bash','-n',str(port/'collect-desktop-failure.sh')],check=True)
for name in ['aml_methods.py','prepare-inline-acpi.py','test-inline-acpi.py']:copy(r/'touchpad-sources'/name,port/'Touchpad/source'/name)
validation=json.loads((v/'VALIDATION.json').read_text());validation['exact_inline_body_ACPICA_test_passed']=True;validation['first_candidate_failure']='External SSDT scope AE_NOT_FOUND during load; I2C3 D0/controller succeeds, ELAN nub unregistered, precision touchpad absent; existing Magic Trackpad stack is Wacom touchscreen not ELAN';validation['retained_efi_payloads']=kept;(v/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n');copy(v/'VALIDATION.json',port/'Touchpad/INLINE-VALIDATION.json')
for n,h in kept.items():assert sha(efi/n)==h,n
assert sha(efi/'config.plist')==validation['config_sha256'] and sha(efi/'ACPI/DSDT.aml')==validation['dsdt_sha256']
subprocess.run(['plutil','-lint',str(efi/'config.plist')],check=True)
mf=port/'SHA256SUMS.json';m=json.loads(mf.read_text())
for n in ['collect-desktop-failure.sh','restore-touchpad.sh','Touchpad/INLINE-VALIDATION.json']+['Touchpad/source/'+n for n in ['aml_methods.py','prepare-inline-acpi.py','test-inline-acpi.py']]:m[n]=sha(port/n)
mf.write_text(json.dumps(m,indent=2)+'\n')
print('Published touchpad inline ACPI correction and current-desktop sampler. Controller, HID and all graphics binaries unchanged; prior candidate input files backed up. No target runtime/NVRAM change or reboot.')
