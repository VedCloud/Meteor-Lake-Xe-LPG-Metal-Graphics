from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
new=(r/'drm-kmod/drivers/gpu/drm/i915/gem/i915_gem_stolen.c').read_text();old=(r/'backups/scanout-reservation-before-1.0.15/i915_gem_stolen.c').read_text()
vm=(r/'NullMoth/gpu-port/xkpi/xnu/nmx_vm.cpp').read_text()
def fn(s,n):
 a=s.index(n);a=s.rfind('\n',0,a)+1;b=s.index('{',a);j=b+1;depth=1
 while depth:depth+=(s[j]=='{')-(s[j]=='}');j+=1
 return s[a:j]+'\n'
pre=r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#define NMXKPI 1
#define U64_MAX UINT64_MAX
#define SZ_64K 65536ull
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#define max_t(t,a,b) ((t)(a)>(t)(b)?(t)(a):(t)(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define GEN6_DSMBASE 0
#define GEN11_BDSM_MASK 0xfffffffffff00000ull
#define INTEL_DEVID(i) ((i)->id)
#define INTEL_REVID(i) ((i)->rev)
#define GRAPHICS_VER(i) 12
#define IS_METEORLAKE(i) ((i)->id==0x7d55)
#define drm_notice(...) ((void)0)
#define drm_dbg(...) ((void)0)
#define drm_info(...) ((void)0)
#define drm_err(...) ((void)0)
#define mutex_init(p) ((void)0)
typedef uint64_t u64;
struct resource {u64 start,end;};
#define DEFINE_RES_MEM(s,n) ((struct resource){(s),(s)+(n)-1})
static u64 resource_size(struct resource *r){return r->end-r->start+1;}
struct mm {int stolen_lock;struct {u64 start,size;}stolen;};
struct drm_i915_private {int id,rev,drm;u64 uncore;struct mm mm;struct {struct resource stolen,reserved;u64 usable_size;}dsm;};
struct intel_memory_region{struct drm_i915_private *i915;struct resource region,io;u64 min_page_size;};
static int intel_vgpu_active(struct drm_i915_private *p){(void)p;return 0;}
static int i915_vtd_active(struct drm_i915_private *p){(void)p;return 0;}
static int adjust_stolen(struct drm_i915_private *i,struct resource *r){(void)i;(void)r;return 0;}
static int request_smem_stolen(struct drm_i915_private *i,struct resource *r){(void)i;(void)r;return 0;}
static int init_reserved_stolen(struct drm_i915_private *i){i->dsm.reserved.start=i->dsm.stolen.end+1;return 0;}
static u64 intel_uncore_read64(u64 *p,int reg){(void)reg;return *p;}
#define drm_mm_init(m,s,n) do{(m)->start=(s);(m)->size=(n);}while(0)
static u64 gop_base=0x60800000,gop_size=8294400;static int gop_error;
static int nmx_xnu_firmware_scanout(u64 *base,u64 *size){*base=gop_base;*size=gop_size;return gop_error;}
'''
post=r'''
int main(void){
 struct drm_i915_private i={.id=0x7d55,.rev=8,.uncore=0x60800000};
 struct intel_memory_region m={.i915=&i,.region={0x800000,0x87fffff},.io={0x60800000,0x687fffff},.min_page_size=65536};
 assert(i915_gem_init_stolen(&m)==0);
#ifdef OLD
 assert(i.mm.stolen.start==0);assert(4096<gop_size);
 puts("PASS negative control: original stolen allocator permits an allocation overlapping the retained firmware framebuffer");
#else
 assert(i.mm.stolen.start==0x7f0000 && i.mm.stolen.size==0x8000000-0x7f0000);
 assert(i.mm.stolen.start>=gop_size);
 u64 s;assert(latitude_stolen_start(100,1000,100,10,64,&s)==0&&s==64);
 assert(latitude_stolen_start(100,1000,356,10,64,&s)==0&&s==320);
 assert(latitude_stolen_start(0,1000,100,10,64,&s)<0);
 assert(latitude_stolen_start(100,1000,99,10,64,&s)<0);
 assert(latitude_stolen_start(100,1000,100,1001,64,&s)<0);
 assert(latitude_stolen_start(100,1000,1100,1,64,&s)<0);
 assert(latitude_stolen_start(100,1000,100,1,3,&s)<0);
 assert(latitude_stolen_start(1,U64_MAX,1,U64_MAX,64,&s)<0);
 for(u64 off=0;off<4096;off++)for(u64 n=1;n<129;n++){
  assert(latitude_stolen_start(0x60800000,0x8000000,0x60800000+off,n,65536,&s)==0);
  assert(s>=off+n && s%65536==0 && s<0x8000000);
 }
 i.id=0x1234;assert(i915_gem_init_stolen(&m)==0&&i.mm.stolen.start==0);
 i.id=0x7d55;gop_error=1;assert(i915_gem_init_stolen(&m)==-ENOSPC);
 gop_error=0;gop_base=0x70000000;assert(i915_gem_init_stolen(&m)==-ENOSPC);
 puts("PASS actual stolen initialization + bound helper ASan/UBSan: GOP exclusion, 524288 boundary cases, offset/alignment/overflow, invalid console refusal and other-device preservation");
#endif
}
'''
bridge_pre=r'''
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
struct PE_Video{uint64_t v_baseAddr,v_depth,v_width,v_height,v_rowBytes;};
static PE_Video video={0x60800000,32,1920,1080,7680};static int result;
#define kIOReturnSuccess 0
class IOPlatformExpert{public:int getConsoleInfo(PE_Video*v){*v=video;return result;}};
static IOPlatformExpert platform;static bool have=true;
class IOService{public:static IOPlatformExpert*getPlatform(){return have?&platform:nullptr;}};
'''
bridge_post=r'''
int main(){uint64_t b,s;assert(nmx_xnu_firmware_scanout(&b,&s)==0&&b==0x60800000&&s==8294400);
assert(nmx_xnu_firmware_scanout(nullptr,&s)==EINVAL);have=false;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL&&b==0&&s==0);have=true;
result=1;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);result=0;
video.v_depth=24;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);video.v_depth=32;
video.v_rowBytes=100;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);video.v_rowBytes=7680;
video.v_baseAddr++;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);video.v_baseAddr--;
video.v_height=16385;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);video.v_height=1080;
video.v_rowBytes=UINT64_MAX;assert(nmx_xnu_firmware_scanout(&b,&s)==EINVAL);
puts("PASS actual console bridge ASan/UBSan: valid scanout dimensions, absent/error console, null output, unaligned base, depth/pitch/bounds/overflow rejection");}
'''
with tempfile.TemporaryDirectory() as d:
 for tag,body,lang in [('old',pre+fn(old,'static int i915_gem_init_stolen(')+post,'c'),('new',pre+fn(new,'static int latitude_stolen_start(')+fn(new,'static int i915_gem_init_stolen(')+post,'c'),('bridge',bridge_pre+fn(vm,'extern "C" int nmx_xnu_firmware_scanout(')+bridge_post,'cpp')]:
  f=Path(d)/(tag+'.'+lang);f.write_text(body);exe=f.with_suffix('')
  subprocess.run(['xcrun','clang++' if lang=='cpp' else 'clang','-g','-O1','-fsanitize=address,undefined',*(['-DOLD'] if tag=='old' else []),str(f),'-o',str(exe)],check=True)
  subprocess.run([str(exe)],check=True)
