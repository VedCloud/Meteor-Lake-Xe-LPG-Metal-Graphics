from pathlib import Path
import os,subprocess,shutil,hashlib,json
b=Path(__file__).resolve().parent;pkg=b/'Tahoe-Combined-Runtime';root=None
for c in Path('/private/tmp').glob('dell-tahoe-combined-tests-*'):
 if (c/'success/Library/NullMoth/tahoe-backups').exists():root=c/'success'
assert root;p=root/'Package';cfg=root/'Volumes/EFI/EFI/OC/config.plist';audio=cfg.parent/'Kexts/SimpleHDA.kext'
for name in ['switch-graphics-mode.sh','resolve-usb-efi.sh']:
 f=p/name
 if f.exists():f.unlink()
 s=(pkg/name).read_text().replace('/usr/sbin/diskutil',str(root/'bin/diskutil'))
 if name=='switch-graphics-mode.sh':
  s=s.replace('"/Library/Extensions/', '"'+str(root)+'/Library/Extensions/')
 f.write_text(s)
for mode in ['accelerated','recovery']:
 f=p/'Profiles'/('pre-audio-'+mode+'.plist');shutil.copy2(pkg/'Profiles'/f.name,f)
# Re-sign no code: fixtures change only scripts/paths, and get their own checksums.
sums=p/'SHA256SUMS.txt';sums.unlink();sums.write_text(''.join(hashlib.sha256(f.read_bytes()).hexdigest()+'  '+str(f.relative_to(p))+'\n' for f in sorted(p.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.txt' and 'reports' not in f.relative_to(p).parts))
env=dict(os.environ,PATH=str(root/'bin')+':/usr/bin:/bin:/usr/sbin:/sbin',TEST_ROOT=str(root),TEST_PACKAGE=str(p),TEST_MODE='success');results=[]
for mode,with_audio in [('recovery',False),('recovery',True),('accelerated',True)]:
 if with_audio and not audio.exists():shutil.copytree(pkg/'EFI-Kexts/SimpleHDA.kext',audio)
 if mode=='accelerated':
  for n in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']:
   dst=root/'Library/Extensions'/(n+'.kext');shutil.rmtree(dst);shutil.copytree(pkg/'Library/Extensions'/(n+'.kext'),dst)
 run=subprocess.run(['/bin/bash',str(p/'switch-graphics-mode.sh'),mode],env=env,capture_output=True,text=True,timeout=20);assert run.returncode==0,run.stdout+run.stderr
 expected=pkg/'Profiles'/((mode if with_audio else 'pre-audio-'+mode)+'.plist');assert cfg.read_bytes()==expected.read_bytes()
 results.append({'mode':mode,'audioInstalled':with_audio,'passed':True});print('PASS',mode,'with audio' if with_audio else 'before audio')
(b/'COMBINED-SWITCH-TESTS.json').write_text(json.dumps({'tests':results,'scope':'Actual profile switch and OpenCore validator with only USB discovery and installed runtime paths redirected into disposable fixtures'},indent=2)+'\n')
