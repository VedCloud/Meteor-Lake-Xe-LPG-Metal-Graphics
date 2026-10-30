# SPDX-License-Identifier: MIT
"""Stage the observer in the recovery System launch-daemon directory.
Only a disposable user-owned image is changed; staging BSD metadata is restored.
"""
from pathlib import Path
import json,os,struct,subprocess,plistlib,hashlib
from hfs_owned_image_metadata import OwnedHFS
R=Path(__file__).resolve().parent;old=json.loads((R/'PENDING-DIAGNOSTICS-IMAGE.json').read_text());src=Path(old['image']);image=src.with_name('BaseSystem-26A428-HFS-Diagnostics-v3.dmg');assert not image.exists();subprocess.run(['cp','-c',str(src),str(image)],check=True)
folders=['System/Library/Templates/Data/Library/LaunchDaemons','System/Library/LaunchDaemons'];rel='com.dell27.bootdiagnostics.plist';h=OwnedHFS(image);saved={}
for name in folders:
 b=h.bsd_info(name);saved[name]=b.hex();assert struct.unpack_from('>II',b)==(0,0);stage=bytearray(b);struct.pack_into('>II',stage,0,os.getuid(),os.getgid());stage[8]&=~8;h.bsd_info(name,stage)
h.close()
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]));dev=a['system-entities'][0]['dev-entry'];mount=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point' in e));inventory=plistlib.loads(subprocess.check_output(['hdiutil','info','-plist']));assert any(Path(x['image-path']).resolve()==image and any(e['dev-entry']==dev for e in x['system-entities']) for x in inventory['images'])
try:
 source=mount/folders[0]/rel;dest=mount/folders[1]/rel;b=source.read_bytes();assert not dest.exists() and plistlib.loads(b)['Label']=='com.dell27.bootdiagnostics';dest.write_bytes(b);dest.chmod(0o644);source.unlink()
finally:subprocess.run(['hdiutil','detach',dev],capture_output=True,check=True)
h=OwnedHFS(image);h.owner(folders[1]+'/'+rel,(0,0))
for name,raw in saved.items():h.bsd_info(name,bytes.fromhex(raw));assert h.bsd_info(name).hex()==raw
h.close()
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(Path(old['source']))=='4d00e3db1ace1db545f2878eba08efa6b13f5b97fccd0dd5fad082eb2ec0341a'
m={**old,'image':str(image),'imageSHA256':sha(image),'version':3,'addedFiles':[{'path':folders[1]+'/'+rel,'bytes':len(b),'SHA256':hashlib.sha256(b).hexdigest(),'UID':0,'GID':0,'mode':'0644'}],'temporaryGuestDirectoryBSDInfoRestored':saved,'observerOperational':False,'activeTestUsesThisImage':False}
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]));dev=a['system-entities'][0]['dev-entry']
try:
 check=subprocess.run(['diskutil','verifyVolume',dev],capture_output=True,text=True);m['filesystemCheck']={'exitCode':check.returncode,'output':check.stdout+check.stderr};assert check.returncode==0,m['filesystemCheck']
finally:subprocess.run(['hdiutil','detach',dev],capture_output=True,check=True)
(R/'PENDING-DIAGNOSTICS-IMAGE.json').write_text(json.dumps(m,indent=2)+'\n');print('V3 System-directory collector staged, ownership/flags restored, HFS verified. Not armed.')
