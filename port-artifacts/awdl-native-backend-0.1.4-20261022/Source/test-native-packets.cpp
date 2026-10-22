#include "LatitudeAWDLNativePacket.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main(){
 using namespace LatitudeAWDLPeer;using namespace LatitudeAWDLNative;
 uint8_t local[]={2,0,0,0,0,1},remote[]={2,0,0,0,0,2};
 uint8_t ether[54]={},air[80]={},prepared[70]={},again[80]={};
 copy(ether,remote,6);copy(ether+6,local,6);ether[12]=0x86;ether[13]=0xdd;ether[14]=0x60;
 DataPath path;assert(path.configure(local)&&path.observe(remote,1));
 assert(path.encode(ether,54,air,80,2)==80);
 copy(prepared,ether,12);prepared[13]=56;copy(prepared+14,air+24,56);
 assert(airFrame(prepared,70,local,0,again,80)==80&&!memcmp(air,again,80));
 for(size_t n=0;n<70;++n)assert(!airFrame(prepared,n,local,0,again,80));
 assert(!airFrame(prepared,70,local,0,again,79));
 // Native sequence bytes survive; only the 802.11 sequence is generated here.
 put16(prepared+24,0x1234);assert(airFrame(prepared,70,local,9,again,80)==80);
 assert(le16(again+34)==0x1234&&le16(again+22)==9*16);
 auto save=prepared[13];prepared[13]^=1;assert(!airFrame(prepared,70,local,0,again,80));prepared[13]=save;
 prepared[12]=0x86;prepared[13]=0xdd;assert(!airFrame(prepared,70,local,0,again,80)); // plain Ethernet isn't prepared
 Originals<int*> originals;int payloads[MaxOutstanding+1]={};int status=99;uint64_t credit=0;
 assert(!originals.insert(0,&payloads[0])&&!originals.insert(1,nullptr));
 for(unsigned i=0;i<MaxOutstanding;++i)assert(originals.insert(i+1,&payloads[i],i+100));
 assert(!originals.insert(1,&payloads[1])&&!originals.insert(100,&payloads[MaxOutstanding]));
 assert(originals.response(1,0)&&!originals.response(1,1)&&originals.count==MaxOutstanding);
 assert(originals.reclaim(1,5,status,&credit)==&payloads[0]&&status==0&&credit==100);
 assert(!originals.reclaim(1,5,status));assert(originals.reclaim(2,5,status)==&payloads[1]&&status==5);
 unsigned completed=0;originals.abortAfterDMAQuiesced(7,[&](int*p,int s){assert(p&&s==7);++completed;});
 assert(completed==MaxOutstanding-2&&originals.count==0);
 originals.abortAfterDMAQuiesced(7,[&](int*,int){assert(false);});
 puts("PASS native prepared-frame adapter and original-packet ownership: no double encapsulation, preserved sequence, truncation, capacity, response/reclaim separation, duplicate completion and reset cleanup.");
}
