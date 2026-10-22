/* SPDX-License-Identifier: MIT
 * SHA-256 implements the public FIPS 180-4 algorithm. No allocation or host APIs. */
#include "boot_input.h"
static uint32_t ror(uint32_t x, unsigned n) { return (x >> n) | (x << (32-n)); }
static const uint32_t k[64] = {
0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
static void transform(D27Sha256 *s) {
 uint32_t w[64];
 for(unsigned i=0;i<16;i++) w[i]=((uint32_t)s->block[4*i]<<24)|((uint32_t)s->block[4*i+1]<<16)|((uint32_t)s->block[4*i+2]<<8)|s->block[4*i+3];
 for(unsigned i=16;i<64;i++) {
  uint32_t x=w[i-15],y=w[i-2];
  w[i]=w[i-16]+(ror(x,7)^ror(x,18)^(x>>3))+w[i-7]+(ror(y,17)^ror(y,19)^(y>>10));
 }
 uint32_t a=s->state[0],b=s->state[1],c=s->state[2],d=s->state[3],e=s->state[4],f=s->state[5],g=s->state[6],h=s->state[7];
 for(unsigned i=0;i<64;i++) {
  uint32_t t=h+(ror(e,6)^ror(e,11)^ror(e,25))+((e&f)^((~e)&g))+k[i]+w[i];
  uint32_t u=(ror(a,2)^ror(a,13)^ror(a,22))+((a&b)^(a&c)^(b&c));
  h=g;g=f;f=e;e=d+t;d=c;c=b;b=a;a=t+u;
 }
 s->state[0]+=a;s->state[1]+=b;s->state[2]+=c;s->state[3]+=d;
 s->state[4]+=e;s->state[5]+=f;s->state[6]+=g;s->state[7]+=h;
}
void d27_sha_init(D27Sha256 *s) {
 static const uint32_t initial[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
 for(unsigned i=0;i<8;i++)s->state[i]=initial[i];s->bytes=0;s->used=0;
}
void d27_sha_update(D27Sha256 *s,const void *data,size_t n) {
 const uint8_t *p=data;s->bytes+=n;
 while(n) {size_t take=64-s->used;if(take>n)take=n;
  for(size_t i=0;i<take;i++)s->block[s->used+i]=p[i];
  s->used+=take;p+=take;n-=take;if(s->used==64){transform(s);s->used=0;}
 }
}
void d27_sha_final(D27Sha256 *s,uint8_t out[32]) {
 uint64_t bits=s->bytes*8;s->block[s->used++]=0x80;
 if(s->used>56){while(s->used<64)s->block[s->used++]=0;transform(s);s->used=0;}
 while(s->used<56)s->block[s->used++]=0;
 for(unsigned i=0;i<8;i++)s->block[56+i]=(uint8_t)(bits>>(56-8*i));
 transform(s);for(unsigned i=0;i<32;i++)out[i]=(uint8_t)(s->state[i/4]>>(24-8*(i%4)));
}
static uint32_t le32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint64_t le64(const uint8_t *p){return le32(p)|((uint64_t)le32(p+4)<<32);}
int d27_kernel_header(const uint8_t *p,size_t n,uint64_t file) {
 if(n<32||file<n||le32(p)!=0xfeedfacf||le32(p+4)!=0x0100000c||le32(p+12)!=12)return 1;
 uint32_t count=le32(p+16),bytes=le32(p+20);
 if(!count||count>32768||bytes>1024*1024||bytes>n-32||count>bytes/8)return 2;
 size_t end=32+(size_t)bytes,at=32;unsigned entries=0;
 for(uint32_t i=0;i<count;i++){
  if(end-at<8)return 3;uint32_t kind=le32(p+at),size=le32(p+at+4);
  if(size<8||(size&7)||size>end-at)return 4;
  if(kind==0x19){
   if(size<72)return 5;uint64_t off=le64(p+at+40),length=le64(p+at+48),va=le64(p+at+24),vm=le64(p+at+32);
   if(off>file||length>file-off||vm>UINT64_MAX-va)return 6;
   uint32_t sections=le32(p+at+64);if(sections>(size-72)/80)return 7;
  }
  if(kind==0x80000035){
   if(size<32||le64(p+at+16)>=file)return 8;
   uint32_t id=le32(p+at+24);if(id<32||id>=size)return 9;
   size_t j=id;while(j<size&&p[at+j])j++;if(j==size)return 10;entries++;
  }
  at+=size;
 }
 return at!=end||!entries?11:0;
}
