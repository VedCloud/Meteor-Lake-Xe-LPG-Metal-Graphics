from pathlib import Path
import subprocess,os,shutil,plistlib,json,hashlib
p=Path(__file__).resolve().parent;b=p.parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');tool=r/'toolchain/rustup/toolchains/stable-aarch64-apple-darwin/bin';env=dict(os.environ,CARGO_HOME=str(r/'toolchain/cargo'),RUSTC=str(tool/'rustc'),RUSTDOC=str(tool/'rustdoc'),CARGO_TARGET_DIR=str(p/'build/target'))
(p/'build').mkdir(exist_ok=True)
k=p/'build/NVMTLDriver.bundle';shutil.copytree(b/'Tahoe-Combined-Runtime/Library/GPUBundles/NVMTLDriver.bundle',k,dirs_exist_ok=True)
with (p/'build/plugin.log').open('w') as log:
 subprocess.run(['xcrun','clang','-arch','x86_64','-mmacosx-version-min=26.0','-bundle','-fobjc-arc','-O1','-DNVMTL_RELEASE=1','-Wno-protocol','-Wno-incomplete-implementation','-I',str(r/'Vulkan-Headers/include'),'-o',str(k/'Contents/MacOS/NVMTLDriver'),str(Path(os.environ.get('NVMTL_PLUGIN_SOURCE',str(p/'plugin')))/'NVMTLDevice.m'),'-framework','Foundation','-framework','Metal','-framework','IOSurface','-framework','IOKit'],stdout=log,stderr=subprocess.STDOUT,check=True)
print('Metal plugin compiled.',flush=True)
with (p/'build/translator.log').open('w') as log:
 subprocess.run([str(tool/'cargo'),'build','--offline','--release','--manifest-path',str(p/'translator/wrapper/Cargo.toml'),'--target','x86_64-apple-darwin'],env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
shutil.copy2(p/'build/target/x86_64-apple-darwin/release/libnvmtl_translate.dylib',k/'Contents/MacOS/libnvmtl_translate.dylib')
subprocess.run(['install_name_tool','-id','@rpath/libnvmtl_translate.dylib',str(k/'Contents/MacOS/libnvmtl_translate.dylib')],check=True)
f=k/'Contents/Info.plist';d=plistlib.loads(f.read_bytes());d['CFBundleVersion']=d['CFBundleShortVersionString']='0.3.21';f.write_bytes(plistlib.dumps(d))
for f in [k/'Contents/MacOS/libnvmtl_translate.dylib',k]:subprocess.run(['codesign','--force','--sign','-',str(f)],check=True)
subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
(p/'BUILD.json').write_text(json.dumps({'version':'0.3.21','hardwareTested':False,'files':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in [k/'Contents/MacOS/NVMTLDriver',k/'Contents/MacOS/libnvmtl_translate.dylib']}},indent=2)+'\n');print('A8 candidate compiled and signed.',flush=True)
