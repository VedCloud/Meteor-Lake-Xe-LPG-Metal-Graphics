from pathlib import Path
import struct
b=Path('/Volumes/Dell27Build/InstallerInputs/dtree.vma2').read_bytes()
def node(at,path=''):
 np,nc=struct.unpack_from('<II',b,at);at+=8;p={}
 for _ in range(np):
  k=b[at:at+32].split(b'\0')[0].decode();n=struct.unpack_from('<I',b,at+32)[0]&0x7fffffff;at+=36;p[k]=b[at:at+n];at+=(n+3)&~3
 path+='/'+p.get('name',b'?').split(b'\0')[0].decode()
 if 'avp-' in path or 'nvram' in path or 'uart' in path:
  print(path)
  for k,v in p.items():print(k,repr(v) if len(v)<80 else str(len(v)))
 for _ in range(nc):at=node(at,path)
 return at
node(0)
