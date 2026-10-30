# SPDX-License-Identifier: MIT
"""Isolated recovery experiment: existing native boot framebuffer class.
Preserves the canonical accelerated-device profile and original extracted files.
"""
from pathlib import Path
import json,hashlib,struct,mmap,subprocess,xml.parsers.expat as expat
r=Path(__file__).resolve().parent;s=r/'InstallerExperiment';d=r/'FramebufferExperiment';d.mkdir(exist_ok=True)
meta=json.loads((s/'VMAPPLE-REFERENCE-BOOT.json').read_text());target=d/'vmapple-reference-boot-state.bin';assert not target.exists()
subprocess.run(['cp','-c',str(s/'vmapple-reference-boot-state.bin'),str(target)],check=True)
f=target.open('r+b');im=mmap.mmap(f.fileno(),0);assert hashlib.sha256(im).hexdigest()==meta['sha256']
def decode(b,at=0):
 np,nc=struct.unpack_from('<II',b,at);at+=8;assert np<4096 and nc<4096;p={}
 for _ in range(np):
  k=bytes(b[at:at+32]).split(b'\0')[0].decode();ln=struct.unpack_from('<I',b,at+32)[0]&0x7fffffff;at+=36;p[k]=bytes(b[at:at+ln]);at+=(ln+3)&~3
 cs=[]
 for _ in range(nc):c,at=decode(b,at);cs.append(c)
 return {'p':p,'c':cs},at
def encode(t):
 out=bytearray(struct.pack('<II',len(t['p']),len(t['c'])))
 for k,v in t['p'].items():out+=k.encode().ljust(32,b'\0')+struct.pack('<I',len(v))+v+bytes((-len(v))%4)
 for c in t['c']:out+=encode(c)
 return out
def name(t):return t['p']['name'].rstrip(b'\0').decode()
dt_off=meta['deviceTreePhysical']-meta['guestBase'];tree,used=decode(im[dt_off:dt_off+meta['deviceTreeBytes']]);assert used==meta['deviceTreeBytes']
armio=next(c for c in tree['c'] if name(c)=='arm-io');removed=[]
for c in armio['c']:
 if name(c) in ['gfx','iosurface-mapper']:
  assert c['p']['compatible'].startswith(b'paravirtualizedgraphics,');removed.append(name(c))
assert set(removed)=={'gfx','iosurface-mapper'}
armio['c']=[c for c in armio['c'] if name(c) not in removed]
tree['c'].append({'p':{'name':b'dell27-framebuffer\0','device_type':b'display\0','compatible':b'Dell27,BootFramebuffer\0','AAPL,boot-display':b''},'c':[]})
# Insert ONLY a personality into the existing IOGraphicsFamily prelink XML.
# Preserve Apple's ID/IDREF attributes and every original byte; plistlib
# reserialization would lose shared-object XML semantics.
kc=Path('/Volumes/Dell27Build/InstallerInputs/kernelcache.vma2.macho').read_bytes();at=32;pre=None
for _ in range(struct.unpack_from('<I',kc,16)[0]):
 cmd,size=struct.unpack_from('<II',kc,at)
 if cmd==0x19 and kc[at+8:at+24].rstrip(b'\0')==b'__PRELINK_INFO':pre=struct.unpack_from('<QQQQ',kc,at+24)
 at+=size
assert pre;va,vs,fo,fs=pre;original=kc[fo:fo+fs];end=original.rfind(b'</plist>')+8;assert end>0
args=meta['X0']-meta['guestBase'];virt,phys=struct.unpack_from('<QQ',im,args+8);pre_phys=va-virt+phys;off=pre_phys-meta['guestBase'];assert im[off:off+fs]==original
stack=[];nodes=[];parser=expat.ParserCreate()
def start(tag,attrs):
 node={'tag':tag,'start':parser.CurrentByteIndex,'children':[],'text':''}
 if stack:stack[-1]['children'].append(node)
 stack.append(node);nodes.append(node)
def chars(t):
 if stack:stack[-1]['text']+=t
