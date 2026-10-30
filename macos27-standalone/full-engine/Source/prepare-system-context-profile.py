# SPDX-License-Identifier: MIT
"""HFS RAM root plus genuine APFS Recovery-role boot-object media."""
from pathlib import Path
import json,hashlib,struct,mmap,subprocess,re
r=Path(__file__).resolve().parent;s=r/'HFSExperiment';d=s.resolve().parent/'BootContextExperiment-SystemGroup';d.mkdir(exist_ok=True)
link=r/'BootContextExperiment'
if link.is_symlink():link.unlink()
assert not link.exists();link.symlink_to(d,target_is_directory=True)
meta=json.loads((s/'SYSTEM-BOOT-CONTEXT-PREPARATION.json').read_text());media=Path(meta['GPTImage']);assert meta['GPTAndRoleVerified'] and meta['bootObjectsPopulated']
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(media)==meta['GPTImageSHA256'];m=json.loads((s/'VMAPPLE-REFERENCE-BOOT.json').read_text());source_sha=m['sha256'];base=m['initializedTop'];top=(base+media.stat().st_size+16383)&~16383;length=top-m['guestBase'];assert length<m['RAMBytes'];p=d/'vmapple-reference-boot-state.bin';assert not p.exists();subprocess.run(['cp','-c',str(s/p.name),str(p)],check=True);assert sha(p)==source_sha

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
with p.open('r+b') as f:
 f.truncate(length);im=mmap.mmap(f.fileno(),0);offset=base-m['guestBase']
 with media.open('rb') as image:
  at=0
  for b in iter(lambda:image.read(1024*1024),b''):im[offset+at:offset+at+len(b)]=b;at+=len(b)
 dt=m['deviceTreePhysical']-m['guestBase'];t,n=decode(im[dt:dt+m['deviceTreeBytes']]);assert n==m['deviceTreeBytes'];chosen=child(t,'chosen');mm=child(chosen,'memory-map');mm['p']['RecoveryBootContext']=struct.pack('<QQ',base,media.stat().st_size)
 chosen['p']['boot-uuid']=meta['volumeGroupUUID'].encode()+b'\0';chosen['p']['apfs-preboot-uuid']=meta['volumeGroupUUID'].encode()+b'\0';chosen['p']['boot-command']=b'\x01';chosen['p']['boot-objects-path']=meta['bootObjectsPath'].encode()+b'\0'
 args=m['X0']-m['guestBase'];assert struct.unpack_from('<Q',im,args+32)[0]==m['initializedTop'];struct.pack_into('<Q',im,args+32,top)
 cmd=bytes(im[args+108:args+1132]).split(b'\0')[0];assert b'rd=md0' in cmd;cmd+=b' -no-exclaveos-graft';assert len(cmd)<1024;im[args+108:args+1132]=cmd+bytes(1024-len(cmd));chosen['p']['boot-args']=cmd+b'\0'
 encoded=encode(t);mm['p']['DeviceTree']=struct.pack('<QQ',m['deviceTreePhysical'],len(encoded));encoded=encode(t);assert len(encoded)<=m['deviceTreeCapacityBytes'];im[dt:dt+m['deviceTreeCapacityBytes']]=encoded+bytes(m['deviceTreeCapacityBytes']-len(encoded));struct.pack_into('<I',im,args+104,len(encoded));im.flush();digest=hashlib.sha256(im).hexdigest();im.close()
m.update(sha256=digest,inputRelativePath='EngineProbe/vmapple-'+digest+'.bin',initializedTop=top,guestBytes=length,deviceTreeBytes=len(encoded),virtioImagePhysical=base,virtioImageBytes=media.stat().st_size,virtioImageSHA256=meta['GPTImageSHA256'],bootContextUUID=meta['volumeGroupUUID'],bootObjectsPath=meta['bootObjectsPath'],deviceProfile='Unsealed HFS recovery RAM root plus supplied boot objects in an actual GPT/APFS System/Data group and Preboot volume; ExclaveOS not emulated')
(d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(m,indent=2)+'\n');h=(s/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]
for k,v in [('VMREF_BYTES',length),('VMROOT_BASE',base),('VMROOT_BYTES',media.stat().st_size)]:h=re.sub(r'#define '+k+r' .*','#define '+k+' '+hex(v)+'ULL',h)
h+='static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(digest))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+m['inputRelativePath'].replace('/','\\'))+'\n';(d/'vmapple_reference_boot.h').write_text(h);(d/'BOOT-CONTEXT-PREPARATION.json').write_text(json.dumps(meta,indent=2)+'\n');print(json.dumps({'captureSHA256':digest,'initializedBytes':length,'logicalRAMBytes':m['RAMBytes'],'bootContextUUID':m['bootContextUUID']},indent=2))
