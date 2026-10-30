# SPDX-License-Identifier: MIT
"""Replace the reference M2 DT with the supplied VMAPPLE template and real handoff.

Only CPU0 is declared: the monitor currently runs one vCPU. Runtime memory-map,
boot identity and initialized-image locations come from the captured handoff.
This creates a platform contract to implement; it does not emulate its devices.
"""
from pathlib import Path
import struct,json,hashlib,argparse,mmap
parser=argparse.ArgumentParser();parser.add_argument("--installer",action="store_true");options=parser.parse_args()
r=Path(__file__).resolve().parent;w=r.parent
if options.installer:r=r/'InstallerExperiment'
m=json.loads((r/'VMAPPLE-REFERENCE-BOOT.json').read_text());capture_file=(r/'vmapple-reference-boot-state.bin').open('r+b');image=mmap.mmap(capture_file.fileno(),0)
def decode(data,at=0):
 if at+8>len(data):raise ValueError('truncated DT')
 np,nc=struct.unpack_from('<II',data,at);at+=8;props={}
 assert np<4096 and nc<4096
 for _ in range(np):
  assert at+36<=len(data);key=bytes(data[at:at+32]).split(b'\0')[0].decode();raw=struct.unpack_from('<I',data,at+32)[0];size=raw&0x7fffffff;at+=36;assert at+size<=len(data);props[key]=(raw&0x80000000,bytes(data[at:at+size]));at+=(size+3)&~3
 children=[]
 for _ in range(nc):child,at=decode(data,at);children.append(child)
 return {'props':props,'children':children},at
def name(n):return n['props']['name'][1].split(b'\0')[0].decode()
def child(n,key):return next(c for c in n['children'] if name(c)==key)
def put(n,key,value):n['props'][key]=(n['props'].get(key,(0,b''))[0],value)
def encode(n):
 out=bytearray(struct.pack('<II',len(n['props']),len(n['children'])))
 for key,(flag,value) in n['props'].items():
  raw=key.encode();assert len(raw)<32;out+=raw.ljust(32,b'\0')+struct.pack('<I',len(value))+value+b'\0'*((-len(value))%4)
 for c in n['children']:out+=encode(c)
 return out
original=m['deviceTreePhysical']-m['guestBase'];old,_=decode(image[original:original+m['deviceTreeBytes']]);template=bytearray((w/'InstallerInputs/dtree.vma2').read_bytes());tree,used=decode(template);assert used==len(template)
# PV CTRR is optional in XNU (platform_tests.c explicitly supports an older
# host without it). Do not advertise a protection controller we do not enforce.
# Removing the DT node selects that documented compatibility path, rather than
# inventing a register-success response or patching kernel instructions.
armio=child(tree,'arm-io')
armio['children']=[c for c in armio['children'] if name(c)!='avp-ctrr']
chosen=child(tree,'chosen');oldchosen=child(old,'chosen')
# iBoot fills these zero-valued template fields at runtime. Preserve the
# reference guest's real NVRAM proxy and matching dimensions, not host NVRAM.
reference,_=decode((r/'reference-16GiB.dtree').read_bytes())
reference_chosen=child(reference,'chosen')
for key in ['nvram-proxy-data','nvram-bank-size','nvram-total-size','nvram-bank-count','nvram-current-bank']:
 chosen['props'][key]=reference_chosen['props'][key]
proxy=chosen['props']['nvram-proxy-data'][1]
assert len(proxy)>0 and struct.unpack('<I',chosen['props']['nvram-bank-size'][1])[0]==len(proxy)
assert len(proxy)>=32 and proxy[4:9]==b'nvram'
checksum=proxy[0]+sum(proxy[2:16])
while checksum>255:checksum=(checksum&255)+(checksum>>8)
assert checksum==proxy[1], 'invalid CHRP NVRAM header checksum'
assert int.from_bytes(proxy[2:4],'big')*16==len(proxy), 'NVRAM extent does not match proxy'

# Keep the allocation capacity distinct from the shortened flattened DT size.
# Recover the original capacity only from matching captured boot arguments.
a=m['X0']-m['guestBase'];original_args=(r/'vmapple-bootargs.bin').read_bytes()
assert image[a:a+104]==original_args[:104]
capacity=struct.unpack_from('<I',original_args,104)[0]
assert m['deviceTreeBytes']<=capacity<1024*1024 and original+capacity<=len(image)

# Use initialized runtime data; preserve VMAPPLE's own compatible/model nodes.
for key in ['dram-base','dram-size','random-seed','boot-args','boot-uuid','firmware-version','root-matching','debug-enabled']:
 if key in oldchosen['props']:chosen['props'][key]=oldchosen['props'][key]
