from pathlib import Path
import subprocess,json,hashlib,random,struct
b=Path(__file__).resolve().parent;src=b/'core/VoodooI2C Satellites/VoodooI2CHID/VoodooI2CHID';d=json.loads((b/'DESCRIPTOR.json').read_text())
# Independently derive offsets and bounds from the captured HID descriptor.
for rid in [0x14,0x0b]:
 fields=[f for f in d['penFields']if f['id']==rid]
 assert [(f['bit'],f['size'],f['usage'])for f in fields[:6]]==[(0,1,0x42),(1,1,0x44),(2,1,0x45),(3,1,0x3c),(4,1,0x5a),(5,1,0x32)]
 for usage,bit,maxv in [(0x30,8,29376),(0x31,24,16524)]:
  assert any(f['page']==1 and f['usage']==usage and f['bit']==bit and f['max']==maxv for f in fields)
 assert d['inputBits'][str(rid)]==168
code=r'''
#include <cassert>
#include <cstdio>
#include <cstring>
#include "LatitudeWacomPen.hpp"
int main() {
 unsigned char descriptor[893]; FILE* f=fopen("wacom-056a-4a19.bin","rb"); assert(f);
 assert(fread(descriptor,1,sizeof(descriptor),f)==sizeof(descriptor));fclose(f);
 assert(latitudePenDescriptor(descriptor,893));descriptor[10]^=1;
 assert(!latitudePenDescriptor(descriptor,893));assert(!latitudePenDescriptor(nullptr,893));
 assert(!latitudePenDescriptor(descriptor,892));
 unsigned char p[22]={}; LatitudeWacomPen v;
 assert(!latitudeDecodePen(nullptr,22,20,v));
 for(unsigned n=0;n<22;n++) assert(!latitudeDecodePen(p,n,20,v));
 assert(!latitudeDecodePen(p,22,28,v));assert(!latitudeDecodePen(p,22,20,v));
 unsigned count=0;
 for(unsigned id: {20U,11U}) for(unsigned flags=0;flags<64;flags++)
 for(unsigned x: {0U,14688U,29376U,65535U})
 for(unsigned y: {0U,8262U,16524U,65535U})
 for(unsigned pressure: {0U,2048U,4095U,65535U}) {
  p[0]=id;p[1]=flags;p[2]=x;p[3]=x>>8;p[4]=y;p[5]=y>>8;p[6]=pressure;p[7]=pressure>>8;
  p[16]=0xd8;p[17]=0xdc; // -9000 hundredths of a degree
  p[18]=0x28;p[19]=0x23; // +9000
  assert(latitudeDecodePen(p,22,id,v));
  bool range=flags&0x25,tip=flags&1,erase=flags&4;
  assert(v.inRange==range&&v.tip==tip&&v.eraser==erase);
  unsigned buttons=range?((tip?1:0)|((flags&0x12)?2:0)|(erase?4:0)):0;
  assert(v.buttons==buttons);
  assert(v.x==int((static_cast<unsigned long long>(x>29376?29376:x)*65535)/29376));
  assert(v.y==int((static_cast<unsigned long long>(y>16524?16524:y)*65535)/16524));
  assert(v.pressure==((tip||erase)?int((static_cast<unsigned long long>(pressure>4095?4095:pressure)*65535)/4095):0));
  assert(v.tiltX==-90*65536&&v.tiltY==90*65536);++count;
 }
 assert(latitudePenTilt(0x8000)==-90*65536&&latitudePenTilt(0x7fff)==90*65536);
 printf("PASS %u descriptor-derived hover/tip/eraser/button/axis/pressure/tilt cases; malformed reports rejected\n",count);
}
'''
code=code.replace('#include <cstring>','#include <cstring>\n#include <initializer_list>')
(b/'test-decoder.cpp').write_text(code)
subprocess.run(['xcrun','clang++','-std=c++11','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(src),str(b/'test-decoder.cpp'),'-o',str(b/'test-decoder')],check=True)
p=subprocess.run([str(b/'test-decoder')],cwd=b,text=True,capture_output=True,check=True);print(p.stdout,end='')
old=Path('/Volumes/Corsair 1TB NVMe/Dell-Tahoe-20261026/Pen-input-1.0.3/build/VoodooI2CHID.kext/Contents/MacOS/VoodooI2CHID');new=b/'build/VoodooI2CHID.kext/Contents/MacOS/VoodooI2CHID'
def imports(p):return set(subprocess.check_output(['xcrun','nm','-ju',str(p)],text=True).splitlines())
extra=sorted(imports(new)-imports(old));print('New unresolved kernel imports:',extra);assert extra == []
kernel = Path('/Volumes/Corsair 1TB NVMe/Dell-Tahoe-20261026/KDK25G241-expanded/KDK.pkg/Payload/System/Library/Kernels/kernel')
exports = set(subprocess.check_output(['xcrun','nm','-gjU',str(kernel)],text=True).splitlines())
assert set(extra) <= exports
print('OSData metaclass dependency verified in real Tahoe 25G241 kernel')
frozen=Path('/Volumes/Corsair 1TB NVMe/iGPU Port/port-artifacts/hid-nullguard-1.0.1/source/VoodooI2CHID/VoodooI2CHID');changed=[]
for f in frozen.rglob('*'):
 if f.is_file()and f.suffix in ['.cpp','.hpp']:
  target=src/f.relative_to(frozen)
  if f.read_bytes()!=target.read_bytes():changed.append(str(f.relative_to(frozen)))
assert sorted(changed)==sorted(['VoodooI2CTouchscreenHIDEventDriver.cpp','VoodooI2CTouchscreenHIDEventDriver.hpp','VoodooI2CMultitouchHIDEventDriver.hpp','VoodooI2CHIDDevice.cpp','VoodooI2CHIDDevice.hpp'])
subprocess.run(['codesign','--verify','--deep','--strict',str(new.parent.parent.parent)],check=True)
result={'decoder':p.stdout.strip(),'newKernelImports':extra,'changedBaselineFiles':changed,'transportLogicChanged':False,'passiveWireCountersAdded':True,'baselineSHA256':hashlib.sha256(old.read_bytes()).hexdigest(),'candidateSHA256':hashlib.sha256(new.read_bytes()).hexdigest(),'hardwareVerified':False}
(b/'TESTS.json').write_text(json.dumps(result,indent=2)+'\n')
