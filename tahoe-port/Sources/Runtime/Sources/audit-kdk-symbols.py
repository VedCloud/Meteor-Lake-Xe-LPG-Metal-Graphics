from pathlib import Path
import plistlib,subprocess,json,hashlib
b=Path(__file__).resolve().parent;p=b/'Tahoe-Graphics-Runtime';root=b/'KDK25G241-expanded/KDK.pkg/Payload'
def symbols(f,flag):
 z=subprocess.run(['xcrun','nm','-arch','x86_64',flag,str(f)],capture_output=True,text=True)
 return set(z.stdout.splitlines()) if z.returncode==0 else set()
exports=symbols(root/'System/Library/Kernels/kernel','-gjU');count=0
for k in (root/'System/Library/Extensions').rglob('*.kext'):
 for info in [k/'Contents/Info.plist',k/'Info.plist']:
  if not info.is_file():continue
  d=plistlib.loads(info.read_bytes());n=d.get('CFBundleExecutable')
  if not n:break
  f=next((x for x in [k/'Contents/MacOS'/n,k/n] if x.is_file()),None)
  if f:
   syms=symbols(f,'-gjU');exports|=syms;count+=bool(syms)
  break
out={'build':'25G241','donorX86Kexts':count,'exports':len(exports),'binaries':{},'kernelCollectionCreationOnHost':False,'hostKernelCollectionLimit':'macOS27 ARM host kmutil rejects x86_64 and Intel __HIB; target installer performs real temporary/permanent-path KC builds on Intel Tahoe','physicalDellValidation':False}
for k in (p/'Library/Extensions').rglob('*.kext'):
 d=plistlib.loads((k/'Contents/Info.plist').read_bytes());f=k/'Contents/MacOS'/d['CFBundleExecutable'];imp=symbols(f,'-uj');missing=sorted(imp-exports);assert imp
 out['binaries'][k.name]={'sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'imports':len(imp),'missing':missing};assert not missing,(k.name,missing)
print('All four graphics kexts resolve against actual25G241 x86 exports from',count,'donor kexts.')
(b/'KDK-SYMBOL-VALIDATION.json').write_text(json.dumps(out,indent=2)+'\n')
