from pathlib import Path
import subprocess,plistlib,json,hashlib
root=Path(__file__).resolve().parent.parent;out=root/'desktop-build/ish-transport';out.mkdir(exist_ok=True)
name='LatitudeISHTransport';bundle='com.ved.'+name
flags=['-arch','x86_64','-mmacosx-version-min=11.0','-mkernel','-fapple-kext','-nostdinc','-I'+str(root/'MacKernelSDK/Headers'),'-DKERNEL','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-std=c++17','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-O2','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-Wno-nullability-completeness']
kmod=out/'kmod.cpp';kmod.write_text('''#include <mach/kmod.h>
extern "C" {
extern kmod_start_func_t _start;
extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL(com.ved.LatitudeISHTransport,"0.1.1",_start,_stop)
kmod_start_func_t *_realmain=0;
kmod_stop_func_t *_antimain=0;
int _kext_apple_cc=__APPLE_CC__;
}
''')
for source,obj in [(root/'tablet-sources'/(name+'.cpp'),out/'observer.o'),(kmod,out/'kmod.o')]:
 subprocess.run(['xcrun','clang++',*flags,'-c',str(source),'-o',str(obj)],check=True)
k=out/(name+'.kext');(k/'Contents/MacOS').mkdir(parents=True,exist_ok=True);binary=k/'Contents/MacOS'/name
subprocess.run(['xcrun','clang++','-arch','x86_64','-fapple-kext','-nostdlib','-Xlinker','-kext',str(out/'observer.o'),str(out/'kmod.o'),str(root/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(binary)],check=True)
d={'CFBundleExecutable':name,'CFBundleIdentifier':bundle,'CFBundleName':name,'CFBundleVersion':'0.1.1','CFBundleShortVersionString':'0.1.1','CFBundlePackageType':'KEXT','OSBundleRequired':'Root','OSBundleLibraries':{'com.apple.iokit.IOPCIFamily':'2.9','com.apple.kpi.unsupported':'11.0.0','com.apple.kpi.iokit':'11.0.0','com.apple.kpi.libkern':'11.0.0','com.apple.kpi.mach':'11.0.0'},'IOKitPersonalities':{name:{'CFBundleIdentifier':bundle,'IOClass':name,'IOProviderClass':'IOPCIDevice','IOPCIPrimaryMatch':'0x7e458086','IOMatchCategory':'LatitudeISHExperimental'}}}
(k/'Contents/Info.plist').write_bytes(plistlib.dumps(d));subprocess.run(['codesign','-s','-','--force',str(k)],check=True);subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
uuid=subprocess.check_output(['xcrun','dwarfdump','--uuid',str(binary)],text=True).strip();symbols=subprocess.check_output(['xcrun','nm',str(binary)],text=True)
for symbol in ['_kmod_info','__start','__stop','__realmain','__antimain']:assert any(line.split()[-1]==symbol and ' U ' not in line for line in symbols.splitlines())
assert 'configWrite' not in symbols and 'setBusMasterEnable' not in symbols
print(uuid);print('PASS x86_64 kext linked and signature verified; no PCI configuration-write symbols. Not installed or activated.')
(out/'BUILD.json').write_text(json.dumps({'version':'0.1.1','uuid':uuid,'binarySHA256':hashlib.sha256(binary.read_bytes()).hexdigest(),'installed':False,'defaultMode':'read-only PCI preflight','transportArm':'latitude-ish-pio=1','automaticRotation':False,'liveSensorFeaturesEnabled':False},indent=2)+'\n')
