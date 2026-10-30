#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Snapshot only a disposable recovery-image copy; never host filesystems.
Can safely resume the image left mounted by the earlier XML parsing failure.
"""
from pathlib import Path
import os,sys,subprocess,hashlib,struct,plistlib,json,tempfile,datetime,ctypes
R=Path('/Volumes/Dell27Build/FramebufferBuild');I=Path('/Volumes/Dell27Build/InstallerInputs/BaseSystem')
EXPECTED='cc280e2b8d70e2d7a5165e0581ca727fbe2d89db9002ca2b20481bb5f0d91e27'
def digest(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
 return h.hexdigest()
def run(args):
 p=subprocess.run(args,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if p.stderr:sys.stderr.write(p.stderr.decode(errors='replace'))
 if p.returncode:raise RuntimeError(f'{args[0]} failed ({p.returncode}): '+p.stdout.decode(errors='replace')+p.stderr.decode(errors='replace'))
 return p.stdout
def plist(args):return plistlib.loads(run(args))
def inventory():return plist(['/usr/bin/hdiutil','info','-plist'])['images']
def preflight():
 src=I/'BaseSystem-26A428.apfs';auth=I/'basesystem-26A428.root_hash'
 assert src.is_file() and not src.is_symlink() and digest(src)==EXPECTED,'source media digest mismatch'
 blob=auth.read_bytes();assert hashlib.sha256(blob).hexdigest()=='e3d407ccea8443db1ae557b5477b6cb7cea9309e7f5760c9658a9d1f7b3b0353'
 assert struct.unpack_from('<IIII',blob)==(2,0,1,32)
 return src,'com.apple.os.update-'+blob[16:48].hex().upper()
src,snapshot=preflight()
if '--check' in sys.argv:
 lib=ctypes.CDLL('/usr/lib/libSystem.B.dylib');assert lib.fs_snapshot_create
 print('Preflight PASS: source digest and native snapshot API verified; '+snapshot);sys.exit(0)
if os.geteuid()!=0:raise SystemExit('Administrator access is required for the native snapshot API. No changes made.')
out=R/'RecoverySnapshotExperiment';assert not out.is_symlink();out.mkdir(exist_ok=True)
image=out/'BaseSystem-26A428-with-root-snapshot.apfs';assert not image.is_symlink()
# Only resume an existing image when DiskImages proves it is our own mounted
# image. Unmounted existing files are preserved and never overwritten.
existing=next((x for x in inventory() if Path(x['image-path']).resolve()==image.resolve()),None)
if existing:
 assert image.is_file() and image.stat().st_size==src.stat().st_size
 mount=Path(next(e['mount-point'] for e in existing['system-entities'] if 'mount-point' in e))
 assert mount.parent.resolve()==R.resolve() and mount.name.startswith('dell27-snapshot-')
else:
 assert not image.exists(),'Refusing to overwrite an existing unmounted experiment image.'
 subprocess.run(['/bin/cp','-c',str(src),str(image)],check=True);assert digest(image)==EXPECTED
 mount=Path(tempfile.mkdtemp(prefix='dell27-snapshot-',dir=R))
result={'originalSHA256':EXPECTED,'snapshotName':snapshot,'originalModified':False,'physicalDiskAccess':False,'hostSecurityChanges':False,'snapshotCreated':False,'kernelBoot':False}
error=None
try:
 if not existing:run(['/usr/bin/hdiutil','attach','-readwrite','-owners','on','-nobrowse','-mountpoint',str(mount),'-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)])
 # Re-read authoritative inventory even if the attachment command emitted
 # warnings; stderr is deliberately never fed into the plist parser.
 own=next(x for x in inventory() if Path(x['image-path']).resolve()==image.resolve())
 device=next(e['dev-entry'] for e in own['system-entities'] if e.get('mount-point')==str(mount))
 props=plist(['/usr/sbin/diskutil','info','-plist',device])
 assert props.get('VolumeUUID','').upper()=='A9037730-2821-4F3A-90DC-91BA521BB080'
 assert props.get('MountPoint')==str(mount) and props.get('BusProtocol')=='Disk Image' and props.get('WritableVolume'),props
 snapshots=plist(['/usr/sbin/diskutil','apfs','listSnapshots',device,'-plist'])
 if not any(x.get('SnapshotName')==snapshot for x in snapshots.get('Snapshots',[])):
  lib=ctypes.CDLL('/usr/lib/libSystem.B.dylib',use_errno=True)
  lib.fs_snapshot_create.argtypes=[ctypes.c_int,ctypes.c_char_p,ctypes.c_uint32];lib.fs_snapshot_create.restype=ctypes.c_int
  fd=os.open(mount,os.O_RDONLY|os.O_DIRECTORY)
  try:
   if lib.fs_snapshot_create(fd,snapshot.encode(),0):
    err=ctypes.get_errno();raise OSError(err,os.strerror(err))
  finally:os.close(fd)
 snapshots=plist(['/usr/sbin/diskutil','apfs','listSnapshots',device,'-plist'])
 assert any(x.get('SnapshotName')==snapshot for x in snapshots.get('Snapshots',[])),snapshots
 result.update(snapshotCreated=True,volumeInfo=plist(['/usr/sbin/diskutil','info','-plist',device]),snapshotInfo=snapshots)
except Exception as exc:error=exc;result['error']=str(exc)
finally:
 # Cleanup is independent of attachment stdout parsing and cannot hide the
 # original exception. Never force an eject or delete a mounted directory.
 try:
  own=next((x for x in inventory() if Path(x['image-path']).resolve()==image.resolve()),None)
  if own:run(['/usr/bin/hdiutil','detach',next(e['dev-entry'] for e in own['system-entities'])])
  if mount.exists():mount.rmdir()
 except Exception as exc:result['cleanupError']=str(exc)
 assert digest(src)==EXPECTED,'original image changed unexpectedly'
 result['experimentSHA256']=digest(image);result['finishedUTC']=datetime.datetime.now(datetime.timezone.utc).isoformat()
 (out/'SNAPSHOT-PREP-RESULT.json').write_text(json.dumps(result,indent=2,default=str)+'\n')
if error:raise SystemExit('Snapshot experiment failed: '+str(error)+'; detailed report saved.')
print('Saved verified snapshot experiment:',out)
