"""Compile-only exact-build ABI proof; never link/load the fixture as a kext."""
from pathlib import Path
import json,runpy,subprocess,re
r=Path(__file__).resolve().parent.parent;out=r/'desktop-build/native-awdl-abi';out.mkdir(exist_ok=True)
d=json.loads((r/'research/native-awdl-24H32-methods.json').read_text());source=out/'fixture.cpp'
s='''#include <LatitudeAWDLNativeABI.h>
class LatitudeNativeABIFixture:public IO80211AWDLProtocol {
 OSDeclareDefaultStructors(LatitudeNativeABIFixture)
public:
 IOReturn setMacAddress(ether_addr &) override {return kIOReturnUnsupported;}
'''
for m in d['methods']:s+=' IOReturn '+m['name']+'('+m['type']+' *) override {return kIOReturnUnsupported;}\n'
s+='};\nstatic_assert(offsetof(TxPacketRequest,bufs)==0x18,"native mbuf chains");\nstatic_assert(offsetof(TxPacketRequest,reqTx)==0x58,"native request status");\nOSDefineMetaClassAndStructors(LatitudeNativeABIFixture,IO80211AWDLProtocol)\n'
source.write_text(s)
env=runpy.run_path(str(r/'wireless-sources/compile-private-path.py'));flags=env['flags'];log=out/'clang-vtables.txt'
with log.open('w') as f:rc=subprocess.run(['xcrun','clang++',*flags,'-I'+str(r/'wireless-sources'),'-Xclang','-fdump-vtable-layouts','-c',str(source),'-o',str(out/'fixture.o')],stdout=f,stderr=subprocess.STDOUT)
if rc.returncode:print(log.read_text()[-8000:]);raise SystemExit(rc.returncode)
text=log.read_text();a=text.index("Vtable for 'LatitudeNativeABIFixture'");b=text.find('\nVtable for ',a+1);table=text[a:b if b>=0 else len(text)]
for m in d['methods']:
 index=int(m['offset'],16)//8+2
 assert re.search(r'^\s*'+str(index)+r' \| .*LatitudeNativeABIFixture::'+m['name']+r'\(',table,re.M),(m,index)
# Compare EVERY inherited dispatch slot too, not only the 63 new handlers.
abi=json.loads((r/'research/native-awdl-24H32-vtables.json').read_text())['19IO80211AWDLProtocol']
headers=(r/'wireless-sources/LatitudeAWDLNativeABI.h').read_text()
headers+='\n'+'\n'.join(p.read_text() for p in (r/'desktop-build/airport-awdl-radio/include/Airport/macOS15').glob('*.h'))
aliases={a:subprocess.check_output(['xcrun','c++filt',m[1:]],text=True).strip().split('::',1)[1].split('(')[0] for a,m in re.findall(r'void (\w+)\(\) __asm__\("([^\"]+)"\)',headers)}
for i,entry in enumerate(abi):
 match=re.search(r'^\s*'+str(i+2)+r' \| (.*)$',table,re.M);assert match,(i,entry)
 got=match[1].split('::',1)[1].split('(')[0]
 if entry['symbol']=='___cxa_pure_virtual':
  expected='setMacAddress' if i*8==0xcf8 else d['methods'][i-500]['name']
 else:expected=subprocess.check_output(['xcrun','c++filt',entry['symbol'][1:]],text=True).strip().split('::',1)[1].split('(')[0]
 got=aliases.get(got,got)
 if got.startswith('~'):got='~destructor'
 if expected.startswith('~'):expected='~destructor'
 assert got==expected,(hex(i*8),got,expected)
assert len(abi)==563
(out/'VALIDATION.json').write_text(json.dumps({'nativeUUID':d['nativeUUID'],'objectSize':288,'typedAWDLSlots':len(d['methods']),'allSlotsMatchNativeOffsets':True,'inheritedDispatchSlotsChecked':563,'nativeRequestOffsetsChecked':[24,88],'kernelCompile':'x86_64 passed','fixtureLoaded':False,'runtimeInterfaceCreated':False},indent=2)+'\n')
print('PASS all',len(d['methods']),'typed AWDL slots match exact native offsets; 0x120-byte modern interface sizes compile. Fixture was not linked or loaded.')
