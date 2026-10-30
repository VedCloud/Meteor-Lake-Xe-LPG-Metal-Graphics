from pathlib import Path
import sys,struct,re,json,hashlib
from collections import defaultdict
import capstone
from capstone.x86 import X86_OP_MEM,X86_OP_IMM,X86_OP_REG,X86_REG_RSP,X86_REG_RBP,X86_REG_RIP
b=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');sys.path.insert(0,str(r/'NullMoth/tools'))
from vtdump import load,vtables
base=r/'research/kdk-24H32-expanded/KDK.pkg/Payload/System/Library/Extensions/IOAcceleratorFamily2.kext/Contents/MacOS/IOAcceleratorFamily2'
new=b/'ABI-reference/com.apple.iokit.IOAcceleratorFamily2'
if not base.is_file():base=r/'research/IOAcceleratorFamily2-recovery-reference.macho'
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64);md.detail=True
class Image:
 def __init__(self,p):
  self.p=p;self.d,self.syms,self.ext,self.fo,self.byaddr=load(p)
  text=[];at=32
  for _ in range(struct.unpack_from('<I',self.d,16)[0]):
   cmd,ln=struct.unpack_from('<II',self.d,at)
   if cmd==0x19:
    ns=struct.unpack_from('<I',self.d,at+64)[0]
    for j in range(ns):
     sc=at+72+80*j;name=self.d[sc:sc+16].split(b'\0')[0]
     if name==b'__text':
      start,size=struct.unpack_from('<QQ',self.d,sc+32);text.append((start,start+size))
   at+=ln
  self.names={n:v for n,t,s,v in self.syms if s and not(t&0xe0) and any(lo<=v<hi for lo,hi in text)}
  self.ends={a:next((bb for bb in sorted(self.byaddr) if bb>a),a+2048) for a in self.names.values()}
 def ins(self,n):
  a=self.names[n];off=self.fo(a)
  if off is None:return []
  return list(md.disasm(self.d[off:off+min(self.ends[a]-a,8192)],a))
 def size(self,c):
  for n in ['__ZN'+str(len(c))+c+'9MetaClassC1Ev','__ZN'+str(len(c))+c+'9MetaClassC2Ev']:
   if n in self.names:
    for i in self.ins(n):
     if i.mnemonic=='mov' and i.op_str.startswith('ecx, ') and i.operands[1].type==X86_OP_IMM:return i.operands[1].imm
  return None
 def fields(self,n):
  # Compare member-memory displacement/width/direction in order, excluding instruction, stack and frame pointers.
  a=[]
  for i in self.ins(n):
   for op in i.operands:
    if op.type==X86_OP_MEM and op.mem.base not in [X86_REG_RSP,X86_REG_RBP,X86_REG_RIP] and op.mem.base:
     a.append((i.mnemonic,op.mem.disp,op.size,op.access))
  return a
 def tables(self):
  v=vtables(self.p)
  for c,slots in v.items():
   for i,s in enumerate(slots):
    if s.startswith('0x'):
     raw=int(s,16);target=raw&0x3fffffff
     if raw in self.byaddr:slots[i]=self.byaddr[raw]
     elif target in self.byaddr:slots[i]=self.byaddr[target]
     else:slots[i]='external-or-private'
  return v
old,cur=Image(base),Image(new)
classes=['IOGraphicsAccelerator2','IOAccelDisplayMachine','IOAccelLegacyDisplayMachine','IOAccelDisplayPipe','IOAccelLegacyDisplayPipe','IOAccelResource2','IOAccelSysMemory','IOAccelVidMemory','IOAccelMemoryMap','IOAccelLegacySurface','IOAccelGLContext2','IOAccel2DContext2','IOAccelCLContext2','IOAccelVideoContext2','IOAccelEventMachineFast2','IOAccelTask']
vtold,vtnew=old.tables(),cur.tables();report={'old':str(base),'new':str(new),'classes':{},'memberAccesses':{},'physicalExecution':False}
for c in classes:
 k=str(len(c))+c;a,bb=vtold.get(k,[]),vtnew.get(k,[])
 diffs=[{'slot':i,'old':x,'new':y} for i,(x,y) in enumerate(zip(a,bb)) if x!=y and x!='external-or-private' and y!='external-or-private']
 report['classes'][c]={'oldSize':old.size(c),'newSize':cur.size(c),'oldSlots':len(a),'newSlots':len(bb),'namedSlotChanges':diffs}
 assert old.size(c)==cur.size(c) and old.size(c) is not None,c
 assert len(a)==len(bb) and len(a)>0,c
# Store all same-named functions with direct member accesses. A changed function is evidence to review, never silently a pass.
for n in sorted(set(old.names)&set(cur.names)):
 if not n.startswith('__ZN') or not any(c in n for c in classes):continue
 f1,f2=old.fields(n),cur.fields(n)
 if f1 or f2:report['memberAccesses'][n]={'same':f1==f2,'old':f1,'new':f2}
report['memberAccessMatched']=sum(x['same'] for x in report['memberAccesses'].values());report['memberAccessChanged']=sum(not x['same'] for x in report['memberAccesses'].values())
report['scope']='Class sizes, vtable slot counts and exact member-memory operand sequences for named methods; does not claim full behavioral equivalence.'
(b/'CLASS-LAYOUT-25G83.json').write_text(json.dumps(report,indent=2)+'\n')
print('Class sizes/slot counts match:',len(classes));print('Named method member-access sequences:',report['memberAccessMatched'],'matched,',report['memberAccessChanged'],'changed')
for c,i in report['classes'].items():
 if i['namedSlotChanges']:print('Slot names changed',c,len(i['namedSlotChanges']),i['namedSlotChanges'][:2])
for n,i in report['memberAccesses'].items():
 if not i['same']:print('Changed',n,'old',i['old'][:6],'new',i['new'][:6])
