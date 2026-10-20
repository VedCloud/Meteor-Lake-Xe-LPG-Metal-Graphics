from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
source=(r/'drm-kmod/drivers/gpu/drm/i915/gt/nmx_request_capture.h').read_text()
pre=r'''
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef uint64_t u64;typedef uint32_t u32;typedef unsigned atomic_t;
#define NMXKPI 1
#define NMX_REQUEST_CAPTURE_IMPLEMENTATION 1
#define smp_store_release(p,v) __atomic_store_n(p,v,__ATOMIC_RELEASE)
#define smp_load_acquire(p) __atomic_load_n(p,__ATOMIC_ACQUIRE)
#define ATOMIC_INIT(x) x
#define atomic_inc_return(p) (++*(p))
#define atomic_read(p) (*(p))
#define READ_ONCE(x) (x)
#define INTEL_DEVID(p) (*(p))
#define RENDER_CLASS 0
struct intel_ring {void *vaddr;atomic_t pin_count;u32 size;};
struct ctx {struct {unsigned id;} guc_id;};struct eng {unsigned class;};
struct i915_request {unsigned *i915;struct eng *engine;struct intel_ring *ring;struct ctx *context;struct {unsigned long long seqno,context;} fence;u32 head,infix,postfix,tail,wa_tail;int active;};
#define i915_request_is_active(rq) ((rq)->active)
static unsigned headers,reads;static u32 offsets[140],values[140];
static void pr_err(const char *fmt,...){va_list ap;va_start(ap,fmt);const char *phase=va_arg(ap,const char *);(void)phase;va_arg(ap,unsigned long long);va_arg(ap,unsigned long long);if(strstr(fmt,"ring-word:")){offsets[reads]=va_arg(ap,unsigned);values[reads++]=va_arg(ap,unsigned);}else headers++;va_end(ap);}
'''
post=r'''
int main(void){
 unsigned device=0x7d55;struct eng engine={0};struct ctx ctx={{4098}};u32 *data=malloc(4096);for(unsigned i=0;i<1024;i++)data[i]=0x50000000+i;
 struct intel_ring ring={data,1,4096};struct i915_request rq={.i915=&device,.engine=&engine,.ring=&ring,.context=&ctx,.fence={20,8},.head=0xca8,.infix=0xcc0,.postfix=0xd78,.tail=0xe08,.wa_tail=0xe10,.active=1};
 nmx_log_request_ring(&rq,"before-tail");assert(headers==1&&reads==94);assert(offsets[3]==0xca4&&values[3]==data[0xca4/4]);for(unsigned i=0;i<reads;i++)assert(values[i]==data[offsets[i]/4]);
 reads=0;rq.head=4092;rq.wa_tail=12;nmx_log_request_ring(&rq,"hang");assert(reads==8&&offsets[5]==0&&values[5]==data[0]);
 reads=0;rq.head=0;rq.wa_tail=2048;nmx_log_request_ring(&rq,"hang");assert(reads==132);
 reads=0;unsigned before=headers;rq.head=1;nmx_log_request_ring(&rq,"invalid");assert(headers==before);rq.head=0;
 ring.size=4095;nmx_log_request_ring(&rq,"invalid");assert(headers==before);ring.size=4096;
 ring.vaddr=0;nmx_log_request_ring(&rq,"invalid");assert(headers==before);ring.vaddr=data;
 ring.pin_count=0;nmx_log_request_ring(&rq,"invalid");assert(headers==before);ring.pin_count=1;
 device=0x1234;nmx_log_request_ring(&rq,"other");assert(headers==before);device=0x7d55;
 rq.active=0;nmx_log_request_ring(&rq,"inactive");assert(headers==before);rq.active=1;
 rq.fence.seqno=33;nmx_log_request_ring(&rq,"late");assert(headers==before);rq.fence.seqno=20;

 headers=reads=0;nmx_capture_request_ring(&rq);assert(headers==0&&reads==0);
 assert(nmx_ring_snapshots[0].ready==1);rq.fence.context=9;nmx_dump_saved_request_ring(&rq);assert(reads==0);rq.fence.context=8;
 headers=reads=0;nmx_dump_saved_request_ring(&rq);assert(headers==1&&reads==132);
 headers=reads=0;nmx_ring_snapshot_count=96;nmx_capture_request_ring(&rq);assert(headers==0&&nmx_ring_snapshot_count==96);

 free(data);puts("PASS actual ring capture ASan/UBSan: observed boundary, wrap, truncation, invalid/unmapped/unpinned/inactive ring, device/sequence scope, snapshot budget");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.c').write_text(pre+source+post);subprocess.run(['xcrun','clang','-Wall','-Wextra','-Werror','-g','-fsanitize=address,undefined',str(p/'test.c'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
