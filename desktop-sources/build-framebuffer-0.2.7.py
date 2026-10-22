from pathlib import Path
import subprocess,shutil,plistlib
r=Path(__file__).resolve().parent.parent;pkg=r/'latitude-desktop';out=r/'desktop-build';src=r/'NullMoth/kexts/NMXVGAFB'
sdk=subprocess.check_output(['xcrun','--show-sdk-path'],text=True).strip()
flags=['-arch','x86_64','-mmacosx-version-min=15.0','-fapple-kext','-mkernel','-nostdinc','-I'+sdk+'/System/Library/Frameworks/Kernel.framework/Headers','-DKERNEL','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-DAPPLE','-DNeXT','-DLATITUDE_MTL_PANEL','-I'+str(r/'desktop-sources'),'-std=c++17','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-Wall','-Wno-unused-parameter','-Wno-inconsistent-missing-override','-O2']
with (out/'framebuffer-0.2.7.log').open('w') as log:subprocess.run(['xcrun','clang++',*flags,'-c',str(src/'nmxvgafb.cpp'),'-o',str(out/'nmxvgafb-0.2.7.o')],stdout=log,stderr=subprocess.STDOUT,check=True)
kext=pkg/'WindowServerFramebuffer/LatitudeMTLFB.kext';shutil.copytree(pkg/'Library/Extensions/LatitudeMTLFB.kext',kext,dirs_exist_ok=True)
subprocess.run(['xcrun','clang++','-arch','x86_64','-mmacosx-version-min=15.0','-fapple-kext','-nostdlib','-Xlinker','-kext','-lkmodc++','-lkmod','-lcc_kext',str(out/'nmxvgafb-0.2.7.o'),str(out/'nmx_virtio_gpu.o'),'-o',str(kext/'Contents/MacOS/LatitudeMTLFB')],check=True)
p=kext/'Contents/Info.plist';x=plistlib.loads(p.read_bytes());x['CFBundleVersion']=x['CFBundleShortVersionString']='0.2.7';p.write_bytes(plistlib.dumps(x));subprocess.run(['codesign','-s','-','--force',str(kext)],check=True)
uuid=subprocess.check_output(['xcrun','dwarfdump','--uuid',str(kext/'Contents/MacOS/LatitudeMTLFB')],text=True).split()[1]
(pkg/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').write_text(uuid+'\n');(pkg/'WindowServerFramebuffer/LatitudeMTLFB-VERSION.txt').write_text('0.2.7\n');print('Framebuffer',uuid)
