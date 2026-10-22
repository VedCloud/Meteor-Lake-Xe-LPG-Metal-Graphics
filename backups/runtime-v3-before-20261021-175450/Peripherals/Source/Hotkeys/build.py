from pathlib import Path
import subprocess,plistlib
r=Path(__file__).resolve().parent.parent;sdk=subprocess.check_output(['xcrun','--show-sdk-path'],text=True).strip();p=r/'hotkey-build/LatitudeHotkeys.kext';(p/'Contents/MacOS').mkdir(parents=True,exist_ok=True)
flags=['-arch','x86_64','-mmacosx-version-min=15.0','-fapple-kext','-mkernel','-nostdinc','-I'+sdk+'/System/Library/Frameworks/Kernel.framework/Headers','-DKERNEL','-DKERNEL_PRIVATE','-std=c++17','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-Wall','-Werror','-O2']
subprocess.run(['xcrun','clang++',*flags,'-c',str(r/'hotkey-sources/LatitudeHotkeys.cpp'),'-o',str(r/'hotkey-build/LatitudeHotkeys.o')],check=True)
k=r/'hotkey-build/kmod.cpp';k.write_text('''#include <mach/kmod.h>
extern "C" {
extern kmod_start_func_t _start;
extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL(com.ved.LatitudeHotkeys,"0.1.1",_start,_stop)
kmod_start_func_t *_realmain=0;
kmod_stop_func_t *_antimain=0;
int _kext_apple_cc=__APPLE_CC__;
}
''')
subprocess.run(['xcrun','clang++',*flags,'-c',str(k),'-o',str(r/'hotkey-build/kmod.o')],check=True)
subprocess.run(['xcrun','clang++','-arch','x86_64','-fapple-kext','-nostdlib','-Xlinker','-kext',str(r/'hotkey-build/LatitudeHotkeys.o'),str(r/'hotkey-build/kmod.o'),str(r/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(p/'Contents/MacOS/LatitudeHotkeys')],check=True)
d={'CFBundleExecutable':'LatitudeHotkeys','CFBundleIdentifier':'com.ved.LatitudeHotkeys','CFBundleName':'LatitudeHotkeys','CFBundlePackageType':'KEXT','CFBundleVersion':'0.1.1','CFBundleShortVersionString':'0.1.1','CFBundleInfoDictionaryVersion':'6.0','OSBundleRequired':'Root','OSBundleLibraries':{'com.apple.kpi.iokit':'8.0','com.apple.kpi.libkern':'8.0','com.apple.iokit.IOACPIFamily':'1.4'},'IOKitPersonalities':{'LatitudeHotkeys':{'CFBundleIdentifier':'com.ved.LatitudeHotkeys','IOClass':'LatitudeHotkeys','IOProviderClass':'IOACPIPlatformDevice','IONameMatch':'PNP0C14','IOProbeScore':1000}}}
(p/'Contents/Info.plist').write_bytes(plistlib.dumps(d));subprocess.run(['codesign','-s','-','--force',str(p)],check=True);subprocess.run(['codesign','--verify','--strict',str(p)],check=True)
print(subprocess.check_output(['xcrun','dwarfdump','--uuid',str(p/'Contents/MacOS/LatitudeHotkeys')],text=True).strip())

syms=subprocess.check_output(['xcrun','nm',str(p/'Contents/MacOS/LatitudeHotkeys')],text=True)
for n in ['_kmod_info','__start','__stop','__realmain','__antimain']:assert any(x.split()[-1]==n and ' U ' not in x for x in syms.splitlines()),n
print('PASS defined kernel-module metadata and entry-point symbols')
