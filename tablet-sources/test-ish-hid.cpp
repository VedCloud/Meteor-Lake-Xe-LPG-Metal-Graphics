#include "latitude_ish_hid.h"
#include <cassert>
#include <vector>
#include <cstdio>
using namespace LatitudeISH;
static bool response(HID&h,const std::vector<uint8_t>&data,unsigned time){auto req=h.peek();assert(h.sent());std::vector<uint8_t>p={uint8_t(req.data[0]|0x80),req.data[1],0,0,uint8_t(data.size()),uint8_t(data.size()>>8)};p.insert(p.end(),data.begin(),data.end());return h.input(p.data(),p.size(),time);}
int main(){
 HID h;assert(h.start(0));assert(response(h,{1,7,0,0,0,0,0xc2,0x0a,0x87,0x80},1));assert(h.peek().data[0]==0);assert(response(h,{9,0x21},2));assert(h.peek().data[0]==1);
 assert(response(h,std::vector<uint8_t>(measuredAccelDescriptor,measuredAccelDescriptor+sizeof measuredAccelDescriptor),3));assert(h.peek().data[0]==2&&h.peek().length==7);
 std::vector<uint8_t>feature(289);feature[0]=3;feature[4]=2;assert(response(h,feature,4));assert(h.peek().data[0]==4);
 std::vector<uint8_t>input(136);input[0]=3;input[1]=2;input[2]=5;input[11]=1;int32_t axes[3]={0,-708046,-712327};for(unsigned a=0;a<3;a++)for(unsigned b=0;b<4;b++)input[27+a*4+b]=uint8_t(uint32_t(axes[a])>>(b*8));
 assert(response(h,input,5));auto sample=h.current(5);assert(sample.fresh&&sample.y< -6900&&sample.z< -6900);assert(!h.current(1006).fresh);
 assert(h.tick(105));assert(response(h,input,106));assert(h.current(106).hostMS==5); // repeated firmware timestamp is not a fresh reading
 assert(h.tick(206));input[11]=2;input[1]=1;assert(response(h,input,207));assert(h.current(207).hostMS==5); // not-ready sensor
 assert(h.tick(307));input[1]=2;assert(response(h,input,308));assert(h.current(308).fresh&&h.current(308).hostMS==308);
 assert(!h.tick(307));
 HID bad;assert(bad.start(0));assert(!response(bad,{2,7},1));assert(bad.state()==HID::Phase::Failed);
 HID asleep;assert(asleep.start(0));assert(response(asleep,{1,7,0,0,0,0,0xc2,0x0a,0x87,0x80},1));assert(response(asleep,{9},2));assert(response(asleep,std::vector<uint8_t>(measuredAccelDescriptor,measuredAccelDescriptor+sizeof measuredAccelDescriptor),3));feature[4]=6;assert(response(asleep,feature,4));assert(asleep.state()==HID::Phase::NeedsPower&&!asleep.hasRequest());
 puts("PASS HOSTIF device enumeration, exact captured descriptor binding, read-only feature/power checks, real-format acceleration scaling, duplicate/not-ready/stale rejection and request/reply state under ASan/UBSan. No feature/power writes.");
}
