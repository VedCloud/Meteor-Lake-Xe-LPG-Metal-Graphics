"""Generate exact-build declarations from the UUID-matched native vtable audit.
These declarations do not create an interface or advertise AWDL support.
"""
from pathlib import Path
import re,json,subprocess
r=Path(__file__).resolve().parent.parent;d=json.loads((r/'research/native-awdl-24H32-vtables.json').read_text())
base=d['23IO80211SkywalkInterface'];virtual=d['23IO80211VirtualInterface'];awdl=d['19IO80211AWDLProtocol'];bcm=d['30AppleBCMWLANProximityInterface']
assert len(base)*8==0xe60 and len(virtual)*8==0xfa0 and len(awdl)*8==0x1198
header='\n'.join(p.read_text() for p in (r/'desktop-build/airport-awdl-radio/include/Airport/macOS15').glob('*.h'))
def demangle(symbol):return subprocess.check_output(['xcrun','c++filt',symbol[1:]],text=True).strip()
lines=['#pragma once','// Exact x86_64 IO80211Family UUID 13D3592A-A6BE-35E7-984C-0ACD90A76356.', '// Generated from 24H32 vtables and matching concrete native override signatures.', '// Native object allocation and traffic capability are not enabled by this header.', '#include <Airport/Apple80211.h>', 'class IO80211VirtualInterface : public IO80211SkywalkInterface {',' OSDeclareAbstractStructors(IO80211VirtualInterface)','public:']
overrides=[]
for i,entry in enumerate(virtual[:len(base)]):
 if entry['symbol'].startswith('__ZN23IO80211VirtualInterface') and entry['symbol']!=base[i]['symbol']:
  signature=demangle(entry['symbol']).split('::',1)[1];name=signature.split('(')[0]
  if name.startswith('~') or name.startswith('IO80211VirtualInterface') or name in ['getMetaClass']:continue
  # All virtual overrides are exact return-compatible with the known Skywalk declaration.
  match=re.search(r'virtual ([^;\n]*\b'+re.escape(name)+r'\([^;\n]*?\)(?: const)?)(?: override)?(?: = 0)?;',header)
  if not match:
   special={'terminate':'bool terminate(IOOptionBits)','attach':'bool attach(IOService *)','detach':'void detach(IOService *)'}
   if name in special:
    lines.append(' virtual '+special[name]+' override; // '+entry['offset']);continue
   # Inherited private ABI placeholders retain their C++ identity and slot.
   candidates=re.findall(r'virtual void (\w+)\(\) __asm__\("([^\"]+)"\)',header)
   alias=next((n for n,mangled in candidates if demangle(mangled).split('::',1)[1]==signature),None)
   assert alias,(entry,signature)
   lines.append('private: virtual void '+alias+'() __asm__("'+entry['symbol']+'") override; public:');continue
  decl=re.sub(r'\s+override$','',match[1]);lines.append(' virtual '+decl+' override; // '+entry['offset']);overrides.append(name)
for entry in virtual[len(base):]:
 symbol=entry['symbol'];assert symbol and symbol!='0x0';offset=entry['offset'];signature=demangle(symbol)
 if offset=='0xe60':lines.append(' virtual bool init(IO80211Controller *,ether_addr *,unsigned int,const char *); // '+offset)
 elif offset=='0xe68':lines.append(' virtual bool setLinkState(IO80211LinkState,unsigned int); // '+offset)
 else:lines.append('private: virtual void _native_'+offset[2:]+'() __asm__("'+symbol+'"); public:')
lines+=[' int dequeueTxPackets(TxPacketRequest *);','private: uint8_t _nativeLayout[0x120-sizeof(IO80211SkywalkInterface)];','};','static_assert(sizeof(IO80211VirtualInterface)==0x120,"24H32 virtual interface ABI");']
methods=[];types=set()
for i,entry in enumerate(awdl[len(virtual):],len(virtual)):
 assert entry['symbol']=='___cxa_pure_virtual',entry
 signature=demangle(bcm[i]['symbol']).split('::',1)[1]
 assert re.fullmatch(r'\w+\(\w+\*\)',signature),signature
 name,ty=signature[:-1].split('(');ty=ty[:-1];types.add(ty);methods.append({'offset':entry['offset'],'name':name,'type':ty})
lines += ['struct '+t+';' for t in sorted(types)]
lines += ['class IO80211AWDLProtocol : public IO80211VirtualInterface {',' OSDeclareAbstractStructors(IO80211AWDLProtocol)','public:']
for method in methods:lines.append(' virtual IOReturn '+method['name']+'('+method['type']+' *) = 0; // '+method['offset'])
lines+=['};','static_assert(sizeof(IO80211AWDLProtocol)==0x120,"24H32 AWDL protocol ABI");']
(r/'wireless-sources/LatitudeAWDLNativeABI.h').write_text('\n'.join(lines)+'\n')
(r/'research/native-awdl-24H32-methods.json').write_text(json.dumps({'nativeUUID':'13D3592A-A6BE-35E7-984C-0ACD90A76356','virtualSlots':len(virtual),'protocolSlots':len(awdl),'virtualOverrides':overrides,'methods':methods},indent=2)+'\n')
print('Generated 24H32 modern virtual-interface declarations and',len(methods),'typed AWDL slots. No native object allocation or capability change.')
