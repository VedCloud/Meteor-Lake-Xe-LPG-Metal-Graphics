from pathlib import Path
import subprocess, tempfile
r=Path(__file__).resolve().parent.parent
s=(r/'NullMoth/plugin/NVMTLObjects.m').read_text()
end=s.index('\n@end',s.index('                                        swizzle:(MTLTextureSwizzleChannels)sw {'))
start=s.rfind('- (id<MTLTexture>)newTextureViewWithPixelFormat:',0,s.index('                                        swizzle:(MTLTextureSwizzleChannels)sw {'))
method=s[start:end]
pre=r'''
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <assert.h>
static int refused,destroyed;static void *lastDestroyed;
static void nvlog(const char *fmt,...){}
int nvmtl_pixfmt_public(MTLPixelFormat f,uint32_t *vk,uint32_t *bpp,int *a8){*vk=37;*bpp=4;*a8=0;return 0;}
int nvmtl_vk_image_view_create_range_swz(void *i,uint32_t f,int a,uint32_t l,uint32_t n,uint32_t b,uint32_t c,uint32_t t,const uint8_t sw[4],void **out){if(refused)return -1;assert(sw[0]==MTLTextureSwizzleRed);*out=(void *)0x222;return 0;}
void nvmtl_vk_image_view_destroy(void *v){destroyed++;lastDestroyed=v;}
@interface NVMTLTexture:NSObject{@public void *_ownView,*_swzView;BOOL _stex,_hasSwz;MTLTextureSwizzleChannels _swz;uint32_t _baseLevel,_mips,_baseSlice,_viewSlices,_viewType;NVMTLTexture *_next;}
- (void *)nvi;
- (id<MTLTexture>)newTextureViewWithPixelFormat:(MTLPixelFormat)f textureType:(MTLTextureType)t levels:(NSRange)l slices:(NSRange)s;
- (id<MTLTexture>)newTextureViewWithPixelFormat:(MTLPixelFormat)f textureType:(MTLTextureType)t levels:(NSRange)l slices:(NSRange)s swizzle:(MTLTextureSwizzleChannels)sw;
@end
@implementation NVMTLTexture
- (void *)nvi{return (void *)0xabc;}
- (id<MTLTexture>)newTextureViewWithPixelFormat:(MTLPixelFormat)f textureType:(MTLTextureType)t levels:(NSRange)l slices:(NSRange)s{return (id)_next;}
'''
post=r'''
@end
int main(){@autoreleasepool{
NVMTLTexture *t=[NVMTLTexture new],*v=[NVMTLTexture new];t->_next=v;v->_ownView=(void *)0x111;v->_stex=YES;v->_swzView=(void *)0x333;
MTLTextureSwizzleChannels sw=MTLTextureSwizzleChannelsMake(MTLTextureSwizzleRed,MTLTextureSwizzleRed,MTLTextureSwizzleRed,MTLTextureSwizzleGreen);
assert([t newTextureViewWithPixelFormat:MTLPixelFormatRG8Unorm textureType:MTLTextureType2D levels:NSMakeRange(0,1) slices:NSMakeRange(0,1) swizzle:sw]==(id)v);
assert(v->_ownView==(void *)0x111&&v->_swzView==(void *)0x222&&v->_stex&&v->_hasSwz&&destroyed==1&&lastDestroyed==(void *)0x333);
refused=1;assert(![t newTextureViewWithPixelFormat:MTLPixelFormatRG8Unorm textureType:MTLTextureType2D levels:NSMakeRange(0,1) slices:NSMakeRange(0,1) swizzle:sw]);
assert(v->_ownView==(void *)0x111&&destroyed==1);
puts("PASS actual swizzled-view method: identity attachment/storage view preserved; sampled mapping separate; only prior sampled view freed; failed requested swizzle returns nil.");
}}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.m').write_text(pre+method+post)
 subprocess.run(['xcrun','clang','-fobjc-arc','-fsanitize=address,undefined','-g','-framework','Foundation','-framework','Metal',str(p/'test.m'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
