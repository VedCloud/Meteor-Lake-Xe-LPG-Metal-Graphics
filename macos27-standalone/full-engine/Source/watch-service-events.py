"""Bounded host-side parser for genuine boot events, excluding unit fixtures.
No guest changes. Only already-exported return addresses and status codes.
"""
from pathlib import Path
import datetime,json,re,subprocess,time,os
R=Path(__file__).resolve().parent
cache=Path(os.environ.get('DELL27_DYLD_CACHE_DIR','/Volumes/macOS Base System/System/Library/dyld'))/'dyld_shared_cache_arm64e'
assert cache.is_file(), 'Mount the exact recovery image read-only and set DELL27_DYLD_CACHE_DIR'
A=json.loads((R/'ACTIVE-valid.json').read_text());D=Path(A['run']);L=Path(A['log'])
start='Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay'
pattern=re.compile(r'OBSERVED service (\S+) pc=(0x[0-9a-f]+) slide=(0x[0-9a-f]+) lr=(0x[0-9a-f]+) status=(0x[0-9a-f]+) occurrence=(\d+)')
symbols={};last={};deadline=time.monotonic()+86400
def resolve(pc,slide):
 address=pc-slide
 if not 0x180000000<=address<0x200000000:return None
 if address not in symbols:
  p=subprocess.run(['/Volumes/Dell27Build/ipsw-tools/ipsw','dyld','a2s',str(cache),hex(address),'--cache','/Volumes/Dell27Build/FramebufferBuild/References/arm64e-26A428.a2s'],capture_output=True,text=True,timeout=20)
  symbols[address]=p.stdout.strip() if p.returncode==0 else None
 return symbols[address]
print('Watching real service events:',D,flush=True)
while time.monotonic()<deadline:
 raw=L.read_text(errors='replace').replace('\r','')
 if start not in raw:time.sleep(5);continue
 runtime=raw.split(start,1)[1];events=[]
 for match in pattern.finditer(runtime):
  name,pc,slide,lr,status,number=match.groups();slide=int(slide,16)
  tail=runtime[match.end():];tail=tail.split('OBSERVED service',1)[0]
  returns=[]
  for line in tail.splitlines()[1:]:
   m=re.fullmatch(r'  service return\[(\d+)\]=(0x[0-9a-f]+)',line)
   if not m:break
   returns.append({'PC':m[2],'symbol':resolve(int(m[2],16),slide)})
  event={'name':name,'PC':pc,'slide':hex(slide),'LR':lr,'LRsymbol':resolve(int(lr,16),slide),'status':status,'occurrence':int(number),'returns':returns}
  events.append(event)
  key=(name,slide,number)
  if key not in last:print(json.dumps(event),flush=True)
  last[key]=event
 report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(D),'EFI_SHA256':A['EFI_SHA256'],'scope':'Genuine replay only; fixture events excluded; selected statuses/return addresses only; no service errors suppressed','events':events,'graphicalInstallerVerified':False}
 tmp=D/'SERVICE-EVENTS.tmp';tmp.write_text(json.dumps(report,indent=2)+'\n');tmp.replace(D/'SERVICE-EVENTS.json')
 if 'DONE engine integration tests' in runtime:break
 time.sleep(10)
print('Service event parser ended; boot untouched.',flush=True)
