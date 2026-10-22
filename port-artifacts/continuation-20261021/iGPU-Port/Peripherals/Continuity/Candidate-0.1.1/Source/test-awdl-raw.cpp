#include "LatitudeAWDLFrame.h"
#include <vector>
#include <cassert>
#include <cstdio>
int main(){
 const uint8_t source[]={2,1,2,3,4,5};
 std::vector<uint8_t>packet(12+43,0);packet[2]=12;packet[4]=8;packet[8]=0x6c;packet[9]=9;packet[10]=0x80;packet[11]=0;
 auto p=packet.data()+12;p[0]=0xd0;for(unsigned i=0;i<6;i++){p[4+i]=0xff;p[10+i]=source[i];}
 const uint8_t bssid[]={0,0x25,0,0xff,0x94,0x73};for(unsigned i=0;i<6;i++)p[16+i]=bssid[i];
 p[24]=0x7f;p[26]=0x17;p[27]=0xf2;p[28]=8;p[29]=0x10;p[40]=21;
 LatitudeAWDL::Frame out;assert(LatitudeAWDL::radiotap(packet.data(),packet.size(),2412,source,out)&&out.offset==12&&out.length==43);
 for(size_t n=0;n<packet.size();n++)if(n!=52)assert(!LatitudeAWDL::radiotap(packet.data(),n,2412,source,out));
 assert(!LatitudeAWDL::radiotap(packet.data(),packet.size(),2437,source,out));
 for(size_t index: {size_t(0),size_t(4),size_t(12),size_t(13),size_t(22),size_t(28),size_t(36),size_t(40),size_t(53)}){
  auto changed=packet;changed[index]^=0x80;assert(!LatitudeAWDL::radiotap(changed.data(),changed.size(),2412,source,out));
 }
 for(size_t n=0;n<4097;n++){std::vector<uint8_t>fuzz(n,0xff);assert(!LatitudeAWDL::radiotap(fuzz.data(),fuzz.size(),2412,source,out));}
 puts("PASS bounded radiotap/action validation, source/BSSID/channel binding, truncated TLVs and malformed lengths under ASan/UBSan. No hardware transmission performed.");
}
