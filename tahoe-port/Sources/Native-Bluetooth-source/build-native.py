from pathlib import Path
import subprocess,plistlib,shutil
b=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');out=b/'build';out.mkdir(exist_ok=True)
flags=['-arch','x86_64','-mmacosx-version-min=10.12','-mkernel','-fapple-kext','-nostdinc','-I'+str(r/'MacKernelSDK/Headers'),'-I'+str(b/'Lilu-master/Lilu'),'-DKERNEL','-D_FORTIFY_SOURCE=0','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-DPRODUCT_NAME=IntelBTPatcher','-DMODULE_VERSION=2.5.2','-std=c++14','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-O2','-Wno-inconsistent-missing-override','-Wno-deprecated-declarations','-Wno-nullability-completeness']
objects=[]
for f in [b/'IntelBTPatcher.cpp',b/'Lilu-master/Lilu/Library/plugin_start.cpp']:
 obj=out/(f.stem+'.o');objects.append(obj)
 with (out/(f.stem+'.log')).open('w')as log:
  result=subprocess.run(['xcrun','clang++',*flags,'-c',str(f),'-o',str(obj)],stdout=log,stderr=subprocess.STDOUT)
 if result.returncode:
  print((out/(f.stem+'.log')).read_text()[-8500:]);raise SystemExit(result.returncode)
 print('Compiled',f.name)
# Lilu bootstrap functions use C linkage via EXPORT, not kmod wrappers.
kmod=out/'kmod.cpp';kmod.write_text('''#include <mach/kmod.h>
extern "C" {
extern kmod_start_func_t _start;
extern kmod_stop_func_t _stop;
extern kmod_start_func_t IntelBTPatcher_kern_start;
extern kmod_stop_func_t IntelBTPatcher_kern_stop;
KMOD_EXPLICIT_DECL(com.zxystd.IntelBTPatcher,"2.5.2",_start,_stop)
kmod_start_func_t *_realmain = IntelBTPatcher_kern_start;
kmod_stop_func_t *_antimain = IntelBTPatcher_kern_stop;
int _kext_apple_cc = __APPLE_CC__;
}
''');obj=out/'kmod.o';subprocess.run(['xcrun','clang++',*flags,'-c',str(kmod),'-o',str(obj)],check=True);objects.append(obj)
kext=out/'IntelBTPatcher.kext';shutil.copytree('/Volumes/EFI/EFI/OC/Kexts/IntelBTPatcher.kext',kext,dirs_exist_ok=True,ignore=shutil.ignore_patterns('._*','.DS_Store'))
for f in sorted(kext.rglob('._*'),reverse=True):
 if f.is_file():f.unlink()
with (out/'link.log').open('w')as log:
 p=subprocess.run(['xcrun','clang++','-arch','x86_64','-mmacosx-version-min=10.12','-fapple-kext','-nostdlib','-Xlinker','-kext',*[str(o)for o in objects],str(r/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(kext/'Contents/MacOS/IntelBTPatcher')],stdout=log,stderr=subprocess.STDOUT)
if p.returncode:print((out/'link.log').read_text());raise SystemExit(p.returncode)
f=kext/'Contents/Info.plist';d=plistlib.loads(f.read_bytes());d['CFBundleVersion']=d['CFBundleShortVersionString']='2.5.2';f.write_bytes(plistlib.dumps(d));subprocess.run(['codesign','--force','--sign','-',str(kext)],check=True)
print(subprocess.check_output(['xcrun','dwarfdump','--uuid',str(kext/'Contents/MacOS/IntelBTPatcher')],text=True))
