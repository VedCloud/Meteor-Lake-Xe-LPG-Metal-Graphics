# SPDX-License-Identifier: MIT
from pathlib import Path
import json,uuid,struct,zlib,hashlib,subprocess,plistlib
D=Path('HFSExperiment').resolve();meta=json.loads((D/'SYSTEM-BOOT-CONTEXT-PREPARATION.json').read_text());src=Path(meta['image']);assert meta['detached'] and hashlib.sha256(src.read_bytes()).hexdigest()==meta['imageSHA256']
dst=D/'SystemGroupBootContext.gpt';assert not dst.exists();start=2048;n=src.stat().st_size//512;total=start+n+2048;last=total-1;diskguid=uuid.uuid4();partguid=uuid.uuid4();ptype=uuid.UUID('7C3457EF-0000-11AA-AA11-00306543ECAC')
entries=bytearray(128*128);name='Dell27 System Boot Context'.encode('utf-16-le');struct.pack_into('<16s16sQQQ',entries,0,ptype.bytes_le,partguid.bytes_le,start,start+n-1,0);entries[56:56+len(name)]=name;ec=zlib.crc32(entries)&0xffffffff
def header(at,other,array):
 h=bytearray(512);struct.pack_into('<8sIIIIQQQQ16sQIII',h,0,b'EFI PART',0x10000,92,0,0,at,other,34,last-33,diskguid.bytes_le,array,128,128,ec);struct.pack_into('<I',h,16,zlib.crc32(h[:92])&0xffffffff);return h
with dst.open('xb') as f:
 f.truncate(total*512);mbr=bytearray(512);struct.pack_into('<B3sB3sII',mbr,446,0,b'\0\2\0',0xee,b'\xff\xff\xff',1,min(total-1,0xffffffff));mbr[510:]=b'\x55\xaa';f.write(mbr);f.write(header(1,last,2));f.write(entries);f.seek(start*512)
 with src.open('rb') as image:
  for b in iter(lambda:image.read(1024*1024),b''):f.write(b)
 f.seek((last-32)*512);f.write(entries);f.seek(last*512);f.write(header(last,1,last-32))
# Prove GPT and APFS role are recognized by the real parser, without mounting.
a=plistlib.loads(subprocess.check_output(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)]));dev=a['system-entities'][0]['dev-entry']
try:
 c=plistlib.loads(subprocess.check_output(['diskutil','apfs','list','-plist']))['Containers'];c=next(c for c in c if any('/dev/'+p['DeviceIdentifier'] in [x['dev-entry'] for x in a['system-entities']] for p in c.get('PhysicalStores',[])));v=next(v for v in c['Volumes'] if v['Roles']==['System']);assert v['APFSVolumeUUID']==meta['volumeInfo']['VolumeUUID'];assert sorted(tuple(v['Roles']) for v in c['Volumes'])==[('Data',),('Preboot',),('System',)]
 groups=plistlib.loads(subprocess.check_output(['diskutil','apfs','listVolumeGroups','-plist',c['ContainerReference']]));assert groups['Containers'][0]['VolumeGroups'][0]['APFSVolumeGroupUUID']==meta['volumeGroupUUID']
finally:subprocess.run(['hdiutil','detach',dev],check=True)
meta.update(GPTImage=str(dst),GPTImageSHA256=hashlib.sha256(dst.read_bytes()).hexdigest(),GPTImageBytes=dst.stat().st_size,partitionType=str(ptype),partitionUUID=str(partguid),diskUUID=str(diskguid),GPTAndRoleVerified=True);(D/'SYSTEM-BOOT-CONTEXT-PREPARATION.json').write_text(json.dumps(meta,indent=2)+'\n');print('Verified GPT/APFS System/Data/Preboot image:',meta['GPTImageSHA256'])
