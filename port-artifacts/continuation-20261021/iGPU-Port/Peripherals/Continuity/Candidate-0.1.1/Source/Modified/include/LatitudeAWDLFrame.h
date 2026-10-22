#pragma once
#include <stdint.h>
#include <stddef.h>
// Deliberately narrow raw-action transport bring-up. A channel-changing AWDL
// scheduler and native peer-data path are NOT provided by this parser.
namespace LatitudeAWDL {
struct Frame {size_t offset=0,length=0;uint16_t frequency=0;};
inline uint16_t le16(const uint8_t*p){return uint16_t(p[0])|uint16_t(p[1])<<8;}
inline uint32_t le32(const uint8_t*p){return uint32_t(le16(p))|uint32_t(le16(p+2))<<16;}
inline bool action(const uint8_t*p,size_t n,const uint8_t*source){
 if(!p||!source||n<40||n>2304||p[0]!=0xd0||p[1]!=0)return false;
 const uint8_t bssid[]={0,0x25,0,0xff,0x94,0x73};
 for(unsigned i=0;i<6;i++)if(p[10+i]!=source[i]||p[16+i]!=bssid[i]||p[4+i]!=0xff)return false;
 if(p[24]!=0x7f||p[25]!=0||p[26]!=0x17||p[27]!=0xf2||p[28]!=8||p[31]||(p[30]!=0&&p[30]!=3))return false;
 size_t at=40;unsigned fields=0;
 while(at<n){if(n-at<3||++fields>256)return false;size_t length=le16(p+at+1);at+=3;if(length>n-at)return false;at+=length;}
 return at==n;
}
inline bool radiotap(const uint8_t*p,size_t n,uint16_t currentFrequency,const uint8_t*source,Frame&out){
 out=Frame{};if(!p||n<8||n>4096||p[0]||p[1])return false;
 size_t header=le16(p+2);uint32_t present=le32(p+4);
 // Extended bitmaps and unimplemented fields are refused, never guessed.
 if(header<8||header>256||header>n||(present&~0xfu)||!(present&8u))return false;
 size_t at=8;uint8_t flags=0;
 if(present&1){at=(at+7)&~size_t(7);if(at+8>header)return false;at+=8;}
 if(present&2){if(at+1>header)return false;flags=p[at++];}
 if(present&4){if(at+1>header)return false;at++;}
 at=(at+1)&~size_t(1);if(at+4>header||flags&0x40)return false;
 uint16_t frequency=le16(p+at);
 // Regulatory permission comes from the driver's channel table, not from
 // caller-supplied radiotap flags.
 if(!frequency||frequency!=currentFrequency||!((frequency>=2412&&frequency<=2484)||(frequency>=5000&&frequency<=5900)))return false;
 size_t bytes=n-header;if(flags&0x10){if(bytes<4)return false;bytes-=4;}
 if(!action(p+header,bytes,source))return false;
 out={header,bytes,frequency};return true;
}
}
