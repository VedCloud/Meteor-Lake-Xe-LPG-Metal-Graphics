# SPDX-License-Identifier: MIT
from pathlib import Path
import os,json,plistlib,subprocess,hashlib
from hfs_owned_image_metadata import OwnedHFS
m=json.load(open('PENDING-DIAGNOSTICS-IMAGE.json'));image=Path(m['image']);folder='System/Library/Templates/Data/Library/LaunchDaemons';rel=folder+'/com.dell27.bootdiagnostics.plist'
h=OwnedHFS(image);assert h.owner(folder)['previousUIDGID']==(0,0);staging=h.owner(folder,(os.getuid(),os.getgid()));h.close()
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]));dev=a['system-entities'][0]['dev-entry'];mount=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point' in e));p=mount/rel
script='''i=0
while [ "$i" -lt 40 ]; do
 echo "DELL27 GUEST DIAGNOSTICS BEGIN iteration=$i"
 /bin/date -u
 /bin/ps -axo pid,ppid,state,comm
 for cls in IOFramebuffer IOMobileFramebuffer IOAccelerator IOGPU; do
  echo "DELL27 IOREG CLASS $cls"
  /usr/sbin/ioreg -r -l -w 0 -d 2 -c "$cls"
 done
 echo "DELL27 CRASH REPORT DIRECTORY"
 /bin/ls -lt /Library/Logs/DiagnosticReports /var/log 2>&1 | /usr/bin/head -n 25
 if [ -r /var/log/WindowServer.log ]; then /usr/bin/tail -n 80 /var/log/WindowServer.log; fi
 echo "DELL27 GUEST DIAGNOSTICS END iteration=$i"
 i=$((i + 1))
 /bin/sleep 30
done
'''
b=plistlib.dumps({'Label':'com.dell27.bootdiagnostics','ProgramArguments':['/bin/sh','-c',script],'RunAtLoad':True,'ProcessType':'Background','StandardOutPath':'/dev/console','StandardErrorPath':'/dev/console'})
try:
 assert p.parent.stat().st_uid==os.getuid() and not p.exists();p.write_bytes(b);p.chmod(0o644);assert plistlib.loads(p.read_bytes())['RunAtLoad']
finally:subprocess.run(['hdiutil','detach',dev],capture_output=True,check=True)
h=OwnedHFS(image);file_owner=h.owner(rel,(0,0));h.owner(folder,(0,0));assert h.owner(rel)['previousUIDGID']==(0,0) and h.owner(folder)['previousUIDGID']==(0,0);h.close()
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(Path(m['source']))=='4d00e3db1ace1db545f2878eba08efa6b13f5b97fccd0dd5fad082eb2ec0341a'
m.update(detached=True,imageSHA256=sha(image),originalUnchanged=True,addedFiles=[{'path':rel,'bytes':len(b),'SHA256':hashlib.sha256(b).hexdigest(),'UID':0,'GID':0,'mode':'0644'}],stagingOwnershipRestored=True,guestOnly=True,activeTestUsesThisImage=False)
Path('PENDING-DIAGNOSTICS-IMAGE.json').write_text(json.dumps(m,indent=2)+'\n');print('Read-only guest collector staged; original image and active test unchanged.')
