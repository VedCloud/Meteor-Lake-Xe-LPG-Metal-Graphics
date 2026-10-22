from pathlib import Path
import tempfile,subprocess,os,shutil,sys
r=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory(prefix='ws-installer-') as tmp:
 base=Path(tmp)
 for mode in ['backend-verify','identity-verify','transport-verify','previous','current','cache-repair-previous','success','branding-fail','bad-accel','current-accelerator','previous-accelerator','baseline','kernel','partial','verify','journal','kc-preflight','kc-commit','cache-repair','bad-path','bad-uuid','bad-framebuffer','final-build-fail']:
  p=base/mode;p.mkdir();pkg=p/'Desktop';pkg.mkdir();(p/'bin').mkdir();lib=p/'installed/Library'
  for root in [pkg/'Library',lib]:
   for name in ['NMXGPU','NMXAccel']:
    f=root/f'Extensions/{name}.kext/Contents/MacOS/{name}';f.parent.mkdir(parents=True);f.write_text('verified-'+name)
   f=root/'GPUBundles/nvmtl/libdrm.2.dylib';f.parent.mkdir(parents=True);f.write_text('verified-drm')
   (root/'GPUBundles/nvmtl/libvulkan_intel.dylib').write_text('verified-anv')
   f=root/'GPUBundles/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver';f.parent.mkdir(parents=True);f.write_text('old-plugin' if root==lib else 'new-plugin')
  f=pkg/'WindowServerTransport/libdrm.2.dylib';f.parent.mkdir(parents=True);f.write_text('new-drm')
  f=pkg/'WindowServerBackend/libvulkan_intel.dylib';f.parent.mkdir(parents=True);f.write_text('new-anv')
  shutil.copytree(pkg/'Library/GPUBundles/NVMTLDriver.bundle',pkg/'WindowServerPlugin/NVMTLDriver.bundle')
  accel=pkg/'WindowServerAccelerator/NMXAccel.kext/Contents/MacOS/NMXAccel';accel.parent.mkdir(parents=True);accel.write_text('new-accelerator')
  (pkg/'WindowServerAccelerator/NMXAccel-UUID.txt').write_text('22222222-3333-4444-5555-666666666666')
  (pkg/'WindowServerAccelerator/NMXAccel-VERSION.txt').write_text('0.2.6')
  prev=pkg/'PreviousWindowServerAccelerator';shutil.copytree(pkg/'WindowServerAccelerator',prev);(prev/'NMXAccel-VERSION.txt').write_text('0.2.5');(prev/'NMXAccel-UUID.txt').write_text('33333333-4444-5555-6666-777777777777');(prev/'NMXAccel.kext/Contents/MacOS/NMXAccel').write_text('previous-accelerator')

  for root in [pkg/'Library',lib]:
   f=root/'NullMoth/load-panel-framebuffer.sh';f.parent.mkdir(parents=True,exist_ok=True);f.write_text('#!/bin/bash\n# '+('old' if root==lib else 'new')+' loader\n')
   identity=root/'NullMoth/latitude-gpu-identity';identity.write_text('#!/bin/bash\n# '+('old' if root==lib else 'new')+' identity\n');identity.chmod(0o755)
   f=root/'NullMoth/latitude-desktop-journal';f.write_text('#!/bin/bash\n[ "$MODE" != journal ]\n' if root!=lib else '#!/bin/bash\n# old helper\n');f.chmod(0o755)
  for name in ['NMXGPU','NMXAccel']:(pkg/(name+'-UUID.txt')).write_text('verified-'+name+'-uuid\n')
  (pkg/'SHA256SUMS.txt').write_text('fixture');(pkg/'WINDOWSERVER-BASELINE-SHA256SUMS.txt').write_text('fixture')
  f=pkg/'WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU';f.parent.mkdir(parents=True);f.write_text('diagnostic-kernel')
  if mode=='cache-repair':(lib/'Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU').write_text('diagnostic-kernel')
  (pkg/'WindowServerKernel/NMXGPU-UUID.txt').write_text('E770B985-14A4-3ADE-A1EE-63204C3A9996')
  previous=pkg/'PreviousWindowServerKernel';shutil.copytree(pkg/'WindowServerKernel',previous)
  (previous/'NMXGPU.kext/Contents/MacOS/NMXGPU').write_text('previous-kernel')
  (previous/'NMXGPU-UUID.txt').write_text('previous-uuid')
  if mode in ['previous','cache-repair-previous']:(lib/'Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU').write_text('previous-kernel')
  if mode=='previous-accelerator':(lib/'Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel').write_text('previous-accelerator')
  if mode=='current-accelerator':(lib/'Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel').write_text('new-accelerator')
  if mode=='current':
   (lib/'Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU').write_text('diagnostic-kernel')
   (lib/'GPUBundles/nvmtl/libdrm.2.dylib').write_text('new-drm')
  system=p/'System'
  for cache in ['BootKernelExtensions.kc','SystemKernelExtensions.kc']:
   f=system/'KernelCollections'/cache;f.parent.mkdir(parents=True,exist_ok=True);f.write_text('system-cache')
  f=lib/'KernelCollections/AuxiliaryKernelExtensions.kc';f.parent.mkdir(parents=True);f.write_text('old-cache')
  helper=(r/'latitude-desktop/update-windowserver-kernel.sh').read_text().replace('/System/Library',str(system)).replace('/Library',str(lib))
  (pkg/'update-windowserver-kernel.sh').write_text(helper)
  s=(r/'latitude-desktop/install-windowserver.sh').read_text().replace('/Library',str(lib));s=s.replace('$HERE'+str(lib),'$HERE/Library');(pkg/'install-windowserver.sh').write_text(s)
  (pkg/'install-about-this-mac.sh').write_text('#!/bin/bash\n[ "$MODE" != branding-fail ]\n')
  scripts={'id':'echo 0','uname':'echo x86_64','sw_vers':'echo 24H32','ioreg':'case "$*" in *GFX0*) echo \'"device-id" = <457d0000>\';; esac','kcditto':'if [ "$MODE" = kc-commit ] && [ ! -f "$MOCK_ONCE" ]; then touch "$MOCK_ONCE"; exit 7; fi','kmutil':'if [ "$1" = create ]; then [ "$MODE" != kc-preflight ] || exit 6; while [ "$#" -gt 0 ]; do if [ "$1" = -A ]; then shift; echo new-cache > "$1"; break; fi; shift; done; exit 0; fi; if [ "$MODE" = kernel ]; then echo \'com.nullmoth.NMXGPU (1.0.7) wrong-uuid\'; else echo \'com.nullmoth.NMXGPU (1.0.8) verified-NMXGPU-uuid\'; fi; echo \'com.nullmoth.NMXAccel (0.2.4) verified-NMXAccel-uuid\'; echo com.dhinakg.AMFIPass','codesign':'if [ "$MODE" = verify ] && [ "${@: -1}" = "$MOCK_DEST" ]; then exit 1; fi; exit 0','shasum':'[ "$MODE" != baseline ] || exit 1; echo fixture-sha','chown':'exit 0','ditto':'exec /bin/cp -R "$1" "$2"','nvram':'echo "$*" >> "$MOCK_NVRAM"; if [ "$MODE" = partial ] && [ "$*" = nullmoth-panel-once=1 ]; then exit 1; fi; echo "$*"'}
  (pkg/'WindowServerKernel/NMXGPU-VERSION.txt').write_text('1.0.15')
  fb=pkg/'WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';fb.parent.mkdir(parents=True);fb.write_text('new-framebuffer')
  (pkg/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').write_text('11111111-2222-3333-4444-555555555555')
  (pkg/'WindowServerFramebuffer/LatitudeMTLFB-VERSION.txt').write_text('0.2.5')
  fb=lib/'Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';fb.parent.mkdir(parents=True,exist_ok=True);fb.write_text('old-framebuffer')
  fb=pkg/'Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';fb.parent.mkdir(parents=True,exist_ok=True);fb.write_text('old-framebuffer')
  prior=pkg/'PreviousWindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';prior.parent.mkdir(parents=True);prior.write_text('prior-framebuffer')
  if mode=='current':(lib/'Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB').write_text('prior-framebuffer')
  shutil.copy2(r/'latitude-desktop/validate-kernel-prelink',pkg/'validate-kernel-prelink')
  emitter=pkg/'emit-prelink';emitter.write_text('#!'+sys.executable+'\n'+"""import os,plistlib
entry={'CFBundleIdentifier':'com.nullmoth.NMXGPU','CFBundleVersion':'1.0.15','OSBundleUUID':bytes.fromhex('e770b98514a43adea1ee63204c3a9996'),'_PrelinkBundlePath':os.environ['MOCK_KERNEL_DEST'],'_PrelinkExecutableRelativePath':'Contents/MacOS/NMXGPU','_PrelinkExecutableSize':3071690}
if os.environ['MODE']=='bad-path':entry['_PrelinkBundlePath']='/temporary/NMXGPU.kext'
if os.environ['MODE']=='bad-uuid':entry['OSBundleUUID']=bytes(16)
import sys
fb=dict(entry,CFBundleIdentifier='com.nullmoth.LatitudeMTLFB',CFBundleVersion='0.2.5',OSBundleUUID=bytes.fromhex('11111111222233334444555555555555'),_PrelinkBundlePath=os.environ['MOCK_KERNEL_DEST'].replace('NMXGPU.kext','LatitudeMTLFB.kext'),_PrelinkExecutableRelativePath='Contents/MacOS/LatitudeMTLFB')
if os.environ['MODE']=='bad-framebuffer':fb['OSBundleUUID']=bytes(16)
ac=dict(entry,CFBundleIdentifier='com.nullmoth.NMXAccel',CFBundleVersion='0.2.6',OSBundleUUID=bytes.fromhex('22222222333344445555666666666666'),_PrelinkBundlePath=os.environ['MOCK_KERNEL_DEST'].replace('NMXGPU.kext','NMXAccel.kext'),_PrelinkExecutableRelativePath='Contents/MacOS/NMXAccel')
if os.environ['MODE']=='bad-accel':ac['OSBundleUUID']=bytes(16)
sys.stdout.buffer.write(plistlib.dumps({'_PrelinkInfoDictionary':[entry,fb,ac]}))
""");emitter.chmod(0o755)
  scripts['kmutil']='\nif [ "$1" = create ]; then\n  output=\'\'; paths=\'\'\n  while [ "$#" -gt 0 ]; do\n    case "$1" in -A) shift; output=$1;; -p) shift; paths="$paths $1";; esac\n    shift\n  done\n  case "$output" in *next-aux.kc) [ "$MODE" != preflight-fail ] && [ "$MODE" != kc-preflight ] || exit 6;; *installed-aux.kc) [ "$MODE" != final-build-fail ] || exit 7;; esac\n  printf \'new-cache\\n%s\\n%s\\n\' "$MOCK_UUID" "$paths" > "$output"\n  exit 0\nfi\nif [ "$1" = inspect ]; then exec "$MOCK_PRELINK_EMITTER"; fi\nif [ "$MODE" = kernel ]; then echo \'com.nullmoth.NMXGPU (1.0.7) wrong-uuid\'\nelif [ "$MODE" = previous ]; then echo \'com.nullmoth.NMXGPU (1.0.14) previous-uuid\'\nelif [ "$MODE" = current ]; then echo \'com.nullmoth.NMXGPU (1.0.15) E770B985-14A4-3ADE-A1EE-63204C3A9996\'\nelif [ "$MODE" != cache-repair ] && [ "$MODE" != cache-repair-previous ]; then echo \'com.nullmoth.NMXGPU (1.0.8) verified-NMXGPU-uuid\'; fi\necho \'com.nullmoth.NMXAccel (0.2.4) verified-NMXAccel-uuid\'\necho com.dhinakg.AMFIPass\n'
  scripts['kmutil']=scripts['kmutil'].replace("echo 'com.nullmoth.NMXAccel (0.2.4) verified-NMXAccel-uuid'", "if [ \"$MODE\" = current-accelerator ]; then echo 'com.nullmoth.NMXAccel (0.2.6) 22222222-3333-4444-5555-666666666666'; elif [ \"$MODE\" = previous-accelerator ]; then echo 'com.nullmoth.NMXAccel (0.2.5) 33333333-4444-5555-6666-777777777777'; else echo 'com.nullmoth.NMXAccel (0.2.4) verified-NMXAccel-uuid'; fi")
  scripts['codesign']='case "${@: -1}" in */installed/Library/GPUBundles/nvmtl/libvulkan_intel.dylib) [ "$MODE" != backend-verify ] || exit 1;; esac;\n'+scripts['codesign']
  scripts['codesign']='case "${@: -1}" in */installed/Library/NullMoth/latitude-gpu-identity) [ "$MODE" != identity-verify ] || exit 1;; esac;\n'+scripts['codesign']
  scripts['codesign']='if [ \"$MODE\" = transport-verify ] && [ \"${@: -1}\" = \"$MOCK_TRANSPORT\" ]; then exit 1; fi;\n'+scripts['codesign']
  scripts['nvram']='if [ "$*" = nullmoth-gpu-attaching ]; then exit 1; fi;\n'+scripts['nvram']
  for name,body in scripts.items():
   f=p/'bin'/name;f.write_text('#!/bin/bash\n'+body+'\n');f.chmod(0o755)
  dest=lib/'GPUBundles/NVMTLDriver.bundle';env=os.environ.copy();env.update(PATH=str(p/'bin')+':/usr/bin:/bin',MODE=mode,MOCK_PRELINK_EMITTER=str(emitter),MOCK_DEST=str(dest),MOCK_TRANSPORT=str(lib/'GPUBundles/nvmtl/libdrm.2.dylib'),MOCK_NVRAM=str(p/'nvram.txt'),MOCK_ONCE=str(p/'once'),MOCK_UUID='E770B985-14A4-3ADE-A1EE-63204C3A9996',MOCK_KERNEL_DEST=str(lib/'Extensions/NMXGPU.kext'))
  c=subprocess.run(['/bin/bash',str(pkg/'install-windowserver.sh')],env=env,capture_output=True,text=True,timeout=5)
  identity=(lib/'NullMoth/latitude-gpu-identity').read_text()
  plugin=(dest/'Contents/MacOS/NVMTLDriver').read_text();calls=(p/'nvram.txt').read_text().splitlines() if (p/'nvram.txt').exists() else []
  if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator']:assert c.returncode==(4 if mode=='branding-fail' else 0) and plugin=='new-plugin' and (['nullmoth-gpu-once=1','nullmoth-panel-once=1']==calls[2:4] if mode in ['cache-repair','cache-repair-previous'] else calls[:2]==['nullmoth-gpu-once=1','nullmoth-panel-once=1']),c.stdout+c.stderr
  else:
   assert c.returncode!=0 and plugin=='old-plugin',mode+c.stdout+c.stderr
   if mode in ['backend-verify','identity-verify','transport-verify','partial','verify','journal','kc-commit','bad-path','bad-uuid','bad-framebuffer','bad-accel','final-build-fail']:assert calls[-2:]==['-d nullmoth-panel-once','-d nullmoth-gpu-once'],(calls,c.stdout,c.stderr)
   else:assert not calls,calls
  assert (lib/'GPUBundles/nvmtl/libvulkan_intel.dylib').read_text()==('new-anv' if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'verified-anv'),mode
  assert ('# new identity' in identity)==(mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator']),mode
  assert (lib/'GPUBundles/nvmtl/libdrm.2.dylib').read_text()==('new-drm' if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'verified-drm'), mode+c.stdout+c.stderr
  assert (lib/'Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU').read_text()==('diagnostic-kernel' if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'verified-NMXGPU'),mode+c.stdout+c.stderr
  assert (lib/'Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB').read_text()==('new-framebuffer' if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'old-framebuffer'),mode+c.stdout+c.stderr
  assert (lib/'Extensions/NMXAccel.kext/Contents/MacOS/NMXAccel').read_text()==('new-accelerator' if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'verified-NMXAccel'),mode+c.stdout+c.stderr
  cache=(lib/'KernelCollections/AuxiliaryKernelExtensions.kc').read_text()
  if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator']:assert str(lib/'Extensions/NMXGPU.kext') in cache and '.latitude-windowserver.' not in cache and (pkg/'reports').is_dir(),cache
  else:assert cache=='old-cache',mode+c.stdout+c.stderr
  expected='new'  if mode in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator'] else 'old'
  assert expected+' loader' in (lib/'NullMoth/load-panel-framebuffer.sh').read_text()
  if mode not in ['success','cache-repair','previous','current','cache-repair-previous','branding-fail','current-accelerator','previous-accelerator']:assert 'old helper' in (lib/'NullMoth/latitude-desktop-journal').read_text()
  assert 'test-readback.sh'  not in s and 'test-and-arm-desktop.sh' not in s
 print('PASS actual desktop-only installer: baseline/kernel guards before mutation, plugin/capture/kernel/cache replacement/backup, one-shot arming, rollback and opt-in cleanup on verification/journal/kernel-preflight/kernel-mirroring/partial-arm failures; permanent final path/UUID verified, absent-loaded-kernel repair; no GPU suite')
