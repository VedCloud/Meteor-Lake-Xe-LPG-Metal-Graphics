from pathlib import Path
import shutil,hashlib,json,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inv(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.scalar-tmp');shutil.copy2(a,tmp);tmp.replace(b)
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
before=inv(u/'EFI')
files=sorted(f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
changed=['Desktop/'+str(f.relative_to(p)) for f in files if not (port/'Desktop'/f.relative_to(p)).exists() or sha(f)!=sha(port/'Desktop'/f.relative_to(p))]
changed+=['Desktop/SHA256SUMS.txt','key-name-limit-query.md','key-name-limit-query-validation.json','PORT-STATUS.md','SHA256SUMS.json']
b=r/'backups/key-name-limit-query-before';assert not b.exists();b.mkdir();shutil.copytree(u/'EFI',b/'EFI');absent=[]
for n in changed:
 if (port/n).is_file():cp(port/n,b/n)
 else:absent.append(n)
(b/'BACKUP.json').write_text(json.dumps({'scope':'EFI/replaced package files, not an OS/AuxKC snapshot','efi':before,'changed_files':changed,'previously_absent':absent},indent=2)+'\n')
usbbackup=port/'backups'/b.name;assert not usbbackup.exists();shutil.copytree(b,usbbackup)
v={'framebuffer':'0.2.12','framebuffer_uuid':(p/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').read_text().strip(),'scope':'Locate unregistered ACPI LCD and evaluate local four-character LDBG; retain fresh-result guard','kernel_imports_known_from_target_tested_0_2_10_and_0_2_11':True,'tests':['actual root endpoint ASan/UBSan: reproduce both prior oversized-name failures; unregistered LCD/local LDBG success, malformed package, ACPI failures and root/stopping guards','actual key-state helper ASan/UBSan: success, failure-stage metadata and no stale counters after failed request','actual AML firmware/PS2 route and direct scalar counter checks','x86_64 framebuffer/helper builds and signatures','guarded installer compatibility and rollback fixture'],'firmware_key_source_confirmed':False,'brightness_keys_fixed':False,'hardware_query_verified':False,'gpu_suite_rerun':False,'target_installed_or_armed':False}
(r/'port-artifacts/key-name-limit-query/VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n')
try:
 (p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
 for f in files+[p/'SHA256SUMS.txt']:
  dest=port/'Desktop'/f.relative_to(p)
  if not dest.exists() or sha(dest)!=sha(f):cp(f,dest)
 cp(r/'research/key-name-limit-query.md',port/'key-name-limit-query.md')
 cp(r/'port-artifacts/key-name-limit-query/VALIDATION.json',port/'key-name-limit-query-validation.json')
 (port/'PORT-STATUS.md').write_text((r/'research/key-name-limit-query.md').read_text()+'\n---\n\n'+(b/'PORT-STATUS.md').read_text())
 for f in files+[p/'SHA256SUMS.txt']:m['Desktop/'+str(f.relative_to(p))]=sha(port/'Desktop'/f.relative_to(p))
 for n in ['key-name-limit-query.md','key-name-limit-query-validation.json','PORT-STATUS.md']:m[n]=sha(port/n)
 for n,h in m.items():assert sha(port/n)==h,n
 for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 for path in ['WindowServerFramebuffer/LatitudeMTLFB.kext','latitude-key-state']:subprocess.run(['codesign','--verify','--strict',str(port/'Desktop'/path)],check=True)
 subprocess.run(['/bin/bash','-n',str(port/'Desktop/install-windowserver.sh')],check=True)
 assert before==inv(u/'EFI'),'Unexpected EFI mutation'
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
 for n,h in json.loads((port/'SHA256SUMS.json').read_text()).items():assert sha(port/n)==h,n
except BaseException:
 for n in changed:
  if (b/n).is_file():cp(b/n,port/n)
  elif n in absent and (port/n).is_file():(port/n).unlink()
 raise
print('Published framebuffer0.2.12/local-name correction;',len(m),'root hashes, copied signatures, frozen baseline and unchanged EFI verified. Backup:',usbbackup)
