from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent
code=r'''
#include "latitude_brightness_policy.h"
#include <assert.h>
int main(){for(unsigned key=0;key<65536;key++)for(unsigned flags=0;flags<16;flags++){
 uint64_t mask=(uint64_t)flags<<17;int d=latitudeBrightnessKey(key,mask);assert(d==((flags==0||flags==6)?(key==97?-1:key==98?1:0):0));
}for(unsigned code=0;code<256;code++)for(unsigned state=0;state<256;state++)assert(latitudeBrightnessSystem(8,code,state)==(state==10?(code==2?1:code==3?-1:0):0));
for(unsigned i=0;i<=65535;i++){float x=i/65535.0f;float down=latitudeBrightnessStep(x,-1),up=latitudeBrightnessStep(x,1);assert(down>=0&&down<=x&&up>=x&&up<=1);}}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(code);subprocess.run(['xcrun','clang++','-fsanitize=address,undefined','-I'+str(r),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
print('PASS brightness routing over all65536 keycodes/modifier combinations; system key down/up handling and bounded steps. No keyboard monitoring or hardware writes.')
