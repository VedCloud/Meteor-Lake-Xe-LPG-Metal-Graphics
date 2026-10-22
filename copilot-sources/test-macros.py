from pathlib import Path
import json,subprocess,tempfile,sys
r=Path(__file__).resolve().parent.parent
source=Path(sys.argv[1]) if len(sys.argv)>1 else r/'research/VoodooPS2/VoodooPS2Keyboard/VoodooPS2Keyboard.cpp'
s=source.read_text(errors='replace')
body=s[s.index('bool ApplePS2Keyboard::compareMacro'):s.index('void ApplePS2Keyboard::onMacroTimer')]
body+=s[s.index('void ApplePS2Keyboard::dispatchInvertBuffer()'):s.index('// - - -',s.index('void ApplePS2Keyboard::dispatchInvertBuffer()'))]
macros=json.loads((r/'port-artifacts/copilot-command/macros.json').read_text())
head=r'''
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>
#include <algorithm>
using UInt8=uint8_t;using UInt16=uint16_t;
constexpr int kPacketLength=16,kPacketKeyDataLength=2,kPacketTimeOffset=8,kPrefixBytes=8,kSequenceBytesOffset=8,kModifierBytesOffset=4,kOutputBytesOffset=2;
void absolutetime_to_nanoseconds(uint64_t n,uint64_t*p){*p=n;}void cancelTimer(int){}void setTimerTimeout(int,uint64_t){}
struct OSData {std::vector<UInt8> v;int getLength(){return v.size();}const void*getBytesNoCopy(){return v.data();}};
struct ApplePS2Keyboard {
OSData**_macroInversion;int _macroTimer=1,_macroCurrent=0;uint16_t _PS2modifierState=0;uint64_t _macroMaxTime=25000000;alignas(8) UInt8 storage[16*8]{};UInt8*_macroBuffer=storage;
std::vector<std::array<int,2>> out;
bool compareMacro(const UInt8*,const UInt8*,int);bool invertMacros(const UInt8*);void dispatchInvertBuffer();
void dispatchKeyboardEventWithPacket(const UInt8*p){out.push_back({p[0],p[1]});}
void feed(int a,int b,uint64_t t){alignas(8) UInt8 p[16]{};p[0]=a;p[1]=b;memcpy(p+8,&t,8);if(!invertMacros(p))dispatchKeyboardEventWithPacket(p);}
};
'''
main='int main(){std::vector<OSData> data={'+','.join('OSData{{'+','.join(str(x) for x in bytes.fromhex(m))+'}}' for m in macros)+'};std::vector<OSData*> ptrs;for(auto &d:data)ptrs.push_back(&d);ptrs.push_back(nullptr);\n'
main+=r'''
std::array<std::array<int,2>,3> down={{{1,0x2a},{2,0x5b},{1,0x6e}}},up={{{1,0xaa},{2,0xdb},{1,0xee}}};
std::sort(down.begin(),down.end());int cases=0;
do {auto releases=up;std::sort(releases.begin(),releases.end());do{
 ApplePS2Keyboard k;k._macroInversion=ptrs.data();uint64_t t=0;
 for(auto p:down)k.feed(p[0],p[1],t+=1000000);
 k.feed(1,0x2e,t+=1000000);k.feed(1,0xae,t+=1000000);
 for(auto p:releases)k.feed(p[0],p[1],t+=1000000);
 assert((k.out==std::vector<std::array<int,2>>{{2,0x5b},{1,0x2e},{1,0xae},{2,0xdb}}));assert(k._macroCurrent==0);cases++;
}while(std::next_permutation(releases.begin(),releases.end()));}while(std::next_permutation(down.begin(),down.end()));assert(cases==36);
{ApplePS2Keyboard k;k._macroInversion=ptrs.data();k.feed(1,0x2a,0);k.feed(1,0x2e,1000000);k.feed(1,0xae,2000000);k.feed(1,0xaa,3000000);k.dispatchInvertBuffer();assert((k.out==std::vector<std::array<int,2>>{{1,0x2a},{1,0x2e},{1,0xae},{1,0xaa}}));}
{ApplePS2Keyboard k;k._macroInversion=ptrs.data();k.feed(1,0x2a,0);k.feed(2,0x5b,30000000);k.dispatchInvertBuffer();assert((k.out==std::vector<std::array<int,2>>{{1,0x2a},{2,0x5b}}));}
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(head+body+main)
 subprocess.run(['xcrun','clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
print('PASS actual upstream inversion code: 36 make/release permutations with held Command+C; ordinary Shift+C and timeout flush. User confirmed working Command key on Dell on 2026-10-21.')
