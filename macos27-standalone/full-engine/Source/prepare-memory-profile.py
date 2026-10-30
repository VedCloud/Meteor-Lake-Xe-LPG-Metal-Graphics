# SPDX-License-Identifier: MIT
"""Create a local 16 GiB reference DT variant; never modify the pinned input."""
from pathlib import Path
import struct,json,hashlib
r=Path(__file__).resolve().parent;src=r.parent/'darwin-vm/firmware/dtree';data=bytearray(src.read_bytes());changes=[]
def node(at,path):
 count,children=struct.unpack_from('<II',data,at);at+=8;props={}
 for _ in range(count):
  name=bytes(data[at:at+32]).split(b'\0')[0].decode();size=struct.unpack_from('<I',data,at+32)[0]&0x7fffffff;at+=36;props[name]=(at,size);at+=(size+3)&~3
 off,size=props['name'];name=bytes(data[off:off+size]).split(b'\0')[0].decode();path+='/'+name
 if path=='/device-tree/chosen':
  off,size=props['dram-size'];assert size==8;before=struct.unpack_from('<Q',data,off)[0];struct.pack_into('<Q',data,off,16*1024**3);changes.append({'property':path+'/dram-size','before':before,'after':16*1024**3})
 for _ in range(children):at=node(at,path)
 return at
assert node(0,'')==len(data);assert len(changes)==1
out=r/'reference-16GiB.dtree';out.write_bytes(data)
(r/'MEMORY-PROFILE.json').write_text(json.dumps({'input':str(src),'inputSHA256':hashlib.sha256(src.read_bytes()).hexdigest(),'outputSHA256':hashlib.sha256(data).hexdigest(),'changes':changes},indent=2)+'\n')
print(changes)
