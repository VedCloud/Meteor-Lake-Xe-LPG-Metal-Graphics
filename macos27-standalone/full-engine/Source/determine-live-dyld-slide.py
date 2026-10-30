# SPDX-License-Identifier: MIT
"""Verify this owned boot's ASLR slide from two exact 64-byte code matches.
Reads only selected live instruction bytes via the existing read-only inspector,
then scans the supplied read-only cache. No guest or source code is changed.
"""
from pathlib import Path
import hashlib,json,mmap,struct,subprocess,time,datetime,os
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run'])
cache=Path(os.environ.get('DELL27_DYLD_CACHE_DIR','/Volumes/macOS Base System/System/Library/dyld'));verified=[]
assert (cache/'dyld_shared_cache_arm64e').is_file(), 'Mount the exact recovery image read-only and set DELL27_DYLD_CACHE_DIR to its System/Library/dyld directory'
for attempt in range(12):
 p=subprocess.run(['/Volumes/Dell27Build/deps/bin/python3',str(r/'inspect-live-processes.py')],capture_output=True,text=True,check=True);sample=json.loads(p.stdout)
 pc=int(sample['PC'],16);raw=bytes.fromhex(sample.get('instructionBytes') or '')
 if pc>=0x8000000000000000 and sample.get('savedUserState'):
  pc=int(sample['savedUserState']['PC'],16);raw=bytes.fromhex(sample['savedUserState'].get('instructionBytes') or '')
 if pc>=0x8000000000000000 or len(raw)!=64 or any(x['livePC']==pc for x in verified):time.sleep(.5);continue
 matches=[]
 for path in sorted(cache.glob('dyld_shared_cache_arm64e*')):
  with path.open('rb') as f:
   header=f.read(32)
   if not header.startswith(b'dyld_v1'):continue
   off,count=struct.unpack_from('<II',header,16)
   if count>128:continue
   f.seek(off);mapping=[struct.unpack('<QQQII',f.read(32)) for _ in range(count)]
   with mmap.mmap(f.fileno(),0,access=mmap.ACCESS_READ) as mm:
    pos=mm.find(raw)
    while pos>=0:
     for addr,size,fileoff,maxprot,prot in mapping:
      if prot&4 and fileoff<=pos and pos+len(raw)<=fileoff+size:
       source=addr+pos-fileoff;slide=pc-source
       if 0<=slide<=0x100000000 and not slide%0x4000:matches.append({'livePC':pc,'sourceVA':source,'slide':slide,'cacheFile':str(path),'fileOffset':pos,'codeSHA256':hashlib.sha256(raw).hexdigest()})
     pos=mm.find(raw,pos+1)
 if len(matches)==1:
  verified.append(matches[0]);print('Exact live/source code match:',json.dumps(matches[0]),flush=True)
  if len(verified)>=2:
   assert len({x['slide'] for x in verified})==1,'conflicting live cache slides'
   result={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(d),'EFI_SHA256':a['EFI_SHA256'],'slide':verified[0]['slide'],'basis':'Two distinct live user PCs exactly match 64 bytes in executable mappings of supplied read-only dyld cache; correctly 16K-aligned slide. Scoped to this boot only.','matches':verified}
   (d/'DYLD-SLIDE.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2));break
 time.sleep(.5)
else:raise SystemExit('No two independently matching live user-code samples; no slide recorded')
