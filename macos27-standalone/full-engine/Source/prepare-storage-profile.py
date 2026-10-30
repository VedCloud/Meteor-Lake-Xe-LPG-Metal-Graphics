# SPDX-License-Identifier: MIT
"""Native IOKit block path for the unchanged, sealed recovery APFS image."""
from pathlib import Path
import struct,json,mmap,subprocess,hashlib
r=Path(__file__).resolve().parent;s=r/'FramebufferExperiment';d=r/'StorageExperiment';d.mkdir(exist_ok=True)
m=json.loads((s/'VMAPPLE-REFERENCE-BOOT.json').read_text());p=d/'vmapple-reference-boot-state.bin';assert not p.exists()
subprocess.run(['cp','-c',str(s/p.name),str(p)],check=True)
def decode(b,a=0):
 np,nc=struct.unpack_from('<II',b,a);a+=8;p={}
 for _ in range(np):
  k=bytes(b[a:a+32]).split(b'\0')[0].decode();n=struct.unpack_from('<I',b,a+32)[0]&0x7fffffff;a+=36;p[k]=bytes(b[a:a+n]);a+=(n+3)&~3
 children=[]
 for _ in range(nc):c,a=decode(b,a);children.append(c)
 return {'p':p,'c':children},a
def encode(t):
 b=bytearray(struct.pack('<II',len(t['p']),len(t['c'])))
 for k,v in t['p'].items():b+=k.encode().ljust(32,b'\0')+struct.pack('<I',len(v))+v+bytes((-len(v))%4)
 for c in t['c']:b+=encode(c)
 return b
def child(t,n):return next(c for c in t['c'] if c['p']['name'].rstrip(b'\0').decode()==n)
with p.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);assert hashlib.sha256(im).hexdigest()==m['sha256'];source=m['sha256'];off=m['deviceTreePhysical']-m['guestBase'];tree,n=decode(im[off:off+m['deviceTreeBytes']]);assert n==m['deviceTreeBytes']
 chosen=child(tree,'chosen');mm=child(chosen,'memory-map');disk=mm['p'].pop('RAMDisk');image,size=struct.unpack('<QQ',disk)
 expected='cc280e2b8d70e2d7a5165e0581ca727fbe2d89db9002ca2b20481bb5f0d91e27';assert hashlib.sha256(im[image-m['guestBase']:image-m['guestBase']+size]).hexdigest()==expected
 # Preserve the allocation reservation while avoiding creation of md0.
 mm['p']['Dell27RecoveryBacking']=disk
 auth=Path('/Volumes/Dell27Build/InstallerInputs/BaseSystem/basesystem-26A428.root_hash').read_bytes();assert len(auth)==208
 sh=child(chosen,'secure-boot-hashes');assert sh['p']['system-volume-auth-blob']==auth;sh['p']['base-system-volume-auth-blob']=auth
 args=m['X0']-m['guestBase'];cmd=bytes(im[args+108:args+1132]).split(b'\0')[0];assert b'rd=md0' in cmd
 cmd=b' '.join(x for x in cmd.split() if x!=b'rd=md0');im[args+108:args+1132]=cmd+bytes(1024-len(cmd));chosen['p']['boot-args']=cmd+b'\0'
 enc=encode(tree);mm['p']['DeviceTree']=struct.pack('<QQ',m['deviceTreePhysical'],len(enc));enc=encode(tree);assert len(enc)<=m['deviceTreeCapacityBytes'];im[off:off+m['deviceTreeCapacityBytes']]=enc+bytes(m['deviceTreeCapacityBytes']-len(enc));struct.pack_into('<I',im,args+104,len(enc));im.flush()
 m.update(sha256=hashlib.sha256(im).hexdigest(),deviceTreeBytes=len(enc),deviceProfile='Read-only native VMApple VirtIO root disk with basic GOP; unqualified installer experiment',recoveryImagePhysical=image,recoveryImageBytes=size,recoveryImageSHA256=expected)
 m['inputRelativePath']='EngineProbe/vmapple-'+m['sha256']+'.bin'
 (d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(m,indent=2)+'\n')
 header=(s/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]+'#define VMROOT_BASE '+hex(image)+'ULL\n#define VMROOT_BYTES '+hex(size)+'ULL\nstatic const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(m['sha256']))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+m['inputRelativePath'].replace('/','\\'))+'\n'
 (d/'vmapple_reference_boot.h').write_text(header);(d/'PROFILE.json').write_text(json.dumps({'sourceCaptureSHA256':source,'captureSHA256':m['sha256'],'mediaSHA256':expected,'readOnlyMedia':True,'physicalDiskAccess':False,'rootPath':'IOKit VirtIO PCI -> APFS','installerReached':False,'stockKernelCollection':False},indent=2)+'\n');im.close()
print(json.dumps(m,indent=2))
