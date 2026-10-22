from pathlib import Path
import tempfile,subprocess,os,shutil,sys
r=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory(prefix='ws-kernel-') as d:
 for mode in ['success','preflight-fail','commit-fail','late-fail','already-new','missing-backup','bad-path','bad-uuid','bad-framebuffer','final-build-fail']:
  root=Path(d)/mode;root.mkdir();lib=root/'Library';system=root/'System';pkg=root/'package';work=root/'work';backup=root/'backup';report=root/'report';bin=root/'bin'
  for x in [work,backup,report,bin]:x.mkdir()
  for folder,text in [(pkg/'WindowServerKernel/NMXGPU.kext','new'),(lib/'Extensions/NMXGPU.kext','new' if mode=='already-new' else 'old')]:
   f=folder/'Contents/MacOS/NMXGPU';f.parent.mkdir(parents=True);f.write_text(text)
  for f in [system/'KernelCollections/BootKernelExtensions.kc',system/'KernelCollections/SystemKernelExtensions.kc',lib/'KernelCollections/AuxiliaryKernelExtensions.kc']:f.parent.mkdir(parents=True,exist_ok=True);f.write_text('old-cache')
  src=(r/'latitude-desktop/update-windowserver-kernel.sh').read_text().replace('/System/Library',str(system)).replace('/Library',str(lib))
  # Absolute roots are fixture-only; no actual /Library or boot cache operation.
  f=root/'helper.sh';f.write_text(src)
  mocks={'codesign':'exit 0','chown':'exit 0','ditto':'exec /bin/cp -R "$1" "$2"','kmutil':'echo create >> "$CALLS"; [ "$MODE" != preflight-fail ] || exit 7; while [ "$#" -gt 0 ]; do if [ "$1" = -A ]; then shift; echo new-cache > "$1"; break; fi; shift; done','kcditto':'echo kcditto >> "$CALLS"; if [ "$MODE" = commit-fail ] && [ ! -f "$ONCE" ]; then touch "$ONCE"; exit 8; fi'}
  (pkg/'WindowServerKernel/NMXGPU-VERSION.txt').write_text('1.0.15')
  fb=pkg/'WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';fb.parent.mkdir(parents=True);fb.write_text('new-framebuffer')
  (pkg/'WindowServerFramebuffer/LatitudeMTLFB-UUID.txt').write_text('11111111-2222-3333-4444-555555555555')
  (pkg/'WindowServerFramebuffer/LatitudeMTLFB-VERSION.txt').write_text('0.2.4')
  fb=lib/'Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB';fb.parent.mkdir(parents=True,exist_ok=True);fb.write_text('old-framebuffer')
  for folder,text in [(pkg/'WindowServerAccelerator/NMXAccel.kext','new-accelerator'),(lib/'Extensions/NMXAccel.kext','old-accelerator')]:
   f=folder/'Contents/MacOS/NMXAccel';f.parent.mkdir(parents=True);f.write_text(text)
  (pkg/'WindowServerAccelerator/NMXAccel-UUID.txt').write_text('33333333-2222-3333-4444-555555555555')
  (pkg/'WindowServerAccelerator/NMXAccel-VERSION.txt').write_text('0.2.4')
  shutil.copy2(r/'latitude-desktop/validate-kernel-prelink',pkg/'validate-kernel-prelink')
  emitter=pkg/'emit-prelink';emitter.write_text('#!'+sys.executable+'\n'+"""import os,plistlib
entry={'CFBundleIdentifier':'com.nullmoth.NMXGPU','CFBundleVersion':'1.0.15','OSBundleUUID':bytes.fromhex('e770b98514a43adea1ee63204c3a9996'),'_PrelinkBundlePath':os.environ['MOCK_KERNEL_DEST'],'_PrelinkExecutableRelativePath':'Contents/MacOS/NMXGPU','_PrelinkExecutableSize':3071690}
if os.environ['MODE']=='bad-path':entry['_PrelinkBundlePath']='/temporary/NMXGPU.kext'
if os.environ['MODE']=='bad-uuid':entry['OSBundleUUID']=bytes(16)
import sys
fb=dict(entry,CFBundleIdentifier='com.nullmoth.LatitudeMTLFB',CFBundleVersion='0.2.4',OSBundleUUID=bytes.fromhex('11111111222233334444555555555555'),_PrelinkBundlePath=os.environ['MOCK_KERNEL_DEST'].replace('NMXGPU.kext','LatitudeMTLFB.kext'),_PrelinkExecutableRelativePath='Contents/MacOS/LatitudeMTLFB')
if os.environ['MODE']=='bad-framebuffer':fb['OSBundleUUID']=bytes(16)
ac=dict(fb,CFBundleIdentifier='com.nullmoth.NMXAccel',OSBundleUUID=bytes.fromhex('33333333222233334444555555555555'),_PrelinkBundlePath=os.environ['MOCK_KERNEL_DEST'].replace('NMXGPU.kext','NMXAccel.kext'),_PrelinkExecutableRelativePath='Contents/MacOS/NMXAccel')
sys.stdout.buffer.write(plistlib.dumps({'_PrelinkInfoDictionary':[entry,fb,ac]}))
""");emitter.chmod(0o755)
  mocks['kmutil']='\nif [ "$1" = create ]; then\n  output=\'\'; paths=\'\'\n  while [ "$#" -gt 0 ]; do\n    case "$1" in -A) shift; output=$1;; -p) shift; paths="$paths $1";; esac\n    shift\n  done\n  case "$output" in *next-aux.kc) [ "$MODE" != preflight-fail ] && [ "$MODE" != kc-preflight ] || exit 6;; *installed-aux.kc) [ "$MODE" != final-build-fail ] || exit 7;; esac\n  printf \'new-cache\\n%s\\n%s\\n\' "$MOCK_UUID" "$paths" > "$output"\n  exit 0\nfi\nif [ "$1" = inspect ]; then exec "$MOCK_PRELINK_EMITTER"; fi\nif [ "$MODE" = kernel ]; then echo \'com.nullmoth.NMXGPU (1.0.7) wrong-uuid\'\nelif [ "$MODE" != cache-repair ]; then echo \'com.nullmoth.NMXGPU (1.0.8) verified-NMXGPU-uuid\'; fi\necho \'com.nullmoth.NMXAccel (0.2.4) verified-NMXAccel-uuid\'\necho com.dhinakg.AMFIPass\n'
  (pkg/'WindowServerKernel/NMXGPU-UUID.txt').write_text('E770B985-14A4-3ADE-A1EE-63204C3A9996')
  for name,body in mocks.items():f=bin/name;f.write_text('#!/bin/bash\n'+body+'\n');f.chmod(0o755)
  script=root/'run.sh';script.write_text('''#!/bin/bash
set -euo pipefail
source "$HELPER"
trap 'result=$?; trap - EXIT; if [ "$result" != 0 ]; then rollback_windowserver_kernel || exit 9; fi; exit "$result"' EXIT
prepare_windowserver_kernel
commit_windowserver_kernel
if [ "$MODE" = missing-backup ]; then rm -rf "$WORK/previous.kernel"; exit 6; fi
[ "$MODE" != late-fail ] || exit 6
''')
  env=os.environ.copy();env.update(HERE=str(pkg),WORK=str(work),BACKUP=str(backup),REPORT=str(report),HELPER=str(root/'helper.sh'),MODE=mode,MOCK_PRELINK_EMITTER=str(emitter),MOCK_UUID='E770B985-14A4-3ADE-A1EE-63204C3A9996',MOCK_KERNEL_DEST=str(lib/'Extensions/NMXGPU.kext'),CALLS=str(root/'calls'),ONCE=str(root/'once'),PATH=str(bin)+':/usr/bin:/bin')
  c=subprocess.run(['/bin/bash',str(script)],env=env,capture_output=True,text=True,timeout=5)
  current=(lib/'Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU').read_text();cache=(lib/'KernelCollections/AuxiliaryKernelExtensions.kc').read_text().strip()
  if mode in ['success','already-new']:assert c.returncode==0 and current=='new',c.stdout+c.stderr
  elif mode=='missing-backup':assert c.returncode==9 and current=='new' and cache.startswith('new-cache'),c.stdout+c.stderr
  else:assert c.returncode!=0 and current=='old' and cache=='old-cache',mode+c.stdout+c.stderr
  assert (lib/'Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB').read_text()==('new-framebuffer' if mode in ['success','already-new','missing-backup'] else 'old-framebuffer'),mode+c.stdout+c.stderr
  if mode in ['success','already-new']:assert cache.startswith('new-cache') and str(lib/'Extensions/NMXGPU.kext') in cache and str(work/'NMXGPU.kext') not in cache and (backup/'NMXGPU.kext/Contents/MacOS/NMXGPU').read_text()==('new' if mode=='already-new' else 'old')
 print('PASS actual kernel update helper with OS mocks: build-before-mutation, new kernel/cache commit, preflight failure unchanged, commit/late-failure rollback restores kernel+cache, identical-kernel cache rebuild, missing rollback source preserves installed kernel; no reboot/tests')
