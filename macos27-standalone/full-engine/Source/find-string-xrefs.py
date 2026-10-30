# SPDX-License-Identifier: MIT
from pathlib import Path
import struct,sys,bisect
b=Path('/Volumes/Dell27Build/InstallerInputs/kernelcache.vma2.macho').read_bytes();needle=sys.argv[1].encode()+b'\0';segments=[];a=32
for _ in range(struct.unpack_from('<I',b,16)[0]):
 c,s=struct.unpack_from('<II',b,a)
 if c==25:
  v,z,f,n=struct.unpack_from('<QQQQ',b,a+24);segments.append((v,f,n,b[a+8:a+24].rstrip(b'\0')))
 a+=s
locations=[];start=0
while (o:=b.find(needle,start))>=0:
 start=o+1
 for v,f,n,name in segments:
  if f<=o<f+n:locations.append(v+o-f);print('string',hex(v+o-f))
for v,f,n,name in segments:
 if name!=b'__TEXT_EXEC':continue
 for off in range(f,f+n-32,4):
  w=struct.unpack_from('<I',b,off)[0]
  if w&0x9f000000!=0x90000000:continue
  imm=((w>>5&0x7ffff)<<2)|(w>>29&3);imm=imm-(1<<21) if imm&(1<<20) else imm;pc=v+off-f;page=((pc&~4095)+(imm<<12))&((1<<64)-1)
  for target in locations:
   if page!=target&~4095:continue
   for j in range(1,5):
    t=struct.unpack_from('<I',b,off+j*4)[0]
    if t&0xffc00000==0x91000000 and t>>5&31==w&31 and t>>10&4095==target&4095:print('xref',hex(pc))
