from pathlib import Path
import subprocess,shutil,plistlib,json
r=Path(__file__).resolve().parent.parent;src=r/'research/voodooi2c-core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID';out=r/'desktop-build/hid-readguard';out.mkdir(exist_ok=True)
sdk=subprocess.check_output(['xcrun','--show-sdk-path'],text=True).strip()
flags=['-arch','x86_64','-mmacosx-version-min=10.12','-mkernel','-fapple-kext','-nostdinc','-I'+str(r/'MacKernelSDK/Headers'),'-DKERNEL','-D_FORTIFY_SOURCE=0','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-std=c++11','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-O2','-Wno-inconsistent-missing-override','-Wno-deprecated-declarations','-Wno-nullability-completeness']
objects=[]
for f in sorted(src.rglob('*.cpp')):
 obj=out/(f.stem+'.o');objects.append(obj)
 with (out/(f.stem+'.log')).open('w') as log:subprocess.run(['xcrun','clang++',*flags,'-c',str(f),'-o',str(obj)],stdout=log,stderr=subprocess.STDOUT,check=True)
 print('Compiled',f.name,flush=True)
f=out/'kmod.cpp';f.write_text('''#include <mach/kmod.h>
extern "C" {
extern kmod_start_func_t _start;
extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL(com.alexandred.VoodooI2CHID,"1.0.2",_start,_stop)
kmod_start_func_t *_realmain = 0;
kmod_stop_func_t *_antimain = 0;
int _kext_apple_cc = __APPLE_CC__;
}
''');obj=out/'kmod.o';subprocess.run(['xcrun','clang++',*flags,'-c',str(f),'-o',str(obj)],check=True);objects.append(obj)
kext=out/'VoodooI2CHID.kext';shutil.copytree(r/'port-artifacts/hid-nullguard-1.0.1/Original-VoodooI2CHID.kext',kext,dirs_exist_ok=True)
with (out/'link.log').open('w') as log:subprocess.run(['xcrun','clang++','-arch','x86_64','-mmacosx-version-min=10.12','-fapple-kext','-nostdlib','-Xlinker','-kext',*[str(o) for o in objects],str(r/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(kext/'Contents/MacOS/VoodooI2CHID')],stdout=log,stderr=subprocess.STDOUT,check=True)
f=kext/'Contents/Info.plist';d=plistlib.loads(f.read_bytes());d['CFBundleVersion']=d['CFBundleShortVersionString']='1.0.2';f.write_bytes(plistlib.dumps(d));subprocess.run(['codesign','--force','--sign','-',str(kext)],check=True)
print(subprocess.check_output(['xcrun','dwarfdump','--uuid',str(kext/'Contents/MacOS/VoodooI2CHID')],text=True))
