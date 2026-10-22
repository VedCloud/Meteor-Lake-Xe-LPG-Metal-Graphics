#pragma once
#include <stdint.h>
#include <stddef.h>
static inline uint16_t latitudeWmiWord(const uint8_t *p){return (uint16_t)((unsigned)p[0]|((unsigned)p[1]<<8));}
static inline int latitudeWmiDirection(uint16_t type,uint16_t code){
 if(type==0){if(code==0xe005)return -1;if(code==0xe006)return 1;}
 if(type==0x10){if(code==0x57)return -1;if(code==0x58)return 1;}
 return 0;
}
struct LatitudeWmiRecord{size_t next;uint16_t type,code;int direction;};
static inline int latitudeWmiRecord(const uint8_t *p,size_t bytes,size_t offset,LatitudeWmiRecord *r){
 if(!p||!r||bytes>128||(bytes&1)||offset>bytes||bytes-offset<2)return -1;
 size_t words=latitudeWmiWord(p+offset);if(!words)return 0;
 size_t length=(words+1)*2;if(length<6||length>bytes-offset)return -1;
 r->next=offset+length;r->type=latitudeWmiWord(p+offset+2);r->code=latitudeWmiWord(p+offset+4);
 r->direction=latitudeWmiDirection(r->type,r->code);return 1;
}
static inline bool latitudeWmiGuid(const uint8_t *p,size_t bytes){
 static const uint8_t guid[16]={0x94,0x59,0xbb,0x9d,0x97,0xa9,0xda,0x11,0xb0,0x12,0xb6,0x22,0xa1,0xef,0x54,0x92};
 if(!p||bytes%20||bytes>4096)return false;
 for(size_t i=0;i<bytes;i+=20){unsigned j=0;while(j<16&&p[i+j]==guid[j])j++;if(j==16&&p[i+16]==0xd0&&p[i+18]&&(p[i+19]&8))return true;}
 return false;
}