chosen['children']=[c for c in chosen['children'] if name(c)!='memory-map']+[child(oldchosen,'memory-map')]

if options.installer:
 inputs=w/'InstallerInputs/BaseSystem'
 proof=json.loads((inputs/'BASESYSTEM-INPUT-VALIDATION.json').read_text())
 auth=(inputs/'basesystem-26A428.root_hash').read_bytes()
 assert hashlib.sha256(auth).hexdigest()==proof['BaseSystemVolume']['payloadSHA256']
 put(child(chosen,'secure-boot-hashes'),'system-volume-auth-blob',auth)
 put(chosen,'boot-uuid',proof['APFSVolumeUUID'].encode()+b'\0')
 put(chosen,'apfs-preboot-uuid',proof['APFSVolumeUUID'].encode()+b'\0')
 env=child(child(tree,'osenvironments'),'darwinos-ramdisk-environment')
 for recipe,key in [('no-protected-data-access','protected-data-access'),('no-sepfw-load-at-boot','sepfw-load-at-boot')]:
  action=child(env,recipe)['props']
  assert action['node_path'][1]==b'chosen\0' and action['property_name'][1]==key.encode()+b'\0' and action['value'][1]==bytes(4)
  put(chosen,key,bytes(4))

cpus=child(tree,'cpus');cpus['children']=[child(cpus,'cpu0')];cpu=cpus['children'][0];put(cpu,'state',b'running\0');frequency=struct.unpack('<Q',cpu['props']['clock-frequency'][1])[0]
assert frequency>0
put(child(tree,'memory'),'reg',struct.pack('<QQ',m['RAMBase'],m['RAMBytes']))
# The allocated DT slot is larger than the VM template. Update every size
# reference before hashing, and never move any initialized RAM image.
mmap=child(chosen,'memory-map');encoded=encode(tree)
if 'DeviceTree' in mmap['props']:put(mmap,'DeviceTree',struct.pack('<QQ',m['deviceTreePhysical'],len(encoded)));encoded=encode(tree)
assert len(encoded)<=capacity
image[original:original+capacity]=encoded+b'\0'*(capacity-len(encoded))
a=m['X0']-m['guestBase'];assert struct.unpack_from('<Q',image,a+24)[0]==16*1024**3;struct.pack_into('<I',image,a+104,len(encoded))
image.flush();m.update({'deviceTreeBytes':len(encoded),'deviceTreeCapacityBytes':capacity,'NVRAMProxyBytes':len(proxy),'counterFrequencyHz':frequency,'deviceProfile':'Supplied vma2 template with captured runtime locations, one vCPU and 16 GiB RAM','sha256':hashlib.sha256(image).hexdigest(),'scope':'Full initialized reference memory with VMAPPLE template handoff; virtual devices require runtime implementation'})
m['inputRelativePath']='EngineProbe/vmapple-'+m['sha256']+'.bin'
(r/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(m,indent=2)+'\n')
(r/'vmapple_reference_boot.h').write_text('/* Generated from validated captured handoff and VMAPPLE template. */\n'+''.join('#define '+k+' '+hex(v)+'ULL\n' for k,v in [('VMREF_RAM_BASE',m['RAMBase']),('VMREF_RAM_BYTES',m['RAMBytes']),('VMREF_CNTFRQ',frequency),('VMREF_BASE',m['guestBase']),('VMREF_BYTES',len(image)),('VMREF_PC',m['PC']),('VMREF_X0',m['X0'])])+'static const uint8_t vmref_sha256[32]={'+','.join(str(v) for v in bytes.fromhex(m['sha256']))+'};\n')
with (r/'vmapple_reference_boot.h').open('a') as header:header.write('#define VMREF_INPUT_PATH L'+json.dumps('\\'+m['inputRelativePath'].replace('/','\\'))+'\n')
(r/'VMAPPLE-HANDOFF.json').write_text(json.dumps({'deviceTreeSHA256':hashlib.sha256(encoded).hexdigest(),'deviceTreeBytes':len(encoded),'CPUs':1,'RAMBytes':m['RAMBytes'],'counterHz':frequency,'devicesImplemented':False,'unadvertisedOptionalDevices':['avp-ctrr'],'PVCTRRProtection':False,'NVRAMProxyBytes':len(proxy),'NVRAMProxySHA256':hashlib.sha256(proxy).hexdigest(),'deviceTreeCapacityBytes':capacity},indent=2)+'\n')
image.close();capture_file.close()
print('Prepared VMAPPLE handoff:',len(encoded),'DT bytes;',frequency,'counter Hz; 16 GiB RAM; one vCPU')
