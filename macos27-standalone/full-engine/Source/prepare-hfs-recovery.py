#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Repackage the supplied recovery files into a separate HFS+ RAM-disk test.
Original APFS, host security and physical disks are never modified.
"""
from pathlib import Path
import os,sys,subprocess,plistlib,hashlib,json,stat,datetime
R=Path('/Volumes/Dell27Build/FramebufferBuild');D=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment');S=Path('/Volumes/macOS Base System')
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
def run(args):
 p=subprocess.run(args,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if p.stderr:sys.stderr.write(p.stderr.decode(errors='replace'))
 if p.returncode:raise RuntimeError(str(args)+': '+p.stdout.decode(errors='replace')+p.stderr.decode(errors='replace'))
 return p.stdout
def plist(args):return plistlib.loads(run(args))
assert os.geteuid()==0,'Administrator access is needed to preserve all guest-file ownership and read protected recovery directories.'
original=Path('/Volumes/Dell27Build/InstallerInputs/BaseSystem/BaseSystem-26A428.apfs');expected='cc280e2b8d70e2d7a5165e0581ca727fbe2d89db9002ca2b20481bb5f0d91e27';assert sha(original)==expected
props=plist(['/usr/sbin/diskutil','info','-plist',str(S)]);assert props['VolumeUUID'].upper()=='A9037730-2821-4F3A-90DC-91BA521BB080' and props['BusProtocol']=='Disk Image' and not props['WritableMedia']
version=plistlib.loads((S/'System/Library/CoreServices/SystemVersion.plist').read_bytes());assert version['ProductBuildVersion']=='26A428'
assert not D.is_symlink();D.mkdir(exist_ok=True);image=D/'BaseSystem-26A428-HFS.dmg';assert not image.exists(),'Existing HFS experiment preserved.'
log={'sourceAPFSSHA256':expected,'version':version,'originalModified':False,'hostSecurityChanges':False,'physicalDisksExposed':False,'installerReached':False,'repackagedFilesystem':'HFS+; unsealed bootstrap test','payloadVerified':False};mounted=None
try:
 if not props['GlobalPermissionsEnabled']:run(['/usr/sbin/diskutil','enableOwnership',str(S)])
 run(['/usr/bin/hdiutil','create','-srcfolder',str(S),'-srcowners','any','-anyowners','-noskipunreadable','-fs','HFS+','-layout','NONE','-size','3g','-format','UDRW','-volname','Dell27Recovery','-nospotlight',str(image)])
 info=plist(['/usr/bin/hdiutil','attach','-readonly','-owners','on','-nobrowse','-plist',str(image)]);mounted=info['system-entities'][0]['dev-entry'];target=Path(next(e['mount-point'] for e in info['system-entities'] if 'mount-point' in e));matches=[]
 for parent,dirs,files in os.walk(S,followlinks=False):
  for name in dirs+files:
   source=Path(parent)/name;relative=source.relative_to(S);dest=target/relative;a=source.lstat();b=dest.lstat();assert stat.S_IFMT(a.st_mode)==stat.S_IFMT(b.st_mode),str(relative)
   if source.is_symlink():assert os.readlink(source)==os.readlink(dest),str(relative)
   elif stat.S_ISREG(a.st_mode):
    h=sha(source);assert h==sha(dest),'file bytes changed: '+str(relative);matches.append({'path':str(relative),'SHA256':h,'mode':stat.S_IMODE(b.st_mode),'uid':b.st_uid,'gid':b.st_gid})
   assert (stat.S_IMODE(a.st_mode),a.st_uid,a.st_gid)==(stat.S_IMODE(b.st_mode),b.st_uid,b.st_gid),'ownership/mode changed: '+str(relative)
 log.update(payloadVerified=True,regularFilesVerified=len(matches),volumeInfo=plist(['/usr/sbin/diskutil','info','-plist',str(target)]))
 (D/'PAYLOAD-MANIFEST.json').write_text(json.dumps(matches,indent=2)+'\n')
except Exception as exc:log['error']=str(exc);raise
finally:
 if not props['GlobalPermissionsEnabled']:run(['/usr/sbin/diskutil','disableOwnership',str(S)])
 if mounted:run(['/usr/bin/hdiutil','detach',mounted])
 assert sha(original)==expected
 if image.exists():log.update(imageSHA256=sha(image),imageBytes=image.stat().st_size)
 log['finishedUTC']=datetime.datetime.now(datetime.timezone.utc).isoformat();(D/'HFS-PREPARATION.json').write_text(json.dumps(log,indent=2,default=str)+'\n')
print('Verified complete recovery payload in HFS+ experiment:',D)
