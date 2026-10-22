from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';b=port/'backups/smoothness-brightness-before-0.3.13'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.publish-tmp');shutil.copy2(src,tmp);tmp.replace(dst)
def inventory(root):return {str(f.relative_to(root)):sha(f) for f in root.rglob('*') if f.is_file() and not f.name.startswith('._')}
def verify(root,sums):
 for line in sums.read_text().splitlines():
  digest,name=line.split('  ',1);assert sha(root/name)==digest,name
assert u.is_mount() and not b.exists(),'USB missing or backup already exists'
m=json.loads((port/'SHA256SUMS.json').read_text())
for name,digest in m.items():assert sha(port/name)==digest,name
verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
assert sha(port/'Desktop/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver')=='3eb0fc04b45c0f03bc130d3ee152eb4c0dd5713ba30dd1196cabaa278adc8526'
assert sha(port/'Desktop/WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')=='4ff96f638d9942e9a3a6b642b443b3a1ab257d88692aa51cf2efa2dde84c104f'
frozen=['WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU','WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver','Library/Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel','WindowServerBackend/libvulkan_intel.dylib','WindowServerTransport/libdrm.2.dylib']
identity={name:sha(port/'Desktop'/name) for name in frozen};assert all(sha(p/name)==digest for name,digest in identity.items())
assert sha(p/'PreviousWindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')=='4ff96f638d9942e9a3a6b642b443b3a1ab257d88692aa51cf2efa2dde84c104f'
efi=inventory(u/'EFI')
changed=[]
files=sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
for f in files:
 name='Desktop/'+str(f.relative_to(p));dest=port/name
 if not dest.exists() or sha(dest)!=sha(f):changed.append(name)
for name in changed+['Desktop/SHA256SUMS.txt','collect-desktop-failure.sh','SHA256SUMS.json','PORT-STATUS.md']:
 src=port/name
 if src.exists():copy(src,b/name)
shutil.copytree(u/'EFI',b/'EFI')
(b/'BACKUP.json').write_text(json.dumps({'candidate':'0.3.12','basis_report':'failure-detail-20261020-141853','scope':'EFI and files replaced by 0.3.13; not an OS/AuxKC backup','prior_file_hashes':{name:sha(port/name) for name in changed if (port/name).is_file()},'new_paths_previously_absent':[name for name in changed if not (port/name).exists()],'frozen_runtime_hashes':identity,'efi_hashes':efi},indent=2)+'\n')
local=r/'backups/smoothness-brightness-before-0.3.13';assert not local.exists();shutil.copytree(b,local)
for name in ['install-windowserver.sh','update-windowserver-kernel.sh']:subprocess.run(['/bin/bash','-n',str(p/name)],check=True)
subprocess.run(['/bin/bash','-n',str(r/'desktop-sources/collect-desktop-failure.sh')],check=True)
for name in ['WindowServerAccelerator/NMXAccel.kext','WindowServerFramebuffer/LatitudeMTLFB.kext','latitude-frame-pacing']:subprocess.run(['codesign','--verify','--strict',str(p/name)],check=True)
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dest=port/'Desktop'/f.relative_to(p)
 if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
doc='smoothness-and-brightness-0.3.13.md';copy(r/'research'/doc,port/doc)
status=port/'PORT-STATUS.md';status.write_text((r/'research'/doc).read_text()+'\n---\n\n'+status.read_text())
v={'candidate':'0.3.13','accelerator_version':'0.2.5','accelerator_uuid':(p/'WindowServerAccelerator/NMXAccel-UUID.txt').read_text().strip(),'framebuffer_version':'0.2.6','framebuffer_uuid':(p/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').read_text().strip(),'metal_plugin':'0.3.12 unchanged','tests':['x86_64 kernel builds and signatures','actual copy helper ASan/UBSan tight/padded/guard-page/overflow/deadline cases','actual PWM planner ASan/UBSan all 65536 levels and invalid-state rejection','native/x86_64 frame observer timestamp/idle/percentile fixtures without capture','installer new-accelerator success, wrong prelink UUID failure and complete rollback','collector default/explicit end, Weather/crash/branding/frame-observer fixtures'],'new_accelerator_imports':['_absolutetime_to_nanoseconds','_nanoseconds_to_absolutetime'],'missing_prior_accelerator_exports':[],'existing_mach_kpi_dependency':True,'target_kc_resolution_pending':True,'baseline_preserved':True,'efi_unchanged':True,'full_hardware_suite_rerun':False,'host_real_capture':False,'dell_installation_or_arming_or_reboot':False,'dell_brightness_verified':False,'coreanimation_timeouts_fixed_on_target':False,'weather_fixed_on_target':False,'new_target_performance_result':None,'total_render_latency_measured':False,'frozen_runtime_hashes':identity}
a=r/'port-artifacts/smoothness-brightness-0.3.13';a.mkdir(exist_ok=True);(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'smoothness-brightness-0.3.13-validation.json')
for f in files+[p/'SHA256SUMS.txt']:
 name='Desktop/'+str(f.relative_to(p));m[name]=sha(port/name)
for name in ['collect-desktop-failure.sh',doc,'smoothness-brightness-0.3.13-validation.json','PORT-STATUS.md']:m[name]=sha(port/name)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt');verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name,digest in m.items():assert sha(port/name)==digest,name
assert identity=={name:sha(port/'Desktop'/name) for name in frozen}
assert efi==inventory(u/'EFI')
print('Published candidate0.3.13: accelerator0.2.5, framebuffer0.2.6, Metal0.3.12 unchanged. Package/root/baseline manifests verified. Previous changed0.3.12 files+EFI backed up on USB and locally. EFI unchanged. No target installation, arming, reboot or hardware suite.')
