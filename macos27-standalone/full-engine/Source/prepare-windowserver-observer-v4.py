# SPDX-License-Identifier: MIT
"""Add a bounded, read-only WindowServer observer to an owned guest-image clone."""
from pathlib import Path
import json,os,struct,subprocess,plistlib,hashlib
from hfs_owned_image_metadata import OwnedHFS
R=Path(__file__).resolve().parent
old=json.loads((R/'PENDING-DIAGNOSTICS-IMAGE.json').read_text())
assert old['version']==3
src=Path(old['image']);image=src.with_name('BaseSystem-26A428-HFS-Diagnostics-v4.dmg')
assert not image.exists()
subprocess.run(['cp','-c',str(src),str(image)],check=True)
folder='System/Library/LaunchDaemons';rel=folder+'/com.dell27.bootdiagnostics.plist'
h=OwnedHFS(image);saved=h.bsd_info(folder);assert struct.unpack_from('>II',saved)==(0,0)
stage=bytearray(saved);struct.pack_into('>II',stage,0,os.getuid(),os.getgid());stage[8]&=~8;h.bsd_info(folder,stage);h.close()
script='''echo "DELL27 WINDOWSERVER OBSERVER V4 BEGIN"
/bin/date -u
/bin/ps -axo pid,ppid,state,comm
i=0
ws=
while [ "$i" -lt 256 ]; do
 processes=$(/bin/ps -axo pid=,comm=)
 while read -r pid comm; do
  case "$comm" in
   */WindowServer) ws="$pid" ;;
  esac
 done <<PROCESS_LIST
$processes
PROCESS_LIST
 if [ -n "$ws" ]; then break; fi
 case "$i" in 0|16|32|48|64|80|96|112|128|144|160|176|192|208|224|240) echo "DELL27 WAITING FOR WINDOWSERVER iteration=$i" ;; esac
 i=$((i + 1))
done
if [ -n "$ws" ]; then
 echo "DELL27 WINDOWSERVER FOUND pid=$ws iteration=$i"
 /bin/date -u
 /bin/ps -axo pid,ppid,state,comm
 for cls in IOFramebuffer IOMobileFramebuffer IOAccelerator IOGPU IOHIDSystem; do
  echo "DELL27 LIVE IOREG CLASS $cls"
  /usr/sbin/ioreg -r -l -w 0 -d 3 -c "$cls"
 done
 echo "DELL27 WINDOWSERVER STACK BEGIN pid=$ws"
 /usr/bin/sample "$ws" 1 20 -file /dev/stdout 2>&1
 echo "DELL27 WINDOWSERVER STACK END pid=$ws"
 /bin/ls -lt /Library/Logs/DiagnosticReports /var/log 2>&1
else
 echo "DELL27 WINDOWSERVER NOT FOUND WITHIN BOUNDED PROCESS OBSERVATIONS"
fi
echo "DELL27 WINDOWSERVER OBSERVER V4 END"
'''
b=plistlib.dumps({'Label':'com.dell27.bootdiagnostics','ProgramArguments':['/bin/sh','-c',script],'RunAtLoad':True,'ProcessType':'Background','StandardOutPath':'/dev/console','StandardErrorPath':'/dev/console'})
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]));dev=a['system-entities'][0]['dev-entry'];mount=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point' in e))
try:
 p=mount/rel;assert p.is_file() and p.parent.stat().st_uid==os.getuid();p.unlink();p.write_bytes(b);p.chmod(0o644)
finally:subprocess.run(['hdiutil','detach',dev],capture_output=True,check=True)
h=OwnedHFS(image);h.owner(rel,(0,0));h.bsd_info(folder,saved);assert h.bsd_info(folder)==saved;h.close()
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
 return h.hexdigest()
assert sha(src)==old['imageSHA256']
m={**old,'image':str(image),'imageSHA256':sha(image),'version':4,'observerOperational':False,'addedFiles':[{'path':rel,'bytes':len(b),'SHA256':hashlib.sha256(b).hexdigest(),'UID':0,'GID':0,'mode':'0644'}],'temporaryGuestDirectoryBSDInfoRestored':{folder:saved.hex()},'observerPurpose':'Read-only bounded ps observations, live framebuffer registry and one WindowServer stack; no sleep/pgrep dependency; no service policy changes.'}
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]));dev=a['system-entities'][0]['dev-entry']
try:
 check=subprocess.run(['diskutil','verifyVolume',dev],capture_output=True,text=True);m['filesystemCheck']={'exitCode':check.returncode,'output':check.stdout+check.stderr};assert check.returncode==0,m['filesystemCheck']
finally:subprocess.run(['hdiutil','detach',dev],capture_output=True,check=True)
(R/'GUEST-OBSERVER-v3-PREPARATION.json').write_text(json.dumps(old,indent=2)+'\n')
(R/'PENDING-DIAGNOSTICS-IMAGE.json').write_text(json.dumps(m,indent=2)+'\n')
print('V4 WindowServer observer staged; original and V3 images unchanged; HFS verified.')
