from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
s=(r/'mesa/src/intel/vulkan/anv_device.c').read_text();a=s.index('   ANV_FROM_HANDLE(anv_device, device, _device);',s.index('anv_AllocateMemory('));b=s.index('   mem = vk_device_memory_create(',a);prefix=s[a:b]
test=r'''
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "anv_latitude_heap.h"
#define VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO 5
#define VK_ERROR_OUT_OF_DEVICE_MEMORY -2
#define VK_SUCCESS 0
#define ANV_FROM_HANDLE(t,n,v) struct t*n=(v)
#define vk_error(d,e) (e)
#define p_atomic_read(p) (*(p))
#define align64(n,a) (((n)+(a)-1)&~((a)-1))
typedef int VkResult;
typedef uint64_t VkDeviceSize;
struct anv_memory_heap{uint64_t size;};struct anv_memory_type{unsigned heapIndex;};
struct budget{uint64_t used[1];};
struct anv_physical_device{struct{unsigned type_count,heap_count;struct anv_memory_type types[1];struct anv_memory_heap heaps[1];struct budget*heaps_budget;}memory;};
struct anv_device{struct anv_physical_device*physical;};struct anv_device_memory{};
struct allocation{unsigned sType;uint64_t allocationSize;unsigned memoryTypeIndex;};
static int actual_allocation_guard(struct anv_device*_device,const struct allocation*pAllocateInfo){
'''+prefix+r'''
 return 0;
}
int main(){
 const uint64_t cap=4ull<<30;
 for(uint64_t n=0;n<=(32ull<<30);n+=(1ull<<24)){
  assert(anv_latitude_heap_limit(0x7d55,false,true,n)==(n<cap?n:cap));
  assert(anv_latitude_heap_limit(0x7d55,false,true,n)==n);
  assert(anv_latitude_heap_limit(0x7d55,true,true,n)==n);
  assert(anv_latitude_heap_limit(0x7d55,false,false,n)==n);
 }
 assert(anv_latitude_heap_limit(0x7d55,false,true,UINT64_MAX)==cap);
 struct budget usage={0};struct anv_physical_device p={.memory={.type_count=1,.heap_count=1,.heaps_budget=&usage}};p.memory.heaps[0].size=anv_latitude_heap_limit(0x7d55,false,true,12ull<<30);struct anv_device d={&p};
 for(uint64_t used=0;used<=cap;used+=1ull<<26)for(uint64_t size=1;size<=(5ull<<30);size+=(1ull<<24)){
  usage.used[0]=used;struct allocation a={5,size,0};uint64_t aligned=align64(size,4096ull);
  assert(actual_allocation_guard(&d,&a)==((size>cap||aligned>cap||used+aligned>cap)?-2:0));
 }
 for(uint64_t size=UINT64_MAX-4095;size!=UINT64_MAX;size++){
  usage.used[0]=0;struct allocation a={5,size,0};assert(actual_allocation_guard(&d,&a)==-2);
 }
 usage.used[0]=0;struct allocation exact={5,cap,0},over={5,cap+1,0};assert(actual_allocation_guard(&d,&exact)==0);assert(actual_allocation_guard(&d,&over)==-2);
 usage.used[0]=cap;struct allocation small={5,1,0};assert(actual_allocation_guard(&d,&small)==-2);
 puts("PASS actual ANV heap policy + allocation guards ASan/UBSan: 4GiB ceiling, target/platform/local-memory scope, small heaps not inflated, aggregate usage, page rounding and integer-wrap rejection; no GPU allocations");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.c').write_text(test)
 subprocess.run(['xcrun','clang','-std=c11','-fsanitize=address,undefined','-g','-I',str(r/'mesa/src/intel/vulkan'),str(p/'test.c'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
