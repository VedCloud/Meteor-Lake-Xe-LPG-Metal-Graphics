#define main d27_guest_main
#include "dell27-display-bridge.m"
#undef main
#include <assert.h>
int main(void){@autoreleasepool{
 unsigned cases=0;
 for(unsigned w=1;w<=129;w+=16)for(unsigned h=1;h<=17;h+=4){
  NSDictionary*properties=@{(__bridge NSString*)kIOSurfaceWidth:@(w),(__bridge NSString*)kIOSurfaceHeight:@(h),(__bridge NSString*)kIOSurfaceBytesPerElement:@4,(__bridge NSString*)kIOSurfacePixelFormat:@(kCVPixelFormatType_32BGRA)};
  IOSurfaceRef surface=IOSurfaceCreate((__bridge CFDictionaryRef)properties);assert(surface);
  width=w;height=h;stride=w*4+32;videoOffset=64;videoBytes=videoOffset+height*stride+64;uint8_t*memory=malloc(videoBytes);memset(memory,0x5a,videoBytes);video=(uintptr_t)memory;
  assert(!IOSurfaceLock(surface,0,NULL));uint8_t*src=IOSurfaceGetBaseAddress(surface);size_t sourceRow=IOSurfaceGetBytesPerRow(surface);for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w*4;x++)src[y*sourceRow+x]=(uint8_t)(x+y*3);assert(!IOSurfaceUnlock(surface,0,NULL));
  assert(draw(surface));for(unsigned y=0;y<h;y++){assert(!memcmp(memory+videoOffset+y*stride,src+y*sourceRow,w*4));for(uint64_t i=w*4;i<stride;i++)assert(memory[videoOffset+y*stride+i]==0x5a);}for(unsigned i=0;i<64;i++){assert(memory[i]==0x5a);assert(memory[videoBytes-1-i]==0x5a);}cases++;
  width++;assert(!draw(surface));width--;height++;assert(!draw(surface));height--;assert(!draw(NULL));free(memory);CFRelease(surface);
 }
 printf("PASS %u native IOSurface copies with independent padded strides, guards, and invalid geometry rejection; no display created or opened\n",cases);
}return 0;}
