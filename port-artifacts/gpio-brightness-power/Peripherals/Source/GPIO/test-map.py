from pathlib import Path
import tempfile,subprocess,re
r=Path(__file__).resolve().parent.parent
# Confirm live firmware descriptor pin and authoritative Linux software->pad mapping.
s=(r/'research/pinctrl-meteorlake.c').read_text();assert 'INTEL_GPP(1, 229, 253, 384)' in s and 'PINCTRL_PIN(232, "GPP_D_3")' in s
p=Path('/Volumes/SEQOUIA/iGPU-Port/Desktop/reports/failure-detail-20261021-114143/input/TPD0-acpi.txt');m=re.search(r'"LatitudeI2C-SBFG" = <([0-9a-f]+)>',p.read_text());b=bytes.fromhex(m.group(1));pin_at=int.from_bytes(b[14:16],'little');assert int.from_bytes(b[pin_at:pin_at+2],'little')==387
code=r'''
#include "latitude_gpio_map.h"
#include <cassert>
int main(){assert(LAT_BIT==8&&LAT_PAD-LAT_COMM_BASE==28);uint32_t out;
 for(unsigned pin=0;pin<65536;pin++)assert(latitudePin(pin)==(pin==387));
 for(unsigned base=0;base<0x20000;base++){
  bool ok=latitudePadOffset(base,0x10000,&out);if(ok){assert(base>=0x220&&!(base&15)&&out==base+448&&out+16<=0x10000);}
 }
 for(unsigned bits=0;bits<65536;bits++){
  uint32_t old=(bits<<16)|bits,next=0;bool ok=latitudePadPlan(old,0,0,&next);
  if(ok){assert((next&(15u<<10))==0&&!(next&(1u<<9))&&(next&(1u<<8))&&(next&(1u<<23)));assert(!(next&(15u<<17)));}
  assert(!latitudePadPlan(old,1u<<12,0,&next));
 }
 assert(!latitudePadPlan(0xffffffff,0,0,&out));assert(!latitudePadPlan(0,0,8,&out));
 assert(latitudePadPlan((1u<<23)|(1u<<8),0,8,&out));
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(code);subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-I'+str(r/'gpio-sources'),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
print('PASS firmware GPIO387 -> Linux pad232 GPP_D_3; all65536 pin/plan cases, MMIO bounds, foreign-owner and lock refusal.')
