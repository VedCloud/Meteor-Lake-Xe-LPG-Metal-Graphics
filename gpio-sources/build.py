from pathlib import Path
import subprocess,plistlib
r=Path(__file__).resolve().parent.parent;out=r/'desktop-build/gpio-mtl';out.mkdir(exist_ok=True)
flags=['-arch','x86_64','-mmacosx-version-min=11.0','-mkernel','-fapple-kext','-nostdinc','-I'+str(r/'MacKernelSDK/Headers'),'-I'+str(r/'research/voodooi2c-core/Dependencies/VoodooGPIO/VoodooGPIO'),'-DKERNEL','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-std=c++17','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-O2','-Wall','-Werror','-Wno-deprecated-declarations','-Wno-inconsistent-missing-override','-Wno-nullability-completeness']
subprocess.run(['xcrun','clang++',*flags,'-c',str(r/'gpio-sources/LatitudeGPIO.cpp'),'-o',str(out/'gpio.o')],check=True)
s=(r/'hotkey-build/kmod.cpp').read_text().replace('com.ved.LatitudeHotkeys','com.ved.LatitudeGPIO').replace('0.1.1','0.1.1');(out/'kmod.cpp').write_text(s)
subprocess.run(['xcrun','clang++',*flags,'-c',str(out/'kmod.cpp'),'-o',str(out/'kmod.o')],check=True)
k=out/'LatitudeGPIO.kext';(k/'Contents/MacOS').mkdir(parents=True,exist_ok=True)
subprocess.run(['xcrun','clang++','-arch','x86_64','-mmacosx-version-min=11.0','-fapple-kext','-nostdlib','-Xlinker','-kext',str(out/'gpio.o'),str(out/'kmod.o'),str(r/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(k/'Contents/MacOS/LatitudeGPIO')],check=True)
d={'CFBundleExecutable':'LatitudeGPIO','CFBundleIdentifier':'com.ved.LatitudeGPIO','CFBundleName':'LatitudeGPIO','CFBundleVersion':'0.1.1','CFBundleShortVersionString':'0.1.1','CFBundlePackageType':'KEXT','OSBundleRequired':'Root','OSBundleLibraries':{'org.coolstar.VoodooGPIO':'1.2.0','com.apple.iokit.IOACPIFamily':'1.4.0','com.apple.kpi.iokit':'11.0.0','com.apple.kpi.libkern':'11.0.0'},'IOKitPersonalities':{'LatitudeMeteorLakeGPIO':{'CFBundleIdentifier':'com.ved.LatitudeGPIO','IOClass':'LatitudeGPIO','IOProviderClass':'IOACPIPlatformDevice','IONameMatch':'INTC1083','IOProbeScore':10000}}}
(k/'Contents/Info.plist').write_bytes(plistlib.dumps(d));subprocess.run(['codesign','-s','-','--force',str(k)],check=True);subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
print(subprocess.check_output(['xcrun','dwarfdump','--uuid',str(k/'Contents/MacOS/LatitudeGPIO')],text=True))
