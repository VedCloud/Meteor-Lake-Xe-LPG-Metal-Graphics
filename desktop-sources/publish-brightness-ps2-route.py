from pathlib import Path
import shutil,hashlib,json,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inv(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);t=b.with_name(b.name+'.route-tmp');shutil.copy2(a,t);t.replace(b)
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
before=inv(u/'EFI');assert before['OC/ACPI/SSDT-LAT-BRT.aml']==sha(port/'Desktop/BrightnessKeys/SSDT-LAT-BRT.aml')
a=(p/'BrightnessKeys/SSDT-LAT-BRT.aml').read_bytes();assert a[:4]==b'SSDT' and sum(a)%256==0 and int.from_bytes(a[24:28],'little')==4
files=sorted(f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
changed=['Desktop/'+str(f.relative_to(p)) for f in files if not (port/'Desktop'/f.relative_to(p)).exists() or sha(f)!=sha(port/'Desktop'/f.relative_to(p))]
changed+=['Desktop/SHA256SUMS.txt','brightness-ps2-route.md','PORT-STATUS.md','SHA256SUMS.json']
b=r/'backups/brightness-ps2-route-before';assert not b.exists();b.mkdir();shutil.copytree(u/'EFI',b/'EFI');absent=[]
for n in changed:
 if (port/n).is_file():cp(port/n,b/n)
 else:absent.append(n)
(b/'BACKUP.json').write_text(json.dumps({'scope':'EFI/replaced package files, not an OS snapshot','efi':before,'changed_files':changed,'previously_absent':absent},indent=2)+'\n')
usbbackup=port/'backups'/b.name;assert not usbbackup.exists();shutil.copytree(b,usbbackup)
try:
 (p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
 for f in files+[p/'SHA256SUMS.txt']:
  dest=port/'Desktop'/f.relative_to(p)
  if not dest.exists() or sha(dest)!=sha(f):cp(f,dest)
 cp(r/'research/brightness-ps2-route.md',port/'brightness-ps2-route.md')
 (port/'PORT-STATUS.md').write_text((r/'research/brightness-ps2-route.md').read_text()+'\n---\n\n'+(b/'PORT-STATUS.md').read_text())
 cp(p/'BrightnessKeys/SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml')
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 after=inv(u/'EFI');assert {n for n in set(before)|set(after) if before.get(n)!=after.get(n)}=={'OC/ACPI/SSDT-LAT-BRT.aml'}
 for f in files+[p/'SHA256SUMS.txt']:m['Desktop/'+str(f.relative_to(p))]=sha(port/'Desktop'/f.relative_to(p))
 for n in ['brightness-ps2-route.md','PORT-STATUS.md']:m[n]=sha(port/n)
 for n,h in m.items():assert sha(port/n)==h,n
 for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
 for n,h in json.loads((port/'SHA256SUMS.json').read_text()).items():assert sha(port/n)==h,n
except BaseException:
 cp(b/'EFI/OC/ACPI/SSDT-LAT-BRT.aml',u/'EFI/OC/ACPI/SSDT-LAT-BRT.aml')
 for n in changed:
  if (b/n).is_file():cp(b/n,port/n)
  elif n in absent and (port/n).is_file():(port/n).unlink()
 raise
print('Published brightness route revision4; one EFI file changed,',len(m),'hashes and frozen baseline verified. Backup:',usbbackup)
