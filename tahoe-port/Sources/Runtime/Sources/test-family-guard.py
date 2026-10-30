from pathlib import Path
import struct,uuid,subprocess,plistlib,shutil,json
b=Path(__file__).resolve().parent;p=b/'Tahoe-Combined-Runtime';helper=b/'tahoe-runtime-check-fixed';qual=p/'ABI-qualifications.plist';families=plistlib.loads(qual.read_bytes())['25G241']
def fileset(items):
 cmds=[];children=[];offset=4096
 for ident,u in items:
  text=ident.encode()+b'\0';length=(32+len(text)+7)&~7
  cmds.append(struct.pack('<IIQQII',0x80000035,length,0x100000000+offset,offset,32,0)+text+b'\0'*(length-32-len(text)))
  child=struct.pack('<IIIIIIII',0xfeedfacf,0x1000007,3,11,1,24,0,0)+struct.pack('<II',0x1b,24)+uuid.UUID(u).bytes;children.append((offset,child));offset+=256
 data=bytearray(struct.pack('<IIIIIIII',0xfeedfacf,0x1000007,3,12,len(cmds),sum(map(len,cmds)),0,0)+b''.join(cmds));assert len(data)<=4096;data.extend(b'\0'*(offset-len(data)))
 for off,child in children:data[off:off+len(child)]=child
 return bytes(data)
root=b/'FamilyGuardTests';root.mkdir(exist_ok=True);items=[(x['id'],x['uuid']) for x in families];results=[]
for case in ['valid-fileset','wrong-embedded-uuid','missing-module','duplicate-module','out-of-bounds-entry','wrong-child-architecture','conflicting-collections','fat-x86-slice']:
 r=root/case;r.mkdir(exist_ok=True);k=r/'System/Library/KernelCollections';k.mkdir(parents=True,exist_ok=True);v=items[:]
 if case=='wrong-embedded-uuid':v[0]=(v[0][0],'00000000-0000-0000-0000-000000000000')
 if case=='missing-module':v=v[1:]
 if case=='duplicate-module':v.append(v[0])
 data=bytearray(fileset(v))
 if case=='out-of-bounds-entry':struct.pack_into('<Q',data,32+16,len(data)+4096)
 if case=='wrong-child-architecture':struct.pack_into('<I',data,4096+4,0x100000c)
 if case=='fat-x86-slice':data=bytearray(struct.pack('>II',0xcafebabe,1)+struct.pack('>IIIII',0x1000007,3,4096,len(data),12)+b'\0'*(4096-28)+data)
 (k/'BootKernelExtensions.kc').write_bytes(data)
 if case=='conflicting-collections':(k/'SystemKernelExtensions.kc').write_bytes(fileset([(items[0][0],'00000000-0000-0000-0000-000000000000')]))
 # A correct loose executable cannot override a mismatched/invalid kernel collection.
 if case=='wrong-embedded-uuid':
  dst=r/families[0]['paths'][0];dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(b/'KDK25G241-expanded/KDK.pkg/Payload'/families[0]['paths'][0],dst)
 run=subprocess.run([str(helper),'--families',str(qual),'25G241',str(r)],capture_output=True,text=True);expected=0 if case in ['valid-fileset','fat-x86-slice'] else 2;assert run.returncode==expected,(case,run.stdout,run.stderr);results.append({'case':case,'passed':True,'exit':run.returncode});print('PASS',case)
# Real Apple containers from the verified Tahoe26.6.2 recovery; no fabricated image for this check.
run=subprocess.run([str(helper),'--families',str(qual),'25G83','/Volumes/macOS Base System 1'],capture_output=True,text=True);assert run.returncode==0,run.stderr;results.append({'case':'real-Apple25G83-Boot-and-BaseSystem-collections','passed':True})
run=subprocess.run([str(helper),'--families',str(qual),'25G241',str(b/'KDK25G241-expanded/KDK.pkg/Payload')],capture_output=True,text=True);assert run.returncode==0,run.stderr;results.append({'case':'real-Apple25G241-KDK-standalone-files','passed':True})
(b/'FAMILY-GUARD-TESTS.json').write_text(json.dumps({'tests':results,'guardBypassed':False,'DellGraphicsInstalled':False},indent=2)+'\n');print('PASS both real Apple references; 10 guard cases total.')
# Tiny valid filesets for shell fixtures, which previously used text placeholders as Boot/System KC inputs.
(b/'fixture-family-kc.bin').write_bytes(fileset(items));(b/'fixture-empty-kc.bin').write_bytes(fileset([]))
