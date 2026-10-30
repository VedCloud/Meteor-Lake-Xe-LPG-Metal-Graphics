# SPDX-License-Identifier: MIT
"""Inspect/set BSD owner IDs in an unmounted, user-owned HFS+ raw guest image.
No host device nodes, mount points, symlinks, flags or file data are modified.
"""
from pathlib import Path
import os,struct,subprocess,plistlib
class OwnedHFS:
 def __init__(self,image):
  self.image=Path(image)
  assert self.image.is_file() and not self.image.is_symlink()
  assert self.image.stat().st_uid==os.getuid()
  assert str(self.image.resolve()).startswith('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment/')
  inventory=plistlib.loads(subprocess.check_output(['hdiutil','info','-plist']))
  assert all(Path(x['image-path']).resolve()!=self.image.resolve() for x in inventory['images']), 'detach the guest image first'
  self.f=self.image.open('r+b');self.f.seek(1024);h=self.f.read(512)
  assert h[:2]==b'H+' and struct.unpack_from('>H',h,2)[0]==4
  self.block=struct.unpack_from('>I',h,40)[0];assert self.block==4096
  self.length=struct.unpack_from('>Q',h,272)[0]
  self.extents=[struct.unpack_from('>II',h,288+8*i) for i in range(8)];assert sum(n*self.block for _,n in self.extents)>=self.length, 'catalog overflow extents require a fuller editor'
  data=bytearray()
  for start,count in self.extents:
   if count:self.f.seek(start*self.block);data+=self.f.read(count*self.block)
  self.data=data[:self.length];self.node=struct.unpack_from('>H',self.data,32)[0];assert self.node in [4096,8192,16384,32768]
  self.records={}
  for base in range(0,len(self.data),self.node):
   if struct.unpack_from('>b',self.data,base+8)[0]!=-1:continue
   count=struct.unpack_from('>H',self.data,base+10)[0];assert count<self.node//8
   for i in range(count):
    at=base+struct.unpack_from('>H',self.data,base+self.node-2*(i+1))[0]
    keylen,parent,chars=struct.unpack_from('>HIH',self.data,at);assert keylen==6+2*chars and keylen<520
    name=self.data[at+8:at+8+chars*2].decode('utf-16-be');record=at+2+keylen;kind=struct.unpack_from('>H',self.data,record)[0]
    if kind in [1,2]:self.records[(parent,name)]=(record,kind)
 def physical(self,offset):
  for start,count in self.extents:
   n=count*self.block
   if offset<n:return start*self.block+offset
   offset-=n
  raise ValueError('catalog offset outside extents')
 def lookup(self,path):
  parent=2
  parts=Path(path).parts;assert not Path(path).is_absolute() and '..' not in parts
  for i,name in enumerate(parts):
   at,kind=self.records[(parent,name)]
   if i<len(parts)-1:assert kind==1;parent=struct.unpack_from('>I',self.data,at+8)[0]
  return at,kind
 def owner(self,path,new=None):
  at,kind=self.lookup(path);offset=at+32;old=struct.unpack_from('>II',self.data,offset)
  if new is not None:
   assert all(0<=x<2**32 for x in new);assert self.physical(offset+7)==self.physical(offset)+7
   self.f.seek(self.physical(offset));self.f.write(struct.pack('>II',*new));self.f.flush();os.fsync(self.f.fileno());struct.pack_into('>II',self.data,offset,*new)
  return {'path':path,'recordType':kind,'previousUIDGID':old,'newUIDGID':new,'physicalOffset':self.physical(offset)}
 def bsd_info(self,path,new=None):
  at,kind=self.lookup(path);offset=at+32;old=bytes(self.data[offset:offset+16])
  if new is not None:
   assert len(new)==16 and self.physical(offset+15)==self.physical(offset)+15
   self.f.seek(self.physical(offset));self.f.write(new);self.f.flush();os.fsync(self.f.fileno());self.data[offset:offset+16]=new
  return old
 def close(self):self.f.close()
