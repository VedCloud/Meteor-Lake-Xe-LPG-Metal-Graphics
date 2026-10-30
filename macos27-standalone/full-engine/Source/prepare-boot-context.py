# SPDX-License-Identifier: MIT
"""Create an actual Recovery-role APFS volume in a disposable raw image."""
from pathlib import Path
import subprocess,plistlib,json,os
D=Path('HFSExperiment').resolve(); image=D/'RecoveryContext.raw'; evidence=D/'BOOT-CONTEXT-PREPARATION.json'
assert not image.exists()
with image.open('xb') as f:f.truncate(512*1024*1024)
def run(a):
 p=subprocess.run(a,capture_output=True);print(p.stdout.decode(errors='replace'),flush=True)
 if p.returncode:raise RuntimeError(str(a)+': '+p.stderr.decode(errors='replace'))
 return p.stdout
def plist(a):
 p=subprocess.run(a,capture_output=True,check=True);return plistlib.loads(p.stdout)
a=plist(['hdiutil','attach','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)])
dev=a['system-entities'][0]['dev-entry']; ident=Path(dev).name
info=plist(['diskutil','info','-plist',dev]);assert info['BusProtocol']=='Disk Image' and info['TotalSize']==512*1024*1024
inventory=plist(['hdiutil','info','-plist']);assert any(Path(x['image-path']).resolve()==image and any(y['dev-entry']==dev for y in x['system-entities']) for x in inventory['images'])
run(['diskutil','apfs','createContainer',dev])
containers=plist(['diskutil','apfs','list','-plist'])['Containers']; c=next(c for c in containers if any(p['DeviceIdentifier']==ident for p in c.get('PhysicalStores',[])))
run(['diskutil','apfs','addVolume',c['ContainerReference'],'APFS','Dell27RecoveryContext','-role','R','-nomount'])
c=next(x for x in plist(['diskutil','apfs','list','-plist'])['Containers'] if x['ContainerReference']==c['ContainerReference']);assert len(c['Volumes'])==1
v=c['Volumes'][0];assert v['Roles']==['Recovery'];run(['diskutil','mount',v['DeviceIdentifier']])
p=plist(['diskutil','info','-plist',v['DeviceIdentifier']]);evidence.write_text(json.dumps({'image':str(image),'container':c,'volumeInfo':p,'mountPoint':p['MountPoint'],'physicalDisksModified':False,'bootObjectsPopulated':False},indent=2)+'\n');print('Recovery-role APFS image ready:',evidence)
