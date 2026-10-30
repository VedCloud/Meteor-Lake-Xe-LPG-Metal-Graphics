# SPDX-License-Identifier: MIT
"""Separate HFS+ recovery ramdisk, using the verified supplied recovery files."""
from pathlib import Path
import hashlib,json,mmap,struct,subprocess,re
r=Path(__file__).resolve().parent;s=r/'StorageExperiment';d=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment');meta=json.loads((d/'HFS-PREPARATION.json').read_text())
assert meta['payloadVerified'] and not meta.get('error'),meta
media=d/'BaseSystem-26A428-HFS.dmg'
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(media)==meta['imageSHA256'];f=media.open('rb');f.seek(1024);vh=f.read(512);f.close();assert vh[:2]==b'H+'
block,total=struct.unpack_from('>II',vh,40);assert block*total==media.stat().st_size
m=json.loads((s/'VMAPPLE-REFERENCE-BOOT.json').read_text());oldsha=m['sha256'];root=m['recoveryImagePhysical'];newtop=(root+media.stat().st_size+16383)&~16383;length=newtop-m['guestBase'];assert length<m['RAMBytes']
p=d/'vmapple-reference-boot-state.bin';assert not p.exists();subprocess.run(['cp',str(s/p.name),str(p)],check=True)
def decode(b,a=0):
 np,nc=struct.unpack_from('<II',b,a);a+=8;p={}
 for _ in range(np):
  k=bytes(b[a:a+32]).split(b'\0')[0].decode();n=struct.unpack_from('<I',b,a+32)[0]&0x7fffffff;a+=36;p[k]=bytes(b[a:a+n]);a+=(n+3)&~3
 cs=[]
 for _ in range(nc):c,a=decode(b,a);cs.append(c)
 return {'p':p,'c':cs},a
def encode(t):
 b=bytearray(struct.pack('<II',len(t['p']),len(t['c'])))
 for k,v in t['p'].items():b+=k.encode().ljust(32,b'\0')+struct.pack('<I',len(v))+v+bytes((-len(v))%4)
 for c in t['c']:b+=encode(c)
 return b
def child(t,n):return next(c for c in t['c'] if c['p']['name'].rstrip(b'\0').decode()==n)
assert sha(p)==oldsha
with p.open('r+b') as f:
 f.truncate(length);im=mmap.mmap(f.fileno(),0);off=root-m['guestBase']
 with media.open('rb') as image:
  at=0
  for b in iter(lambda:image.read(1024*1024),b''):im[off+at:off+at+len(b)]=b;at+=len(b)
 dt=m['deviceTreePhysical']-m['guestBase'];tree,n=decode(im[dt:dt+m['deviceTreeBytes']]);assert n==m['deviceTreeBytes'];chosen=child(tree,'chosen');mm=child(chosen,'memory-map');mm['p'].pop('Dell27RecoveryBacking');mm['p']['RAMDisk']=struct.pack('<QQ',root,media.stat().st_size)
 for key in ['boot-uuid','apfs-preboot-uuid','root-matching','root-snapshot-name']:chosen['p'].pop(key,None)
 args=m['X0']-m['guestBase'];assert struct.unpack_from('<Q',im,args+32)[0]==m['initializedTop'];struct.pack_into('<Q',im,args+32,newtop)
 cmd=bytes(im[args+108:args+1132]).split(b'\0')[0];assert b'rd=' not in cmd;cmd=b'rd=md0 '+cmd;assert len(cmd)<1024;im[args+108:args+1132]=cmd+bytes(1024-len(cmd));chosen['p']['boot-args']=cmd+b'\0'
 encoded=encode(tree);mm['p']['DeviceTree']=struct.pack('<QQ',m['deviceTreePhysical'],len(encoded));encoded=encode(tree);assert len(encoded)<=m['deviceTreeCapacityBytes'];im[dt:dt+m['deviceTreeCapacityBytes']]=encoded+bytes(m['deviceTreeCapacityBytes']-len(encoded));struct.pack_into('<I',im,args+104,len(encoded));im.flush()
 m.update(sha256=hashlib.sha256(im).hexdigest(),initializedTop=newtop,guestBytes=length,deviceTreeBytes=len(encoded),recoveryImageBytes=media.stat().st_size,recoveryImageSHA256=meta['imageSHA256'],deviceProfile='Verified recovery files repackaged as HFS+ md0 bootstrap; no installer or graphics acceleration claim')
 m['inputRelativePath']='EngineProbe/vmapple-'+m['sha256']+'.bin';(d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(m,indent=2)+'\n')
 header=(s/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]
 for key,value in [('VMREF_BYTES',length),('VMROOT_BYTES',media.stat().st_size)]:header=re.sub(r'#define '+key+r' .*', '#define '+key+' '+hex(value)+'ULL',header)
 header+='static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(m['sha256']))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+m['inputRelativePath'].replace('/','\\'))+'\n';(d/'vmapple_reference_boot.h').write_text(header);im.close()
print(json.dumps(m,indent=2))
