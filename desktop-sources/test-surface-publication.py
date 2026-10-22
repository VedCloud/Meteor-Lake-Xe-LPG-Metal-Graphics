from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/plugin/NVMTLObjects.m').read_text()
def body(needle):
 a=s.index(needle);b=s.index('{',a);j=b+1;d=1
 while d:d+=(s[j]=='{')-(s[j]=='}');j+=1
 return s[a:j]+'\n'
pre=r'''#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#include <stdatomic.h>
#include <assert.h>
#include <time.h>
#include "nvmtl_vk.h"
#include "nvmtl_surface_publication.h"
static unsigned char pixels[24];static int locked,lock_fail,gpu_fail,reads,published,avoid_fail,lock_calls;static IOSurfaceLockOptions held_options;
static IOReturn lockSurface(IOSurfaceRef s,IOSurfaceLockOptions o,uint32_t *seed){lock_calls++;if(lock_fail)return kIOReturnError;if(avoid_fail&&(o&kIOSurfaceLockAvoidSync))return kIOReturnCannotLock;assert(!locked);held_options=o;locked=1;return 0;}
static IOReturn unlockSurface(IOSurfaceRef s,IOSurfaceLockOptions o,uint32_t *seed){assert(locked&&o==held_options);locked=0;if(seed)*seed=7;return 0;}
#define IOSurfaceLock lockSurface
#define IOSurfaceUnlock unlockSurface
#define IOSurfaceGetBaseAddressOfPlane(s,p) pixels
#define IOSurfaceGetBytesPerRowOfPlane(s,p) 12ul
#define IOSurfaceGetSeed(s) 7u
#define IOSurfaceGetID(s) ((uint32_t)(uintptr_t)(s))
#define IOSurfaceSetValue(s,k,n) do{assert(CFNumberGetTypeID()==CFGetTypeID(n));published++;}while(0)
static void nvlog(const char *f,...){}
static int nvmtl_noseed(){return 0;}
int nvmtl_vk_windowserver_serial(){return 1;}
int nvmtl_vk_surface_dirty(uint32_t s,uint32_t p){return -1;}
int nvmtl_vk_image_read(nvk_queue *q,nvk_image *i,void *d,size_t row){reads++;if(row==8)assert(!locked);else assert(locked);if(gpu_fail)return -2;for(size_t y=0;y<2;y++)for(size_t x=0;x<8;x++)((unsigned char *)d)[y*row+x]=(unsigned char)(1+y*8+x);return 0;}
static uint32_t nvmtl_vram_tag_bump(IOSurfaceRef s,NSUInteger p,uint32_t seed){return 1;}
static uint32_t nvmtl_vram_gen(IOSurfaceRef s,NSUInteger p){return 1;}
static void nvmtl_vram_tag_set(IOSurfaceRef s,NSUInteger p,uint32_t seed){}
@interface NVMTLTexture:NSObject{@public NVMTLTexture *_parent;IOSurfaceRef _surf;BOOL _hostOn,_vramOn,_surfSynced;nvk_image _i;nvk_queue *_q;NSUInteger _plane;uint32_t _surfSeed,_surfGen;}
-(BOOL)nvmtlSurfaceOut;-(BOOL)nvmtlSurfaceCopyable;-(NSUInteger)pixelFormat;
@end
static id nvmtl_surface_sync(NVMTLTexture *t){return t;}
@implementation NVMTLTexture
-(BOOL)nvmtlSurfaceCopyable{return YES;}-(NSUInteger)pixelFormat{return 80;}
'''
post=r'''@end
@interface NVMTLSubmissionPart:NSObject{@public NSSet *surfaces;}@end
@implementation NVMTLSubmissionPart @end
@interface NVMTLCommandBuffer:NSObject{@public long long _pubN;NSUInteger _nextPart;NSArray *_parts;}@end
@implementation NVMTLCommandBuffer @end
'''
main=r'''int main(){@autoreleasepool{
NVMTLTexture *t=[NVMTLTexture new];t->_surf=(IOSurfaceRef)(uintptr_t)99;t->_i.w=2;t->_i.h=2;t->_i.bpp=4;
memset(pixels,0xa5,sizeof pixels);assert([t nvmtlSurfaceOut]&&!locked&&reads==1&&pixels[0]==1&&pixels[12]==9&&pixels[8]==0xa5&&pixels[20]==0xa5);
assert(held_options==kIOSurfaceLockAvoidSync&&lock_calls==1);
avoid_fail=1;lock_calls=0;assert([t nvmtlSurfaceOut]&&!locked&&lock_calls==2&&held_options==0);avoid_fail=0;
memset(pixels,0xa5,sizeof pixels);gpu_fail=1;assert(![t nvmtlSurfaceOut]&&!locked);for(int i=0;i<24;i++)assert(pixels[i]==0xa5);gpu_fail=0;
lock_fail=1;assert(![t nvmtlSurfaceOut]&&!locked);lock_fail=0;
void *p[3];int leases[3];for(int i=0;i<3;i++){leases[i]=nvmtl_publication_acquire(16,&p[i]);assert(leases[i]>=0);}
assert([t nvmtlSurfaceOut]&&!locked);for(int i=0;i<3;i++)nvmtl_publication_release(leases[i]);
NVMTLCommandBuffer *cb=[NVMTLCommandBuffer new];cb->_pubN=12;
NVMTLSubmissionPart *a=[NVMTLSubmissionPart new],*b=[NVMTLSubmissionPart new];a->surfaces=[NSSet setWithObject:t];b->surfaces=[NSSet setWithObject:t];cb->_parts=@[a,b];
nvmtl_mark_published(cb,t);assert(published==0);cb->_nextPart=1;nvmtl_mark_published(cb,t);assert(published==1);
cb->_pubN=0;nvmtl_mark_published(cb,t);assert(published==1);
puts("PASS actual surface-out: GPU readback before IOSurface lock, AvoidSync success and CannotLock fallback with matching unlock options, padded copy/seed, GPU/lock failures leave output unchanged and release leases; saturated staging uses existing synchronous path. Actual marker waits for final repeated-plane part.");
}}
'''
f=r/'desktop-build/test-surface-publication.m';f.write_text(pre+body('- (BOOL)nvmtlSurfaceOut {')+post+body('void nvmtl_mark_published(')+main)
exe=r/'desktop-build/test-surface-publication';subprocess.run(['xcrun','clang','-arch','x86_64','-g','-fobjc-arc','-fsanitize=address,undefined','-I',str(r/'NullMoth/plugin'),str(f),'-framework','Foundation','-framework','IOSurface','-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
