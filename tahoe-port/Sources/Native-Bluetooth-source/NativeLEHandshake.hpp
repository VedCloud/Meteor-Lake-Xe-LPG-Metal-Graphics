// Bounded per-link compatibility state for Intel 8087:0033 on Tahoe.
// Caller serializes access. Unsolicited, failed or malformed events pass unchanged.
#ifndef NATIVE_LE_HANDSHAKE_HPP
#define NATIVE_LE_HANDSHAKE_HPP
struct NativeLEHandshake {
 struct Link {unsigned handle=0,stage=0;unsigned long long began=0;bool used=false;};
 Link links[16];
 void reset(){for(auto& s:links)s=Link();}
 void cancel(unsigned h){for(auto& s:links)if(s.used&&s.handle==h)s=Link();}
 bool pending(unsigned long long now){bool result=false;for(auto& s:links){if(s.used&&now-s.began>5000000000ULL)s=Link();result|=s.used;}return result;}
 bool begin(unsigned h,unsigned long long now){
  if(h>0x0eff)return false;
  pending(now);
  for(auto& s:links)if(s.used&&s.handle==h)return false;
  for(auto& s:links)if(!s.used){s.used=true;s.handle=h;s.began=now;s.stage=0;return true;}
  return false;
 }
 unsigned event(unsigned char* p,unsigned n,unsigned cap,unsigned long long now){
  if(!p||n>cap||n<2||unsigned(p[1])+2!=n)return n;
  pending(now);
  if(p[0]==5&&n==6&&p[2]==0){cancel(unsigned(p[3])|(unsigned(p[4])<<8));return n;}
  if(p[0]!=0x3e||n!=14||p[2]!=4)return n;
  unsigned h=unsigned(p[4])|(unsigned(p[5])<<8);
  for(auto& s:links)if(s.used&&s.handle==h){
   if(p[3]!=0){s=Link();return n;}
   if(s.stage==0){s.stage=1;return n;} // Preserve the real remote features reply.
   if(s.stage==1){
    unsigned char phy[8]={0x3e,6,0x0c,0,(unsigned char)h,(unsigned char)(h>>8),2,2};
    if(cap<sizeof(phy))return n;
    for(unsigned i=0;i<sizeof(phy);i++)p[i]=phy[i];s.stage=2;return sizeof(phy);
   }
   // LE Data Length Change has no status octet. The handle starts at byte 3.
   unsigned char length[13]={0x3e,11,7,(unsigned char)h,(unsigned char)(h>>8),27,0,0x48,1,27,0,0x48,1};
   if(cap<sizeof(length))return n;
   for(unsigned i=0;i<sizeof(length);i++)p[i]=length[i];s=Link();return sizeof(length);
  }
  return n;
 }
};
#endif
