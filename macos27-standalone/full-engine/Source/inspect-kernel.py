from pathlib import Path
import struct,sys
from capstone import Cs,CS_ARCH_ARM64,CS_MODE_ARM
b=Path('/Volumes/Dell27Build/InstallerInputs/kernelcache.vma2.macho').read_bytes()
def cmds(off):
 n=struct.unpack_from('<I',b,off+16)[0];at=off+32
 for _ in range(n):
  c,s=struct.unpack_from('<II',b,at);yield c,at;at+=s
segments=[]
for c,at in cmds(0):
 if c==0x19:
  name=b[at+8:at+24].split(b'\0')[0].decode();va,sz,fo,fs=struct.unpack_from('<QQQQ',b,at+24);segments.append((va,sz,fo,fs,name))
 if c==0x80000035:
  va,fo,st=struct.unpack_from('<QQI',b,at+8);name=b[at+st:at+st+256].split(b'\0')[0].decode()
  if name=='com.apple.kernel':
   for c2,a2 in cmds(fo):
    if c2==0x19:
     nm=b[a2+8:a2+24].split(b'\0')[0].decode();v,s,f,fs=struct.unpack_from('<QQQQ',b,a2+24);segments.append((v,s,f,fs,nm))
a=int(sys.argv[1],0);size=int(sys.argv[2],0) if len(sys.argv)>2 else 512
for v,s,f,fs,n in segments:
 if v<=a<v+fs:
  data=b[f+a-v:f+a-v+size];print(n,hex(f+a-v));break
else:raise ValueError(hex(a))
for i in Cs(CS_ARCH_ARM64,CS_MODE_ARM).disasm(data,a): print(hex(i.address),i.mnemonic,i.op_str)
