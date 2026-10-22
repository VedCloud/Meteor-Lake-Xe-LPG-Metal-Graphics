from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';b=port/'backups/smoothness-before-0.3.12'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.publish-tmp');shutil.copy2(src,tmp);tmp.replace(dst)
def verify(root,sums):
 for line in sums.read_text().splitlines():
  digest,name=line.split('  ',1);assert sha(root/name)==digest,name
assert u.is_mount() and not b.exists(),'USB missing or backup already exists'
m=json.loads((port/'SHA256SUMS.json').read_text())
for name,digest in m.items():assert sha(port/name)==digest,name
verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
old=port/'Desktop/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver'
assert sha(old)=='e388688d61a5c7c962a3601a944b8d7711b2f61b62e20349bdcaf7b513358e6f'
frozen=['WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU','WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB','Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel','WindowServerBackend/libvulkan_intel.dylib','WindowServerTransport/libdrm.2.dylib']
identity={name:sha(port/'Desktop'/name) for name in frozen};assert all(sha(p/name)==digest for name,digest in identity.items())
efi_before={str(f.relative_to(u/'EFI')):sha(f) for f in (u/'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
for name in ['Desktop/WindowServerPlugin','Desktop/AboutThisMac','Desktop/install-windowserver.sh','Desktop/install-about-this-mac.sh','Desktop/restore-about-this-mac.sh','Desktop/Library/NullMoth/load-panel-framebuffer.sh','Desktop/README.md','Desktop/BUILD-MANIFEST.json','Desktop/SHA256SUMS.txt','collect-desktop-failure.sh','SHA256SUMS.json','PORT-STATUS.md']:
 src=port/name;dst=b/name
 if src.is_dir():shutil.copytree(src,dst)
 else:copy(src,dst)
shutil.copytree(u/'EFI',b/'EFI')
(b/'BACKUP.json').write_text(json.dumps({'target_report':'failure-detail-20261020-135231','plugin':'0.3.11','plugin_sha256':sha(old),'scope':'EFI and changed package/collector files; not an OS or AuxiliaryKC backup','unchanged_runtime_hashes':identity},indent=2)+'\n')
local=r/'backups/smoothness-before-0.3.12';assert not local.exists();shutil.copytree(b,local)
for name in ['install-windowserver.sh','install-about-this-mac.sh','restore-about-this-mac.sh','Library/NullMoth/load-panel-framebuffer.sh']:
 subprocess.run(['/bin/bash','-n',str(p/name)],check=True)
subprocess.run(['/bin/bash','-n',str(r/'desktop-sources/collect-desktop-failure.sh')],check=True)
for name in ['WindowServerPlugin/NVMTLDriver.bundle','AboutThisMac/latitude-about-this-mac']:
 subprocess.run(['codesign','--verify','--strict',str(p/name)],check=True)
files=sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dst=port/'Desktop'/f.relative_to(p)
 if not dst.exists() or sha(dst)!=sha(f):copy(f,dst)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
copy(r/'research/smoothness-0.3.12.md',port/'smoothness-0.3.12.md')
status=port/'PORT-STATUS.md';status.write_text((r/'research/smoothness-0.3.12.md').read_text()+'\n---\n\n'+status.read_text())
validation={'candidate':'0.3.12','plugin_sha256':sha(p/'WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver'),'plugin_uuid':'D97DEED3-66E3-352D-8FF7-76270206D333','three_plugin_variants_compiled':True,'signatures_verified':True,'new_plugin_imports':[],'missing_plugin_exports':[],'tests':['actual swizzle-view method ASan/UBSan','actual tight/padded readback/cache/failure fixtures ASan/UBSan x86_64','actual publication ordering','installer/rollback/branding-failure isolation','loader','Weather and branding collector success/failure','release constructor/diagnostic controls','release logging','30000 pool lifetimes and completion-gated retirement','serialized submit timeout/quarantine','native and x86_64 branding helper resolver/preferences fixtures'],'frozen_rendering_evidence_preserved':True,'efi_unchanged':True,'hardware_suite_rerun':False,'dell_installation_or_reboot_performed':False,'weather_fixed_on_target':False,'coreanimation_timeouts_fixed_on_target':False,'new_performance_result':None}
art=r/'port-artifacts/smoothness-0.3.12';art.mkdir(exist_ok=True);(art/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n');copy(art/'VALIDATION.json',port/'smoothness-0.3.12-validation.json')
for f in files+[p/'SHA256SUMS.txt']:
 name='Desktop/'+str(f.relative_to(p));m[name]=sha(port/name)
for name in ['collect-desktop-failure.sh','smoothness-0.3.12.md','smoothness-0.3.12-validation.json','PORT-STATUS.md']:m[name]=sha(port/name)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt');verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name,digest in m.items():assert sha(port/name)==digest,name
assert identity=={name:sha(port/'Desktop'/name) for name in frozen}
assert efi_before=={str(f.relative_to(u/'EFI')):sha(f) for f in (u/'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
print('Published0.3.12. Package/root manifests verified; graphics kernel, framebuffer, accelerator, backend, transport, EFI and saved rendering evidence unchanged. Prior0.3.11+EFI saved. No target installation or reboot. Weather/timeouts/performance/icon display require the next Dell report.')
