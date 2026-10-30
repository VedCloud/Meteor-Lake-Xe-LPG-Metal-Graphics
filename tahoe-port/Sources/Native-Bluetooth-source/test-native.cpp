
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include "NativeLEHandshake.hpp"
static void remote(unsigned char* p,unsigned h,unsigned status=0){memset(p,0,64);p[0]=0x3e;p[1]=12;p[2]=4;p[3]=status;p[4]=h;p[5]=h>>8;}
int main(){NativeLEHandshake c;unsigned char p[64],original[64];
 remote(p,1);memcpy(original,p,64);assert(c.event(p,14,64,1)==14&&!memcmp(p,original,64));
 assert(c.begin(1,1));assert(!c.begin(1,2));assert(!c.begin(0x0fff,2));assert(c.begin(0x124,1));
 remote(p,1);assert(c.event(p,14,64,2)==14&&p[2]==4);
 remote(p,0x124);assert(c.event(p,14,64,2)==14&&p[2]==4);
 remote(p,1);assert(c.event(p,14,64,3)==8&&p[2]==12&&p[4]==1&&p[5]==0);
 remote(p,0x124);assert(c.event(p,14,64,3)==8&&p[2]==12&&p[4]==0x24&&p[5]==1);
 remote(p,1);assert(c.event(p,14,64,4)==13&&p[2]==7&&p[3]==1&&p[4]==0&&p[5]==27&&p[7]==0x48);
 remote(p,0x124);assert(c.event(p,14,64,4)==13&&p[2]==7&&p[3]==0x24&&p[4]==1);
 assert(!c.pending(5));
 assert(c.begin(2,10));remote(p,2,1);assert(c.event(p,14,64,11)==14&&!c.pending(12));
 assert(c.begin(2,10));remote(p,2);assert(c.event(p,14,64,6000000011ULL)==14&&!c.pending(6000000012ULL));
 c.reset();assert(c.begin(2,1));for(unsigned n=0;n<14;n++){remote(p,2);assert(c.event(p,n,64,2)==n);}
 remote(p,2);assert(c.event(p,14,13,3)==14);assert(c.event(nullptr,14,64,3)==14);
 c.reset();for(unsigned h=0;h<16;h++)assert(c.begin(h,1));assert(!c.begin(16,1));c.cancel(7);assert(c.begin(16,2));
 c.reset();assert(c.begin(1,1));memset(p,0,64);p[0]=5;p[1]=4;p[3]=1;assert(c.event(p,6,64,2)==6&&!c.pending(3));
 unsigned seed=1;
 for(unsigned i=0;i<250000;i++){for(auto& x:p){seed=seed*1664525+1013904223;x=seed>>24;}unsigned n=(seed>>8)%65;memcpy(original,p,64);unsigned result=c.event(p,n,64,100);assert(result==n&&!memcmp(p,original,64));}
 puts("PASS native LE codec: original features retained, per-handle PHY/data-length, malformed/error/unrelated/disconnect/expiry cases, 250000 fuzz packets");
}
