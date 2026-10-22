#include "LatitudeInputSuppression.h"
#include <cassert>
#include <vector>
#include <utility>
#include <cstdio>
using namespace LatitudeTablet;
int main(){
 KeyboardGate normal;assert(normal.accept(5,false));assert(normal.accept(5,true));assert(normal.accept(5,true));assert(normal.accept(5,false));
 KeyboardGate g;std::vector<std::pair<unsigned,bool>> events;
 auto emit=[&](unsigned k,bool d){events.push_back({k,d});};
 auto send=[&](unsigned k,bool d){if(g.accept(k,d))emit(k,d);};
 send(0x37,true);send(0,true);assert(events.size()==2);g.transition(true,emit);assert(events.size()==4&&!events[2].second&&!events[3].second);
 g.transition(true,emit);assert(events.size()==4);send(0x37,true);send(2,true);assert(events.size()==4);
 g.transition(false,emit);send(0x37,true);send(2,true);assert(events.size()==4);send(0x37,false);send(2,false);send(0,false);assert(events.size()==4);
 send(0x37,true);send(0x37,false);assert(events.size()==6); // Copilot's translated Command follows the same gate.
 PointerGate p;uint32_t out=0;unsigned releases=0;auto release=[&](){releases++;};assert(p.pointer(1,out)&&out==1);p.transition(true,release);assert(releases==1&&!p.scroll());assert(!p.pointer(3,out));p.transition(false,release);assert(p.scroll());assert(p.pointer(3,out)&&out==0);assert(p.pointer(2,out)&&out==0);assert(p.pointer(0,out)&&out==0);assert(p.pointer(1,out)&&out==1);
 assert(suppressionLease(2000000000ULL,true,true,true,1000000000ULL));
 assert(!suppressionLease(3000000000ULL,true,true,true,1000000000ULL));assert(!suppressionLease(1,true,true,true,2));assert(!suppressionLease(1,true,true,true,0));assert(!suppressionLease(2,false,true,true,1));assert(!suppressionLease(2,true,false,true,1));assert(!suppressionLease(2,true,true,false,1));
 puts("PASS release held keys/buttons once, quarantine until physical release, Command retention, stale/future/missing/unqualified signal fail-open. Device-local gate only.");
}
