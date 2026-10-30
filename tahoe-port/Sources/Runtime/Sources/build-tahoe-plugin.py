from pathlib import Path
import shutil,subprocess,plistlib,os,json,hashlib
b=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');s=b/'Graphics-source/plugin';shutil.copytree(r/'NullMoth/plugin',s,dirs_exist_ok=True)
for name in ['NVMTLObjects.m','NVMTLEventSync.m','nvmtl_vk.c']:
 f=s/name;t=f.read_text();assert '0.3.19' in t;f.write_text(t.replace('0.3.19','0.3.20-tahoe'))
p=b/'Tahoe-Graphics-Runtime';(p/'Library/GPUBundles').mkdir(parents=True,exist_ok=True)
k=p/'Library/GPUBundles/NVMTLDriver.bundle';shutil.copytree(r/'latitude-desktop/WindowServerPlugin/NVMTLDriver.bundle',k,dirs_exist_ok=True)
env=dict(os.environ,SDKROOT='/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk')
with (b/'tahoe-plugin-build.log').open('w') as log:
 subprocess.run(['xcrun','clang','-arch','x86_64','-mmacosx-version-min=26.0','-bundle','-fobjc-arc','-O1','-DNVMTL_RELEASE=1','-Wno-protocol','-Wno-incomplete-implementation','-I',str(r/'Vulkan-Headers/include'),'-o',str(k/'Contents/MacOS/NVMTLDriver'),str(s/'NVMTLDevice.m'),'-framework','Foundation','-framework','Metal','-framework','IOSurface','-framework','IOKit'],env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
f=k/'Contents/Info.plist';d=plistlib.loads(f.read_bytes());d['CFBundleVersion']=d['CFBundleShortVersionString']='0.3.20';d['LatitudeTargetDarwin']='25.x';f.write_bytes(plistlib.dumps(d))
subprocess.run(['codesign','-s','-','--force',str(k)],check=True);subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
(p/'PLUGIN-BUILD.json').write_text(json.dumps({'version':'0.3.20','base':'0.3.19 with local presentation/fresh-pool fixes retained','arch':'x86_64','deploymentTarget':'26.0','SDK':'26.5','hardwareTested':False,'sha256':hashlib.sha256((k/'Contents/MacOS/NVMTLDriver').read_bytes()).hexdigest()},indent=2)+'\n');print('Tahoe Metal plugin compiled and signed.')
