#include "latitude_ishtp_hbm.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace LatitudeISH;
static void setup(HBM&s){
 assert(s.start(0));HBM::BusMessage m;assert(s.pop(m)&&m.size==4&&!memcmp(m.bytes,"\1\0\0\1",4));
 const uint8_t version[]={0x81,1,0,1};assert(s.bus(version,4,1,s.generation()));assert(s.pop(m)&&m.bytes[0]==4);
 uint8_t e[36]={0x84};e[4]=1<<5;assert(s.bus(e,sizeof e,2,s.generation()));assert(s.pop(m)&&m.bytes[0]==5&&m.bytes[1]==5);
 uint8_t p[32]={0x85,5,0,0,0x58,0xcd,0xae,0x33,0x79,0xb6,0x54,0x4e,0x9b,0xd9,0xa0,0x4d,0x34,0xf0,0xc2,0x26};p[20]=1;p[21]=1;p[23]=1;p[25]=0x10;
 assert(s.bus(p,sizeof p,3,s.generation()));assert(s.pop(m)&&m.bytes[0]==6&&m.bytes[1]==5&&m.bytes[2]==1);
 const uint8_t c[]={0x86,5,1,0};assert(s.bus(c,4,4,s.generation()));assert(s.pop(m)&&m.size==8&&m.bytes[0]==8);assert(s.state()==HBM::State::Connected);
}
int main(){
 HBM s;setup(s);auto gen=s.generation();HBM::BusMessage m;
 assert(!s.takeTransmitCredit(gen,10));uint8_t credit[8]={8,5,1};assert(s.bus(credit,8,5,gen));assert(s.takeTransmitCredit(gen,10));assert(!s.takeTransmitCredit(gen,10));
 uint8_t a[128]={5,1,124,0};a[4]=3;assert(s.input(a,128,6,gen)==0);assert(!s.pop(m)&&!s.data());uint8_t b[16]={5,1,12,0x80};assert(s.input(b,16,7,gen)==1);assert(s.size()==136&&s.data()[0]==3);assert(!s.pop(m));assert(s.input(b,16,8,gen)==-1);assert(s.consumed(gen));assert(s.pop(m)&&m.bytes[0]==8);assert(!s.consumed(gen));
 s.reset();auto newGen=s.generation();assert(newGen!=gen);assert(!s.bus(credit,8,9,gen));assert(s.state()==HBM::State::Idle);
 setup(s);assert(!s.tick(3)&&s.state()==HBM::State::Failed);
 assert(s.start(10));assert(!s.tick(7010)&&!strcmp(s.error(),"handshake timeout"));
 setup(s);credit[3]=1;assert(!s.bus(credit,8,6,s.generation()));
 setup(s);a[0]=6;assert(s.input(a,128,6,s.generation())==-1&&s.state()==HBM::State::Failed);
 setup(s);const uint8_t stop[]={3,0,0,0};assert(!s.bus(stop,4,6,s.generation()));
 setup(s);a[0]=5;assert(s.input(a,128,6,s.generation())==0);assert(!s.tick(1006)&&!strcmp(s.error(),"client receive timeout"));
 // Every truncation fails closed without out-of-bounds accesses.
 for(unsigned n=0;n<36;n++){HBM q;assert(q.start(0));uint8_t bytes[36]={0x81,1,0,1};assert(!q.bus(bytes,n,1,q.generation())||n==4);}
 puts("PASS HBM 1.0 start/enumerate/GUID properties/connect, real 136-byte fragment assembly, one credit per consumed message, TX credit exhaustion, malformed packets, reset generations and monotonic deadlines (ASan/UBSan).");
}
