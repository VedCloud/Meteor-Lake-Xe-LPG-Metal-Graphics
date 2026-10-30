# SPDX-License-Identifier: MIT
"""Admit the independently created recovery snapshot without faking boot state."""
from pathlib import Path
import json,hashlib,struct,subprocess,mmap
r=Path(__file__).resolve().parent;s=r/'StorageExperiment';d=r/'SnapshotExperiment';prep=r/'RecoverySnapshotExperiment'
proof=json.loads((prep/'SNAPSHOT-PREP-RESULT.json').read_text());assert proof['snapshotCreated'] and not proof.get('cleanupError') and not proof.get('error'),proof
assert proof['volumeInfo']['Sealed']=='Yes','snapshot experiment lost its seal'
name=proof['snapshotName'];assert any(x['SnapshotName']==name for x in proof['snapshotInfo']['Snapshots'])
apfs=prep/'BaseSystem-26A428-with-root-snapshot.apfs'
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(apfs)==proof['experimentSHA256']
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
m=json.loads((s/'VMAPPLE-REFERENCE-BOOT.json').read_text());source=m['sha256'];d.mkdir(exist_ok=True);p=d/'vmapple-reference-boot-state.bin';assert not p.exists()
subprocess.run(['cp','-c',str(s/p.name),str(p)],check=True)
with p.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);assert hashlib.sha256(im).hexdigest()==source
 media=m['recoveryImagePhysical']-m['guestBase'];assert apfs.stat().st_size==m['recoveryImageBytes']
 with apfs.open('rb') as data:
  at=0
  for chunk in iter(lambda:data.read(1024*1024),b''):im[media+at:media+at+len(chunk)]=chunk;at+=len(chunk)
 off=m['deviceTreePhysical']-m['guestBase'];tree,n=decode(im[off:off+m['deviceTreeBytes']]);assert n==m['deviceTreeBytes'];chosen=child(tree,'chosen');assert 'root-snapshot-name' not in chosen['p']
 chosen['p']['root-snapshot-name']=name.encode()+b'\0';enc=encode(tree);child(chosen,'memory-map')['p']['DeviceTree']=struct.pack('<QQ',m['deviceTreePhysical'],len(enc));enc=encode(tree)
 assert len(enc)<=m['deviceTreeCapacityBytes'];im[off:off+m['deviceTreeCapacityBytes']]=enc+bytes(m['deviceTreeCapacityBytes']-len(enc));struct.pack_into('<I',im,m['X0']-m['guestBase']+104,len(enc));im.flush()
 m.update(sha256=hashlib.sha256(im).hexdigest(),deviceTreeBytes=len(enc),recoveryImageSHA256=proof['experimentSHA256'],rootSnapshotName=name,deviceProfile='Sealed recovery APFS snapshot over read-only native VirtIO; basic GOP; installer still unqualified')
 m['inputRelativePath']='EngineProbe/vmapple-'+m['sha256']+'.bin';(d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(m,indent=2)+'\n')
 header=(s/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]+'static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(m['sha256']))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+m['inputRelativePath'].replace('/','\\'))+'\n';(d/'vmapple_reference_boot.h').write_text(header)
 (d/'PROFILE.json').write_text(json.dumps({'sourceCaptureSHA256':source,'captureSHA256':m['sha256'],'snapshotPreparation':proof,'authenticationPatchAdded':False,'originalInstallerImageModified':False,'installerReached':False,'stockKernelCollection':False},indent=2)+'\n');im.close()
print(json.dumps(m,indent=2))
