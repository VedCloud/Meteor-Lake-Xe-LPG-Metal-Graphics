from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parent.parent
s=r'''#include <vulkan/vulkan.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "nvmtl_memory_identity.h"
int main(void){
 VkPhysicalDeviceMemoryProperties p={0};assert(!nvmtl_memory_properties_valid(0));assert(!nvmtl_memory_properties_valid(&p));
 p.memoryHeapCount=1;p.memoryTypeCount=1;p.memoryHeaps[0].size=8ull<<30;p.memoryTypes[0].heapIndex=0;
 assert(nvmtl_memory_properties_valid(&p));
 p.memoryTypes[0].heapIndex=1;assert(!nvmtl_memory_properties_valid(&p));p.memoryTypes[0].heapIndex=0;
 p.memoryHeapCount=VK_MAX_MEMORY_HEAPS+1;assert(!nvmtl_memory_properties_valid(&p));p.memoryHeapCount=1;
 p.memoryTypeCount=VK_MAX_MEMORY_TYPES+1;assert(!nvmtl_memory_properties_valid(&p));p.memoryTypeCount=1;
 p.memoryHeaps[0].size=0;assert(!nvmtl_memory_properties_valid(&p));
 for(uint32_t n=1;n<=VK_MAX_MEMORY_HEAPS;n++)for(uint32_t t=1;t<=VK_MAX_MEMORY_TYPES;t++){
  p.memoryHeapCount=n;p.memoryTypeCount=t;
  for(uint32_t i=0;i<n;i++)p.memoryHeaps[i].size=(uint64_t)(i+1)<<30;
  for(uint32_t i=0;i<t;i++)p.memoryTypes[i].heapIndex=i%n;
  assert(nvmtl_memory_properties_valid(&p));
  p.memoryTypes[t-1].heapIndex=n;assert(!nvmtl_memory_properties_valid(&p));
 }
 assert(nvmtl_mtl_unified_identity(0x8086,0x7d55,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU));
 assert(!nvmtl_mtl_unified_identity(0x8086,0x7d55,VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU));
 assert(!nvmtl_mtl_unified_identity(0x10de,0x7d55,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU));
 assert(!nvmtl_mtl_unified_identity(0x8086,0x7d55,VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU));
 assert(nvmtl_mtl_userptr_unavailable(0x8086,0x7d55));
 assert(!nvmtl_mtl_userptr_unavailable(0x10de,0x7d55));
 assert(!nvmtl_mtl_userptr_unavailable(0x8086,0x7d55));
 for(uint64_t heap=1;heap<10000;heap+=17)for(uint64_t budget=0;budget<15000;budget+=73){
  uint64_t limit=nvmtl_shared_working_set(heap,budget);
  assert(limit<=heap && limit<=budget);
  assert(limit==(budget<heap?budget:heap));
 }
 assert(nvmtl_shared_working_set(UINT64_MAX,UINT64_MAX)==UINT64_MAX);
 assert(nvmtl_shared_working_set(0,UINT64_MAX)==0);
 puts("PASS actual memory identity helpers ASan/UBSan: 512 valid heap/type combinations and invalid bounds/zero heap/indices; exact integrated-MTL scope, unsupported userptr scope and bounded shared budgets");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);f=p/'test.c';f.write_text(s)
 subprocess.run(['xcrun','clang','-fsanitize=address,undefined','-g','-I'+str(r/'Vulkan-Headers/include'),'-I'+str(r/'NullMoth/plugin'),str(f),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
