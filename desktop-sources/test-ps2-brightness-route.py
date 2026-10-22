from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
s=(r/'research/VoodooPS2/VoodooPS2Keyboard/VoodooPS2Keyboard.cpp').read_text(errors="replace")
a=s.index('IOReturn ApplePS2Keyboard::message(');b=s.index('{',a);j=b+1;depth=1
while depth:depth+=(s[j]=='{')-(s[j]=='}');j+=1
method=s[a:j]
h=(r/'research/VoodooPS2/VoodooPS2Keyboard/ApplePS2ToADBMap.h').read_text(errors="replace");a=h.index('static const UInt8 PS2ToADBMapStock[');b=h.index('\n};',a)+3;table=h[a:b]
pre=r'''
#include <cstdint>
#include <vector>
#include <utility>
#include <cassert>
#include <cstdio>
using UInt8=uint8_t;using UInt16=uint16_t;using UInt32=uint32_t;using IOReturn=int;
#define ADB_CONVERTER_LEN 512
#define DEADKEY 0x80
#define BRIGHTNESS_DOWN 0x6b
#define BRIGHTNESS_UP 0x71
#define kPacketLength 16
#define kPacketTimeOffset 8
constexpr int kIOReturnSuccess=0,kIOACPIMessageDeviceNotification=100,kPS2K_getKeyboardStatus=101,kPS2K_setKeyboardStatus=102,kPS2K_notifyKeystroke=103;
struct IOService{};struct PS2KeyInfo{uint64_t time;UInt16 adbKeyCode;bool goingDown,eatKey;};
void clock_get_uptime(uint64_t *v){*v=1;}
'''
classes=r'''
struct ApplePS2Keyboard{
 void *_macroInversion=nullptr;bool _disableInput=false;std::vector<std::pair<unsigned,bool>>events;
 bool invertMacros(UInt8*){return false;}
 void dispatchKeyboardEventWithPacket(UInt8*p){events.push_back({PS2ToADBMapStock[(p[0]==2?256:0)+(p[1]&0x7f)],!(p[1]&0x80)});}
 void dispatchKeyboardEventX(unsigned k,bool down,uint64_t){events.push_back({k,down});}
 IOReturn message(UInt32,IOService*,void*);
};
'''
main=r'''
int main(){for(auto pair:{std::pair<unsigned,unsigned>{0x0365,0x6b},{0x0366,0x71}}){ApplePS2Keyboard k;unsigned code=pair.first;assert(!k.message(kIOACPIMessageDeviceNotification,nullptr,&code));assert(k.events.size()==2);assert(k.events[0]==std::make_pair(pair.second,true));assert(k.events[1]==std::make_pair(pair.second,false));}
 puts("PASS upstream VoodooPS2 message implementation + actual stock map: 0x0365/0x0366 emit exactly one F14/F15 make/break pair, matching BrightnessKeys brightness codes. No keyboard/device input.");}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(pre+table+classes+method+main)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined','-g',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