def stop(tag):node=stack.pop();node['end']=parser.CurrentByteIndex
parser.StartElementHandler=start;parser.EndElementHandler=stop;parser.CharacterDataHandler=chars;parser.Parse(original[:end],True)
def pairs(n):return list(zip(n['children'][::2],n['children'][1::2]))
module=next(n for n in nodes if n['tag']=='dict' and any(k['tag']=='key' and k['text']=='CFBundleIdentifier' and v['text']=='com.apple.iokit.IOGraphicsFamily' for k,v in pairs(n)))
personalities=next(v for k,v in pairs(module) if k['text']=='IOKitPersonalities');assert personalities['tag']=='dict'
addition=b'''<key>Dell27BootFramebuffer</key><dict><key>CFBundleIdentifier</key><string>com.apple.iokit.IOGraphicsFamily</string><key>IOClass</key><string>IOBootFramebuffer</string><key>IOPersonalityPublisher</key><string>com.apple.iokit.IOGraphicsFamily</string><key>IOProviderClass</key><string>IOPlatformDevice</string><key>IONameMatch</key><string>dell27-framebuffer</string><key>IOMatchCategory</key><string>IOFramebuffer</string><key>IOProbeScore</key><integer>1000</integer></dict>'''
where=personalities['end'];patched=original[:where]+addition+original[where:end];assert len(patched)<=fs
im[off:off+fs]=patched+bytes(fs-len(patched))
# Validate the new XML and round-trip addition independently, retaining IDs.
import plistlib
parsed=plistlib.loads(patched)
mod=next(x for x in parsed['_PrelinkInfoDictionary'] if x.get('CFBundleIdentifier')=='com.apple.iokit.IOGraphicsFamily')
assert mod['IOKitPersonalities']['Dell27BootFramebuffer']['IOClass']=='IOBootFramebuffer'
chosen=next(c for c in tree['c'] if name(c)=='chosen');memory_map=next(c for c in chosen['c'] if name(c)=='memory-map')
encoded=encode(tree)
if 'DeviceTree' in memory_map['p']:memory_map['p']['DeviceTree']=struct.pack('<QQ',meta['deviceTreePhysical'],len(encoded));encoded=encode(tree)
assert len(encoded)<=meta['deviceTreeCapacityBytes'];im[dt_off:dt_off+meta['deviceTreeCapacityBytes']]=encoded+bytes(meta['deviceTreeCapacityBytes']-len(encoded));struct.pack_into('<I',im,args+104,len(encoded))
assert im[args+40:args+88]==bytes(48),'runtime GOP owns the actual video geometry'
im.flush();source_sha=meta['sha256'];meta.update(sha256=hashlib.sha256(im).hexdigest(),deviceTreeBytes=len(encoded),deviceProfile='Basic GOP framebuffer recovery experiment; paravirtual GPU omitted; IOGraphicsFamily matching personality added')
meta['inputRelativePath']='EngineProbe/vmapple-'+meta['sha256']+'.bin';(d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(meta,indent=2)+'\n')
header=(s/'vmapple_reference_boot.h').read_text();header=header[:header.index('static const uint8_t')]+ 'static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(meta['sha256']))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+meta['inputRelativePath'].replace('/','\\'))+'\n';(d/'vmapple_reference_boot.h').write_text(header)
proof={'sourceSHA256':source_sha,'captureSHA256':meta['sha256'],'originalPrelinkSHA256':hashlib.sha256(original).hexdigest(),'patchedPrelinkSHA256':hashlib.sha256(im[off:off+fs]).hexdigest(),'personalityInsertionBytes':len(addition),'metadataOnlyWithinPrelink':True,'removedUnimplementedDevices':removed,'guestFramebufferPhysical':hex(0x600000000),'videoGeometry':'Actual UEFI GOP at runtime','visibleInstaller':False,'acceleration':False,'stockKernelCollection':False}
(d/'PROFILE.json').write_text(json.dumps(proof,indent=2)+'\n');im.close();f.close();print(json.dumps(proof,indent=2))
