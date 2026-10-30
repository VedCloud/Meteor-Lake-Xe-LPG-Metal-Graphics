# SPDX-License-Identifier: MIT
"""Validate the actual initialized bytes consumed by the VMAPPLE kernel."""
from pathlib import Path
import struct,json,hashlib,sys,mmap
r=Path(__file__).resolve().parent;w=r.parent
installer='--installer' in sys.argv
if installer:r=r/'InstallerExperiment'
m=json.loads((r/'VMAPPLE-REFERENCE-BOOT.json').read_text());stream=(r/'vmapple-reference-boot-state.bin').open('rb');b=mmap.mmap(stream.fileno(),0,access=mmap.ACCESS_READ)
def file_sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for chunk in iter(lambda:f.read(8*1024*1024),b''):h.update(chunk)
 return h.hexdigest()
assert hashlib.sha256(b).hexdigest()==m['sha256'] and len(b)==m['guestBytes']
def physical(pa,length):
 assert pa>=m['guestBase'] and length>=0 and pa-m['guestBase']<=len(b)-length,(hex(pa),length)
 return memoryview(b)[pa-m['guestBase']:pa-m['guestBase']+length]
def decode(at):
 np,nc=struct.unpack_from('<II',dt,at);at+=8;p={}
 for _ in range(np):
  key=dt[at:at+32].split(b'\0')[0].decode();length=struct.unpack_from('<I',dt,at+32)[0];assert not length&0x80000000;at+=36;p[key]=dt[at:at+length];assert len(p[key])==length;at+=(length+3)&~3
 children=[]
 for _ in range(nc):child,at=decode(at);children.append(child)
 return (p,children),at
def child(n,name):return next(c for c in n[1] if c[0]['name'].split(b'\0')[0].decode()==name)
a=physical(m['X0'],4096);assert struct.unpack_from('<Q',a,24)[0]==16*1024**3
assert struct.unpack_from('<I',a,104)[0]==m['deviceTreeBytes']
dt=bytes(physical(m['deviceTreePhysical'],m['deviceTreeBytes']));tree,end=decode(0);assert end==len(dt)
chosen=child(tree,'chosen');mm=child(chosen,'memory-map')[0]
assert len(child(tree,'cpus')[1])==1
proxy=chosen[0]['nvram-proxy-data'];assert len(proxy)==struct.unpack('<I',chosen[0]['nvram-bank-size'])[0]==8192
pa,length=struct.unpack('<QQ',mm['TrustCache']);tc=physical(pa,length);num=struct.unpack_from('<I',tc)[0];assert num>0 and 4+num*4<=len(tc)
offsets=struct.unpack_from('<'+'I'*num,tc,4);assert all(4+num*4<=o<len(tc) for o in offsets)
assert num==1 and tc[offsets[0]:]==(w/('InstallerInputs/BaseSystem/basesystem-26A428.trustcache' if installer else 'darwin-vm/firmware/ramdisk.tc')).read_bytes(), 'reference trust cache payload mismatch'
rdkey=next(k for k in mm if k.lower()=='ramdisk');rdpa,rdlength=struct.unpack('<QQ',mm[rdkey]);rd=physical(rdpa,rdlength);assert hashlib.sha256(rd).hexdigest()==file_sha(w/('InstallerInputs/BaseSystem/BaseSystem-26A428.apfs' if installer else 'darwin-vm/firmware/ramdisk.dmg'))
report={'inputSHA256':m['sha256'],'logicalRAMBytes':m['RAMBytes'],'capturedPhysicalBase':hex(m['guestBase']),'kernelPhysicalBase':hex(m['kernelPhysicalBase']),'initializedBytes':len(b),'trustCachePhysical':hex(pa),'trustCacheContainerBytes':len(tc),'trustCacheModules':num,'trustCachePayloadSHA256':hashlib.sha256(tc[offsets[0]:]).hexdigest(),'ramdiskPhysical':hex(rdpa),'ramdiskBytes':len(rd),'ramdiskSHA256':hashlib.sha256(rd).hexdigest(),'installerProfile':installer,'CPUs':1,'NVRAMProxyBytes':len(proxy),'result':'PASS: complete real trust-cache and ramdisk bytes, DT and 16 GiB handoff'}
(r/'HANDOFF-VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
