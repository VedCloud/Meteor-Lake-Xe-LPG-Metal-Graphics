from pathlib import Path
import hashlib,json,shutil,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop';u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';b=port/'backups/nightshift-audio-before-0.3.16'
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
assert sha(port/'Desktop/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver')=='9d7f675e10c30f77ab4ca26cf8898297dcc29f70992eafe524b41d58626bc88e'
assert sha(port/'Desktop/WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')==sha(p/'PreGammaWindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB')
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
(b/'BACKUP.json').write_text(json.dumps({'candidate':'0.3.15','basis_report':'failure-detail-20261020-172853','scope':'EFI and files replaced by 0.3.16; not an OS/AuxKC backup','prior_file_hashes':{name:sha(port/name) for name in changed if (port/name).is_file()},'new_paths_previously_absent':[name for name in changed if not (port/name).exists()],'frozen_runtime_hashes':identity,'efi_hashes':efi},indent=2)+'\n')
local=r/'backups/nightshift-audio-before-0.3.16';assert not local.exists();shutil.copytree(b,local)
for name in ['install-windowserver.sh','update-windowserver-kernel.sh']:subprocess.run(['/bin/bash','-n',str(p/name)],check=True)
subprocess.run(['/bin/bash','-n',str(r/'desktop-sources/collect-desktop-failure.sh')],check=True)
for name in ['WindowServerAccelerator/NMXAccel.kext','WindowServerFramebuffer/LatitudeMTLFB.kext','WindowServerPlugin/NVMTLDriver.bundle','Audio/AppleALC.kext','latitude-frame-pacing']:subprocess.run(['codesign','--verify','--strict',str(p/name)],check=True)
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
 dest=port/'Desktop'/f.relative_to(p)
 if not dest.exists() or sha(dest)!=sha(f):copy(f,dest)
copy(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
doc='nightshift-and-audio-0.3.16.md';copy(r/'research'/doc,port/doc)
status=port/'PORT-STATUS.md';status.write_text((r/'research'/doc).read_text()+'\n---\n\n'+status.read_text())
v={'candidate':'0.3.16','accelerator':'0.2.7','framebuffer':'0.2.8','plugin':'0.3.15 unchanged','gpu_kernel':'1.0.15 unchanged','accelerator_uuid':(p/'WindowServerAccelerator/NMXAccel-UUID.txt').read_text().strip(),'framebuffer_uuid':(p/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').read_text().strip(),'audio':'AppleALC1.9.8; ALC295 confirmed by user; related-Dell layout33; controller51CA compatibility identity','official_applealc_zip_sha256':'d4d36cd2da7863cf7cbfcb0892d45c08457f4f4ddd065a073dd010e46bd63bcb','new_imports':{'accelerator':['__ZNK9IOService10isInactiveEv'],'framebuffer':['_memcmp']},'tests':['x86_64 framebuffer/accelerator compilation and signatures','actual gamma decoder/copy ASan/UBSan integer widths/identity/warmth/alpha/padding/refusals','actual framebuffer gamma setter generation/reset/error fixture','actual front timer idle gamma generation update','installer guard/rollback/prelink and collector retention fixtures','audio plan official release digest, supported layout/controller resource and signature validation','official ocvalidate1.0.8'],'nightshift_verified_on_dell':False,'sound_verified_on_dell':False,'audio_pinmap_verified_on_dell':False,'nonidentity_gamma_adds_cpu_work':True,'full_hardware_suite_rerun':False,'dell_installation_or_arming_or_reboot':False,'keyboard_touchpad_configuration_changed':False,'frozen_runtime_hashes':identity}
a=r/'port-artifacts/nightshift-audio-0.3.16';a.mkdir(exist_ok=True);(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'nightshift-audio-0.3.16-validation.json')
for f in files+[p/'SHA256SUMS.txt']:
 name='Desktop/'+str(f.relative_to(p));m[name]=sha(port/name)
for name in ['collect-desktop-failure.sh',doc,'nightshift-audio-0.3.16-validation.json','PORT-STATUS.md']:m[name]=sha(port/name)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt');verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name,digest in m.items():assert sha(port/name)==digest,name
assert identity=={name:sha(port/'Desktop'/name) for name in frozen}
# Add only the reviewed audio configuration and AppleALC bundle.
import plistlib
oldconfig=(u/'EFI/OC/config.plist').read_bytes();oc=plistlib.loads(oldconfig);nc=plistlib.loads((p/'Audio/config.plist').read_bytes())
entry=nc['Kernel']['Add'].pop();props=nc['DeviceProperties']['Add'].pop('PciRoot(0x0)/Pci(0x1F,0x3)')
assert entry['BundlePath']=='AppleALC.kext' and entry['Enabled'] and nc==oc
assert props=={'device-id':bytes.fromhex('ca510000'),'layout-id':(33).to_bytes(4,'little'),'alc-layout-id':(33).to_bytes(4,'little')}
old_alc_exists=(u/'EFI/OC/Kexts/AppleALC.kext').exists();old_alc_backup=b/'AppleALC-at-commit.kext'
assert not old_alc_backup.exists()
try:
 if old_alc_exists:(u/'EFI/OC/Kexts/AppleALC.kext').rename(old_alc_backup)
 shutil.copytree(p/'Audio/AppleALC.kext',u/'EFI/OC/Kexts/AppleALC.kext')
 copy(p/'Audio/config.plist',u/'EFI/OC/config.plist')
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(u/'EFI/OC/config.plist')],check=True)
 subprocess.run(['codesign','--verify','--strict',str(u/'EFI/OC/Kexts/AppleALC.kext')],check=True)
 after=inventory(u/'EFI');changed={n for n in set(efi)|set(after) if efi.get(n)!=after.get(n)}
 expected={'OC/Kexts/AppleALC.kext/'+n for n in inventory(p/'Audio/AppleALC.kext')}|{'OC/config.plist'}
 allowed=expected|{n for n in efi if n.startswith('OC/Kexts/AppleALC.kext/')}
 assert changed<=allowed and 'OC/config.plist' in changed,changed
 assert inventory(u/'EFI/OC/Kexts/AppleALC.kext')==inventory(p/'Audio/AppleALC.kext')
except BaseException:
 copy(b/'EFI/OC/config.plist',u/'EFI/OC/config.plist')
 if (u/'EFI/OC/Kexts/AppleALC.kext').exists():shutil.rmtree(u/'EFI/OC/Kexts/AppleALC.kext')
 if old_alc_exists and old_alc_backup.exists():old_alc_backup.rename(u/'EFI/OC/Kexts/AppleALC.kext')
 raise
v['efi_before_hashes']=efi;v['efi_after_hashes']=after
(a/'VALIDATION.json').write_text(json.dumps(v,indent=2)+'\n');copy(a/'VALIDATION.json',port/'nightshift-audio-0.3.16-validation.json')
m['nightshift-audio-0.3.16-validation.json']=sha(port/'nightshift-audio-0.3.16-validation.json');(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
print('Published0.3.16: scanout gamma/Night Shift candidate and ALC295 AppleALC configuration. Prior0.3.15 full EFI/replaced package files backed up locally and on USB. Manifests/baseline/signatures/config verified; only planned audio EFI differences. No target installation, arming, reboot or GPU suite.')
