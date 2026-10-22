#include "latitude_ish_pio.h"
#include <cassert>
#include <vector>
#include <utility>
#include <cstdio>
struct MMIO {uint32_t r[128]={};std::vector<std::pair<unsigned,uint32_t>> writes;
 uint32_t read32(unsigned at){assert(at<0x200&&!(at&3));return r[at/4];}
 void write32(unsigned at,uint32_t v){assert(at<0x200&&!(at&3));r[at/4]=v;writes.push_back({at,v});}void flush(){}
 void inject(unsigned protocol,unsigned command,const std::vector<uint8_t>&p){r[0x48/4]=0;for(unsigned i=0;i<128;i+=4)r[(0x60+i)/4]=0;for(unsigned i=0;i<p.size();i++)r[(0x60+i)/4]|=(uint32_t)p[i]<<(8*(i%4));r[0x54/4]=0x80000000u|(protocol<<10)|(command<<16)|(unsigned)p.size();}
};
static std::vector<uint8_t> bus(const std::vector<uint8_t>&p){std::vector<uint8_t>w={0,0,(uint8_t)p.size(),0x80};w.insert(w.end(),p.begin(),p.end());return w;}
int main(){using Link=LatitudeISH::PIO<MMIO>;MMIO io;io.r[0x34/4]=3;io.r[0x38/4]=0x20;Link l(io);assert(l.start(0));assert(io.r[0x38/4]==0xa0);assert((io.r[0x48/4]&0xffff)==(3u<<10|4));
 io.inject(3,4,{1,0,0,0});assert(l.interrupt(1)==0);assert(l.state()==Link::Phase::HBM);assert(io.r[0xe0/4]==0x80040000);
 io.inject(1,0,bus({0x81,1,0,1}));assert(l.interrupt(2)==0);
 std::vector<uint8_t>enumerate(36);enumerate[0]=0x84;enumerate[4]=1<<5;io.inject(1,0,bus(enumerate));assert(l.interrupt(3)==0);
 std::vector<uint8_t>props={0x85,5,0,0,0x58,0xcd,0xae,0x33,0x79,0xb6,0x54,0x4e,0x9b,0xd9,0xa0,0x4d,0x34,0xf0,0xc2,0x26,1,1,0,1,0,0x10,0,0,0,0,0,0};io.inject(1,0,bus(props));assert(l.interrupt(4)==0);
 io.inject(1,0,bus({0x86,5,1,0}));assert(l.interrupt(5)==0);assert(l.state()==Link::Phase::Connected);
 io.inject(1,0,bus({8,5,1,0,0,0,0,0}));assert(l.interrupt(6)==0);const uint8_t enumerateHID[6]={33,0,0,0,0,0};assert(l.sendClient(enumerateHID,6,6));assert(io.r[0xe0/4]==0x80060105&&io.r[0xe4/4]==33);
 std::vector<uint8_t>a(128);a[0]=5;a[1]=1;a[2]=124;a[4]=3;io.inject(1,0,a);assert(l.interrupt(7)==0);
 std::vector<uint8_t>b(16);b[0]=5;b[1]=1;b[2]=12;b[3]=0x80;io.inject(1,0,b);assert(l.interrupt(8)==1&&l.size()==136&&l.data()[0]==3);assert(l.consumed());
 auto writes=io.writes.size();l.stop();assert(io.r[0x38/4]==0x20&&io.writes.size()==writes+1);for(auto w:io.writes)assert(w.first!=0x368);
 MMIO dma;dma.r[0x34/4]=0x10003;Link forbidden(dma);assert(!forbidden.start(0)&&dma.writes.empty());
 MMIO bios;bios.r[0x38/4]=0xa0;Link biosLink(bios);assert(biosLink.start(0));biosLink.stop();assert(bios.r[0x38/4]==0x20);
 MMIO dead;Link timeout(dead);assert(timeout.start(0));assert(!timeout.tick(10000));timeout.stop();
 puts("PASS real PIO register queue connected to HBM: reset, GUID enumeration, credits, client enumeration, fragmented payload delivery/ack, host-bit ownership cleanup, DMA refusal and timeouts under ASan/UBSan. MMIO simulated.");
}
