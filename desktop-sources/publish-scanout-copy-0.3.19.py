from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent
p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def inventory(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def copy(a,b):
 b.parent.mkdir(parents=True,exist_ok=True);tmp=b.with_name(b.name+'.publish-tmp');shutil.copy2(a,tmp);tmp.replace(b)
m=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in m.items():assert sha(port/n)==h,n
efi=inventory(u/'EFI')
files=sorted(f for f in p.rglob('*') if f.is_file() and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
changed=[str(f.relative_to(p)) for f in files if not (port/'Desktop'/f.relative_to(p)).exists() or sha(f)!=sha(port/'Desktop'/f.relative_to(p))]
extra=['collect-desktop-failure.sh','PORT-STATUS.md','scanout-copy-0.3.19.md','scanout-copy-0.3.19-validation.json']
names=['Desktop/'+n for n in changed+['SHA256SUMS.txt']]+extra+['SHA256SUMS.json']
b=r/'backups/scanout-copy-before-0.3.19';assert not b.exists();b.mkdir();shutil.copytree(u/'EFI',b/'EFI')
absent=[]
for n in names:
 if (port/n).is_file():copy(port/n,b/n)
 else:absent.append(n)
(b/'BACKUP.json').write_text(json.dumps({'scope':'EFI and replaced package files; not an OS or AuxKC snapshot','efi':efi,'changed_files':names,'previously_absent':absent},indent=2)+'\n')
usbbackup=port/'backups'/b.name;assert not usbbackup.exists();shutil.copytree(b,usbbackup)
validation={'candidate':'0.3.19','components':json.loads((p/'BUILD-MANIFEST.json').read_text())['candidate_0_3_19'],'no_new_kernel_imports':True,'gamma_copy_integer_only_disassembly':True,'gamma_copy_stack_frame_bytes':0x568,'tests':['x86_64 Metal/framebuffer/accelerator compilation and signatures','60 gamma tile guard-page layouts and scalar-reference pixels/alpha/padding/source preservation','gamma decoder and framebuffer setter','actual surface publication: AvoidSync, CannotLock fallback, lock errors, GPU errors, saturated staging and completion marker','actual event completion ordering','root counter endpoint with absolute ACPI path','collector capture and absent Weather','installer and three-kext AuxKC preflight/rollback fixtures'],'target_performance_verified':False,'weather_fixed':False,'physical_brightness_keys_fixed':False,'timeouts_fixed':False,'night_shift_confirmed_in_previous_candidate':True,'full_gpu_suite_rerun':False,'target_installed_or_armed':False}
a=r/'port-artifacts/scanout-copy-0.3.19';a.mkdir(parents=True,exist_ok=True);(a/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n')
shutil.copy2(r/'desktop-build/gamma-copy-0.2.9.asm',a/'gamma-copy.asm')
try:
 (p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
 for f in files+[p/'SHA256SUMS.txt']:
  dest=port/'Desktop'/f.relative_to(p)
  if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
 copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
 copy(r/'research/scanout-copy-0.3.19.md',port/'scanout-copy-0.3.19.md')
 copy(a/'VALIDATION.json',port/'scanout-copy-0.3.19-validation.json')
 (port/'PORT-STATUS.md').write_text((r/'research/scanout-copy-0.3.19.md').read_text()+'\n---\n\n'+(b/'PORT-STATUS.md').read_text())
 for f in files+[p/'SHA256SUMS.txt']:m['Desktop/'+str(f.relative_to(p))]=sha(port/'Desktop'/f.relative_to(p))
 for n in extra:m[n]=sha(port/n)
 for n,h in m.items():assert sha(port/n)==h,n
 for line in (p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():
  h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 for name in ['WindowServerAccelerator/NMXAccel.kext','WindowServerFramebuffer/LatitudeMTLFB.kext','WindowServerPlugin/NVMTLDriver.bundle']:
  subprocess.run(['codesign','--verify','--strict',str(port/'Desktop'/name)],check=True)
 for name in ['Desktop/install-windowserver.sh','collect-desktop-failure.sh']:subprocess.run(['/bin/bash','-n',str(port/name)],check=True)
 assert inventory(u/'EFI')==efi,'Unexpected EFI change'
 (port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
 for n,h in json.loads((port/'SHA256SUMS.json').read_text()).items():assert sha(port/n)==h,n
except BaseException:
 for n in names:
  if (b/n).is_file():copy(b/n,port/n)
  elif n in absent and (port/n).is_file():(port/n).unlink()
 raise
print('Published0.3.19:',len(m),'root hashes; signatures, frozen baseline and unchanged EFI verified. Backup:',usbbackup)
