from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inventory(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def copy(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);t=b.with_name(b.name+'.publish-tmp');shutil.copy2(a,t);t.replace(b)
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
before=inventory(u/'EFI');assert before['OC/ACPI/DSDT.aml']=='0888e0d04465c4b8dc16b460bb4ed25286f33f4ab3fa13957f718563e346ea78'
assert before['OC/ACPI/SSDT-LAT-BRT.aml']=='7d20b1f86478d5fcbb14d4e51629eb2e8fb6079deb2a915dcd85a3bad3c0b51c'
files=sorted(f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
changes=[str(f.relative_to(p)) for f in files if not (port/'Desktop'/f.relative_to(p)).exists() or sha(f)!=sha(port/'Desktop'/f.relative_to(p))]
b=r/'backups/colour-transaction-before-0.3.18';assert not b.exists();b.mkdir();shutil.copytree(u/'EFI',b/'EFI')
for n in changes+['SHA256SUMS.txt']:
 f=port/'Desktop'/n
 if f.is_file():copy(f,b/'Desktop'/n)
for n in ['SHA256SUMS.json','collect-desktop-failure.sh','PORT-STATUS.md']:copy(port/n,b/n)
(b/'BACKUP.json').write_text(json.dumps({'scope':'EFI and replaced package files; not an OS or AuxKC snapshot','efi_before':before,'changed_package_files':changes,'previously_absent':[n for n in changes if not (port/'Desktop'/n).exists()]},indent=2)+'\n')
usbbackup=port/'backups'/b.name;assert not usbbackup.exists();shutil.copytree(b,usbbackup)
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dest=port/'Desktop'/f.relative_to(p)
 if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
doc='colour-transaction-0.3.18.md';copy(r/'research'/doc,port/doc)
status=port/'PORT-STATUS.md';status.write_text((r/'research'/doc).read_text()+'\n---\n\n'+status.read_text())
try:
 # Keep original firmware query code intact, reached through the counter wrapper.
 old=(u/'EFI/OC/ACPI/DSDT.aml').read_bytes();expected=bytearray(old);assert expected.count(b'_Q66\x00')==1
 i=expected.index(b'_Q66\x00');expected[i:i+4]=b'XQ66';expected[9]=0;expected[9]=(-sum(expected))&255
 assert bytes(expected)==(p/'BrightnessKeys/DSDT-brightness.aml').read_bytes()
 assert sum((p/'BrightnessKeys/SSDT-LAT-BRT.aml').read_bytes())%256==0
 copy(p/'BrightnessKeys/SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml')
 copy(p/'BrightnessKeys/DSDT-brightness.aml',u/'EFI/OC/ACPI/DSDT.aml')
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 after=inventory(u/'EFI');assert {n for n in set(before)|set(after) if before.get(n)!=after.get(n)}=={'OC/ACPI/DSDT.aml','OC/ACPI/SSDT-LAT-BRT.aml'}
except BaseException:
 for name in ['DSDT.aml','SSDT-LAT-BRT.aml']:copy(b/'EFI/OC/ACPI'/name,u/'EFI/OC/ACPI'/name)
 raise
v={'candidate':'0.3.18','components':{name:(p/path).read_text().strip() for name,path in [('plugin_uuid','WindowServerPlugin/NVMTLDriver-UUID.txt'),('framebuffer_uuid','WindowServerFramebuffer/LatitudeMTLFB-UUID.txt'),('accelerator_uuid','WindowServerAccelerator/NMXAccel-UUID.txt')]},'new_imports':json.loads((r/'port-artifacts/colour-transaction-0.3.18/NEW-IMPORTS.json').read_text()),'reference_exports_verified':True,'efi_before':before,'efi_after':after,'tests':['colour math ASan/UBSan','actual pipe colour conversion/cache/bypass/refusal fixture','reference getter fingerprint; privileged FP guard compiled not host executed','actual framebuffer gamma and front timer','actual root reporting/counter endpoint ASan/UBSan','ACPICA actual AML firmware-query chain/counters/fallback','actual release logger; all bounded records survive secondary throttle','collector capture fixture','installer and three-kext AuxKC/prelink rollback fixtures'],'target_nightshift_verified':False,'brightness_keys_fixed':False,'remaining_timeouts_fixed':False,'non_diagonal_csc_supported':False,'full_gpu_suite_rerun':False,'target_installed_or_armed_or_rebooted':False}
a=r/'port-artifacts/colour-transaction-0.3.18/VALIDATION.json';a.write_text(json.dumps(v,indent=2)+'\n');copy(a,port/'colour-transaction-0.3.18-validation.json')
for f in files+[p/'SHA256SUMS.txt']:
 n='Desktop/'+str(f.relative_to(p));m[n]=sha(port/n)
for n in [doc,'colour-transaction-0.3.18-validation.json','PORT-STATUS.md','collect-desktop-failure.sh']:m[n]=sha(port/n)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
for n,h in m.items():assert sha(port/n)==h,n
for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
 h,n=line.split(None,1);assert sha(port/n.strip())==h,n
for name in ['WindowServerAccelerator/NMXAccel.kext','WindowServerFramebuffer/LatitudeMTLFB.kext','WindowServerPlugin/NVMTLDriver.bundle','latitude-key-state']:
 subprocess.run(['codesign','--verify','--strict',str(port/'Desktop'/name)],check=True)
assert (port/'collect-desktop-failure.sh').read_bytes()==(r/'desktop-sources/collect-desktop-failure.sh').read_bytes()
print('Published0.3.18:',len(m),'root hashes, frozen baseline, copied signatures and intended two EFI changes verified. Backup:',usbbackup)
