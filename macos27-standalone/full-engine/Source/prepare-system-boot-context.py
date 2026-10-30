# SPDX-License-Identifier: MIT
"""Build real System/Data/Preboot metadata in a copy of our owned APFS image."""
from pathlib import Path
import subprocess,plistlib,json,hashlib,shutil
D=Path('HFSExperiment').resolve();old=json.loads((D/'BOOT-CONTEXT-PREPARATION.json').read_text());image=D/'SystemGroupBootContext-DataFirst.raw';assert not image.exists()
with image.open('xb') as f:f.truncate(2*1024*1024*1024)
def run(a):
 p=subprocess.run(a,capture_output=True)
 if p.returncode:raise RuntimeError(str(a)+': '+p.stdout.decode(errors='replace')+p.stderr.decode(errors='replace'))
 return p.stdout
def plist(a):return plistlib.loads(run(a))
a=plist(['hdiutil','attach','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(image)]);dev=a['system-entities'][0]['dev-entry'];ident=Path(dev).name;info=plist(['diskutil','info','-plist',dev]);assert info['BusProtocol']=='Disk Image' and info['TotalSize']==2*1024*1024*1024
inventory=plist(['hdiutil','info','-plist']);assert any(Path(x['image-path']).resolve()==image and any(y['dev-entry']==dev for y in x['system-entities']) for x in inventory['images'])
def container():return next(c for c in plist(['diskutil','apfs','list','-plist'])['Containers'] if any(p['DeviceIdentifier']==ident for p in c.get('PhysicalStores',[])))
run(['diskutil','apfs','createContainer',dev]);c=container()
run(['diskutil','apfs','addVolume',c['ContainerReference'],'APFS','Dell27 Boot Data','-role','D','-nomount']);c=container();data=next(v['DeviceIdentifier'] for v in c['Volumes'] if v['Roles']==['Data'])
run(['diskutil','apfs','addVolume',c['ContainerReference'],'APFS','Dell27 Boot System','-role','S','-groupWith',data,'-nomount']);c=container();system=next(v['DeviceIdentifier'] for v in c['Volumes'] if v['Roles']==['System'])
run(['diskutil','apfs','addVolume',c['ContainerReference'],'APFS','Dell27 Preboot','-role','B','-nomount']);c=container();groups=plist(['diskutil','apfs','listVolumeGroups','-plist',c['ContainerReference']]);(D/'SYSTEM-CONTEXT-VOLUME-GROUPS.json').write_text(json.dumps(groups,indent=2)+'\n');print(json.dumps(groups,indent=2),flush=True)
preboot=next(v for v in c['Volumes'] if v['Roles']==['Preboot']);system_info=plist(['diskutil','info','-plist',system]);group=system_info['APFSVolumeGroupID'];run(['diskutil','mount',preboot['DeviceIdentifier']]);pre_info=plist(['diskutil','info','-plist',preboot['DeviceIdentifier']]);mount=Path(pre_info['MountPoint']);path=Path(group)/'boot'/'26A428-vmapple';dest=mount/path;dest.mkdir(parents=True)
manifest=[]
for item in old['files']:
 src=Path(item['source']);target=dest/Path(item['path']).name;shutil.copy2(src,target);assert hashlib.sha256(target.read_bytes()).hexdigest()==item['SHA256'];manifest.append({**item,'path':str(path/target.name)})
(dest/'CONTENTS.json').write_text(json.dumps({'scope':'Supplied VMAPPLE boot objects; unsealed test volume group','files':manifest},indent=2)+'\n')
run(['hdiutil','detach',dev]);m={**old,'image':str(image),'imageSHA256':hashlib.sha256(image.read_bytes()).hexdigest(),'imageBytes':image.stat().st_size,'container':c,'volumeInfo':system_info,'prebootVolumeInfo':pre_info,'volumeGroupUUID':group,'bootContextUUID':group,'bootObjectsPath':'/'+str(path),'files':manifest,'layout':'System/Data volume group and Preboot; unsealed test filesystems','GPTAndRoleVerified':False,'detached':True,'bootObjectsPopulated':True};(D/'SYSTEM-BOOT-CONTEXT-PREPARATION.json').write_text(json.dumps(m,indent=2)+'\n');print('System/Data/Preboot prepared:',group)
