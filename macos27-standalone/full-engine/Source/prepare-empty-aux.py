# SPDX-License-Identifier: MIT
"""Create a NEW private VM utility image with an empty EAN filesystem.
No host devices, existing storage, keybags, or active run files are modified.
Format comes from supplied 26A428 AppleNVMeEAN::eanFormat/writeHeader.
"""
from pathlib import Path
import struct,zlib,json,hashlib,sys
SIZE=32*1024*1024
EAN_OFFSET=26*1024*1024
EAN_BYTES=6*1024*1024
MAGIC=bytes.fromhex('01000000eaeaeaea')
def empty_header():
 h=bytearray(4096);h[:8]=MAGIC
 struct.pack_into('<III',h,8,0,EAN_BYTES,4096)
 struct.pack_into('<I',h,4092,zlib.crc32(h[:4092]))
 return bytes(h)
def validate_header(h):
 assert len(h)==4096 and h[:8]==MAGIC
 assert struct.unpack_from('<III',h,8)==(0,EAN_BYTES,4096)
 assert struct.unpack_from('<I',h,4092)[0]==zlib.crc32(h[:4092])
 assert h[20:4092]==bytes(4072)
def create(path):
 path=Path(path);assert path.is_absolute() and path.parent.is_dir()
 # Exclusive creation is mandatory: never format or overwrite a used image.
 h=empty_header();validate_header(h)
 with path.open('xb') as f:
  f.truncate(SIZE);f.seek(EAN_OFFSET);assert f.write(h)==4096
 with path.open('rb') as f:
  f.seek(EAN_OFFSET);validate_header(f.read(4096))
 return {'path':str(path),'bytes':SIZE,'EANOffset':EAN_OFFSET,'EANBytes':EAN_BYTES,'headerSHA256':hashlib.sha256(h).hexdigest(),'state':'Empty filesystem only; no keys, tokens or authenticated state fabricated','nativeAcceptanceVerified':False}
if __name__=='__main__':
 assert len(sys.argv)==2
 print(json.dumps(create(sys.argv[1]),indent=2))
