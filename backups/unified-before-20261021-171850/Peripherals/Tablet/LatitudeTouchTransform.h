#pragma once
#include <stdint.h>
// Normalize each physical axis BEFORE rotation. Coordinates have a top-left
// origin; quarterTurns is clockwise in that coordinate system, not a CG API flag.
namespace LatitudeTablet {
struct Point {uint32_t x,y;};
inline bool touchTransform(int32_t x,int32_t y,int32_t minX,int32_t maxX,int32_t minY,int32_t maxY,unsigned quarterTurns,Point *out){
 if(!out || maxX<=minX || maxY<=minY || x<minX || x>maxX || y<minY || y>maxY || quarterTurns>3)return false;
 uint32_t nx=(uint64_t)((int64_t)x-minX)*65535/((int64_t)maxX-minX);
 uint32_t ny=(uint64_t)((int64_t)y-minY)*65535/((int64_t)maxY-minY);
 switch(quarterTurns){
  case 0:*out={nx,ny};break;
  case 1:*out={65535-ny,nx};break;
  case 2:*out={65535-nx,65535-ny};break;
  case 3:*out={ny,65535-nx};break;
 }
 return true;
}
}
