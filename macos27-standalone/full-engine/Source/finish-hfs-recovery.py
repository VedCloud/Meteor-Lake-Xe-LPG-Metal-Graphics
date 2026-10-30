#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
from pathlib import Path
import os,subprocess,plistlib,json,hashlib,stat,datetime
R=Path('/Volumes/Dell27Build/FramebufferBuild');D=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment');S=Path('/Volumes/macOS Base System');T=Path('/Volumes/Dell27Recovery');image=D/'BaseSystem-26A428-HFS.dmg';out=R/'HFSProof';out.mkdir(exist_ok=True)
def run(args):
 p=subprocess.run(args,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if p.returncode:raise RuntimeError(str(args)+': '+p.stderr.decode(errors='replace')+p.stdout.decode(errors='replace'))
 return p.stdout
def plist(args):return plistlib.loads(run(args))
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert os.geteuid()==0
original=Path('/Volumes/Dell27Build/InstallerInputs/BaseSystem/BaseSystem-26A428.apfs');expected='cc280e2b8d70e2d7a5165e0581ca727fbe2d89db9002ca2b20481bb5f0d91e27';assert sha(original)==expected
inventory=plist(['/usr/bin/hdiutil','info','-plist']);own=next(x for x in inventory['images'] if Path(x['image-path']).resolve()==image.resolve());device=next(e['dev-entry'] for e in own['system-entities'] if e.get('mount-point')==str(T));props=plist(['/usr/sbin/diskutil','info','-plist',device]);assert props['BusProtocol']=='Disk Image' and props['WritableVolume'] and props['VolumeName']=='Dell27Recovery'
sp=plist(['/usr/sbin/diskutil','info','-plist',str(S)]);assert sp['VolumeUUID'].upper()=='A9037730-2821-4F3A-90DC-91BA521BB080' and not sp['WritableMedia']
log={'sourceAPFSSHA256':expected,'version':plistlib.loads((S/'System/Library/CoreServices/SystemVersion.plist').read_bytes()),'originalModified':False,'hostSecurityChanges':False,'physicalDisksExposed':False,'installerReached':False,'repackagedFilesystem':'HFS+; unsealed bootstrap test','payloadVerified':False}
try:
 if not sp['GlobalPermissionsEnabled']:run(['/usr/sbin/diskutil','enableOwnership',str(S)])
 run(['/usr/sbin/diskutil','enableOwnership',str(T)])
 # DiskImages' from-folder builder excludes some root folders such as cores.
 # ditto fills those omissions and preserves every supplied filesystem object.
 run(['/usr/bin/ditto','--rsrc','--extattr','--acl',str(S),str(T)])
 matches=[]
 def err(e):raise e
 for parent,dirs,files in os.walk(S,followlinks=False,onerror=err):
  for name in dirs+files:
   source=Path(parent)/name;relative=source.relative_to(S);dest=T/relative;a=source.lstat();b=dest.lstat();assert stat.S_IFMT(a.st_mode)==stat.S_IFMT(b.st_mode),str(relative)
   if source.is_symlink():assert os.readlink(source)==os.readlink(dest),str(relative)
   elif stat.S_ISREG(a.st_mode):
    h=sha(source);assert h==sha(dest),'file bytes changed: '+str(relative);matches.append({'path':str(relative),'SHA256':h,'mode':stat.S_IMODE(b.st_mode),'uid':b.st_uid,'gid':b.st_gid})
   assert (stat.S_IMODE(a.st_mode),a.st_uid,a.st_gid)==(stat.S_IMODE(b.st_mode),b.st_uid,b.st_gid),'ownership/mode changed: '+str(relative)
 log.update(payloadVerified=True,regularFilesVerified=len(matches),volumeInfo=plist(['/usr/sbin/diskutil','info','-plist',str(T)]))
 (out/'PAYLOAD-MANIFEST.json').write_text(json.dumps(matches,indent=2)+'\n')
except Exception as exc:log['error']=str(exc);raise
finally:
 if not sp['GlobalPermissionsEnabled']:run(['/usr/sbin/diskutil','disableOwnership',str(S)])
 run(['/usr/bin/hdiutil','detach',own['system-entities'][0]['dev-entry']])
 assert sha(original)==expected;log['finishedUTC']=datetime.datetime.now(datetime.timezone.utc).isoformat();(out/'HFS-PREPARATION.json').write_text(json.dumps(log,indent=2,default=str)+'\n')
print('Complete recovery payload verified:',out)
