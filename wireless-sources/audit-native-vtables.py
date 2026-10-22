"""Read the exact-build x86_64 Mach-O vtables without loading any kext."""
from pathlib import Path
import struct,json,subprocess
r=Path(__file__).resolve().parent.parent
class MachO:
 def __init__(self,path):
  self.path=Path(path);self.b=self.path.read_bytes();assert struct.unpack_from('<I',self.b)[0]==0xfeedfacf
  self.segments=[];self.symbols=[];self.byname={};self.bymem={};self.relocs={};at=32
  for _ in range(struct.unpack_from('<I',self.b,16)[0]):
   cmd,size=struct.unpack_from('<II',self.b,at);assert size>=8 and at+size<=len(self.b)
   if cmd==0x19:
    vm,vmsize,fo,fs=struct.unpack_from('<QQQQ',self.b,at+24);self.segments.append((vm,vmsize,fo,fs))
   if cmd==2:symoff,nsyms,stroff,strsize=struct.unpack_from('<IIII',self.b,at+8)
   if cmd==0xb:extoff,extn=struct.unpack_from('<II',self.b,at+64)
   at+=size
  strings=self.b[stroff:stroff+strsize]
  for i in range(nsyms):
   si,nt,ns,nd,value=struct.unpack_from('<IBBHQ',self.b,symoff+16*i);end=strings.find(b'\0',si);name=strings[si:end].decode('utf8','replace');self.symbols.append(name);self.byname[name]=value
   if value and nt&0xe:self.bymem.setdefault(value,[]).append(name)
  for i in range(extn):
   addr,bits=struct.unpack_from('<iI',self.b,extoff+8*i)
   if bits&(1<<27):self.relocs[addr]=self.symbols[bits&0xffffff]
 def off(self,vm):
  for v,vs,f,fs in self.segments:
   if v<=vm<v+fs:return f+vm-v
  raise ValueError(hex(vm))
 def table(self,name):
  start=self.byname[name];end=min(x for x in self.bymem if x>start)
  # The next symbol after a vtable is its following metadata, not its entries.
  result=[]
  for vm in range(start+16,end,8):
   value=struct.unpack_from('<Q',self.b,self.off(vm))[0];names=self.bymem.get(value,[])
   result.append({'offset':hex(vm-start-16),'symbol':self.relocs.get(vm,next((n for n in names if n),hex(value)))})
  while result and result[-1]['symbol']=='0x0':result.pop()
  return result
if __name__=='__main__':
 family=MachO(r/'research/IO80211Family-24H32-x86_64.macho');bcm=MachO(r/'research/kdk-24H32-expanded/KDK.pkg/Payload/System/Library/Extensions/AppleBCMWLANCoreMac.kext/Contents/MacOS/AppleBCMWLANCoreMac')
 result={}
 for name in ['23IO80211SkywalkInterface','23IO80211VirtualInterface','19IO80211AWDLProtocol']:
  result[name]=family.table('__ZTV'+name)
 result['30AppleBCMWLANProximityInterface']=bcm.table('__ZTV30AppleBCMWLANProximityInterface')
 out=r/'research/native-awdl-24H32-vtables.json';out.write_text(json.dumps(result,indent=2)+'\n')
 for name,slots in result.items():
  print(name,len(slots),'slots; last',slots[-1] if slots else None)
  if name=='19IO80211AWDLProtocol':
   base=result['23IO80211VirtualInterface']
   for i,s in enumerate(slots):
    if i>=len(base):print(s['offset'],s['symbol'],'concrete:',result['30AppleBCMWLANProximityInterface'][i]['symbol'])
