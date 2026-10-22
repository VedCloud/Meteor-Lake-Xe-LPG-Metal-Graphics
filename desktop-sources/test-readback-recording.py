from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/plugin/nvmtl_vk.c').read_text();a=s.index('static int nvmtl_vk_readback(');b=s.index('\nint nvmtl_vk_image_read(',a)
pre='''#include <mach/mach.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vulkan/vulkan.h>
#include "nvmtl_vk.h"
#include "nvmtl_readback.h"
#include "nvmtl_present_cost.h"
#define nvmtl_cpu_cache_sync nvmtl_cpu_cache_sync_impl
#include "nvmtl_cpu_cache.h"
#undef nvmtl_cpu_cache_sync
#include "nvmtl_desktop_policy.h"
static unsigned g_vendor=0x8086,g_pci_device=0x7d45;static int g_state=1;
static const char *process="WindowServer";
#define getprogname() process
static VkPhysicalDeviceMemoryProperties g_memp;
static unsigned g_copy_bpp_override,step,destroyed,created,begin_fail;
static int submit_fail;static unsigned tls_calls;
static uint8_t staging[64];
static void nvlog(const char *s,...){(void)s;}
static VkImageAspectFlags nvmtl_copy_aspect(const nvk_image *i){(void)i;return VK_IMAGE_ASPECT_COLOR_BIT;}
static VkImageAspectFlags nvmtl_barrier_aspect(const nvk_image *i){return nvmtl_copy_aspect(i);}
void nvmtl_vk_image_upload_barrier(nvk_image *i){(void)i;}
int nvmtl_vk_buffer_create(size_t n,int host,nvk_buffer *b){assert(n<=64&&host==3);created++;*b=(nvk_buffer){.buf=(void *)2,.map=staging,.memtype1=1,.size=n};return 0;}
static int nvmtl_vk_stage(nvk_queue *q,size_t n,nvk_buffer *b){(void)q;tls_calls++;return nvmtl_vk_buffer_create(n,3,b);}
void nvmtl_vk_buffer_destroy(nvk_buffer *b){assert(step==0||step==5);destroyed++;memset(b,0,sizeof *b);}
int nvmtl_vk_cmd_begin(nvk_queue *q,nvk_cmdbuf *c){(void)q;c->cb=(void *)3;return begin_fail?-1:0;}
static void pvkCmdPipelineBarrier(VkCommandBuffer cb,VkPipelineStageFlags src,VkPipelineStageFlags dst,VkDependencyFlags flags,uint32_t nm,const VkMemoryBarrier *m,uint32_t nb,const VkBufferMemoryBarrier *bb,uint32_t ni,const VkImageMemoryBarrier *ib){
 (void)cb;(void)flags;(void)nm;(void)m;
 if(step==0){assert(src==VK_PIPELINE_STAGE_ALL_COMMANDS_BIT&&dst==VK_PIPELINE_STAGE_TRANSFER_BIT&&ni==1&&nb==0);assert(ib->oldLayout==VK_IMAGE_LAYOUT_GENERAL&&ib->newLayout==VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);}
 else if(step==2){assert(src==VK_PIPELINE_STAGE_TRANSFER_BIT&&dst==VK_PIPELINE_STAGE_HOST_BIT&&nb==1&&ni==0);assert(bb->srcAccessMask==VK_ACCESS_TRANSFER_WRITE_BIT&&bb->dstAccessMask==VK_ACCESS_HOST_READ_BIT);}
 else if(step==3){assert(src==VK_PIPELINE_STAGE_TRANSFER_BIT&&dst==VK_PIPELINE_STAGE_ALL_COMMANDS_BIT&&ni==1);assert(ib->oldLayout==VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL&&ib->newLayout==VK_IMAGE_LAYOUT_GENERAL);}
 else assert(0);step++;
}
static void pvkCmdCopyImageToBuffer(VkCommandBuffer cb,VkImage i,VkImageLayout l,VkBuffer b,uint32_t n,const VkBufferImageCopy *p){(void)cb;(void)i;(void)l;(void)b;assert(step==1&&n==1&&p->imageExtent.width==2&&p->imageExtent.height==2);step++;}
int nvmtl_vk_submit_wait_at(nvk_cmdbuf *c,const char *w){(void)c;(void)w;assert(step==4);step++;return submit_fail;}
'''
pre += '\n#include \"nvmtl_readback_stage.h\"\n'
pre += '''\nstatic unsigned cache_pre,cache_post;\nstatic int nvmtl_cpu_cache_sync(void *base,size_t bytes,size_t capacity){assert(step==0||step==5);if(step==0)cache_pre++;else cache_post++;return nvmtl_cpu_cache_sync_impl(base,bytes,capacity);}\n'''
post='''int main(void){
 nvk_queue q={0};nvk_image i={.img=(void *)1,.w=2,.h=2,.bpp=4,.usage=VK_IMAGE_USAGE_TRANSFER_SRC_BIT,.layout=VK_IMAGE_LAYOUT_GENERAL};uint8_t out[24];
 g_memp.memoryTypeCount=1;g_memp.memoryTypes[0].propertyFlags=VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
 setenv("NVMTL_READBACK_STAGE_FRESH","1",1);for(int k=0;k<16;k++)staging[k]=k;
 memset(out,0xa5,sizeof out);assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));
 assert(step==5&&destroyed==1&&i.layout==VK_IMAGE_LAYOUT_GENERAL&&i.trace_id);
 assert(!memcmp(out,staging,8)&&!memcmp(out+12,staging+8,8)&&out[8]==0xa5&&out[20]==0xa5);
 uint64_t image_id=i.trace_id;step=0;submit_fail=-2;memset(out,0xa5,sizeof out);
 assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));
 assert(step==5&&destroyed==1&&i.trace_id==image_id);for(int k=0;k<24;k++)assert(out[k]==0xa5);
 step=0;begin_fail=1;assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(destroyed==2&&step==0);begin_fail=0;
 g_memp.memoryTypes[0].propertyFlags=VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
 assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(destroyed==3&&step==0);
 unsigned before=created;assert(nvmtl_vk_readback(&q,&i,out,7,0,0,2,2,0,0));assert(created==before);
 g_memp.memoryTypes[0].propertyFlags=VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
 assert(!tls_calls);submit_fail=0;unsigned free0=destroyed,make0=created;
 for(int k=0;k<20000;k++){step=0;assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));}
 assert(created-make0==20000&&destroyed-free0==20000&&!tls_calls);
 step=0;submit_fail=-1;free0=destroyed;assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(destroyed==free0+1);
 g_state=-1;step=0;free0=destroyed;assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(destroyed==free0);g_state=1;
 unsetenv("NVMTL_READBACK_STAGE_FRESH");submit_fail=0;process="normal-app";step=0;free0=destroyed;assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(tls_calls==1&&destroyed==free0);
 process="WindowServer";g_pci_device=0x7d55;step=0;assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(tls_calls==2);
 process="WindowServer";g_pci_device=0x7d45;unsigned start_created=created,start_destroyed=destroyed;
 for(int k=0;k<1000;k++){step=0;memset(out,0xa5,sizeof out);assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(!memcmp(out,staging,8)&&!memcmp(out+12,staging+8,8)&&out[8]==0xa5);}
 assert(created==start_created+1&&destroyed==start_destroyed&&nvmtl_readback_slots[0].buffer.buf);
 step=0;begin_fail=1;assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));begin_fail=0;
 step=0;assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(created==start_created+1&&destroyed==start_destroyed);
 g_memp.memoryTypes[0].propertyFlags=VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;step=0;
 assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(destroyed==start_destroyed+1&&!nvmtl_readback_slots[0].buffer.buf);
 g_memp.memoryTypes[0].propertyFlags|=VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;step=0;
 assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(created==start_created+2);
 step=0;submit_fail=-2;memset(out,0xa5,sizeof out);assert(nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(nvmtl_readback_slots[0].quarantined&&destroyed==start_destroyed+1);for(int k=0;k<24;k++)assert(out[k]==0xa5);
 submit_fail=0;step=0;assert(!nvmtl_vk_readback(&q,&i,out,12,0,0,2,2,0,0));assert(created==start_created+3&&nvmtl_readback_slots[1].buffer.buf);
 assert(cache_pre>=1002&&cache_post>=1001&&cache_pre>=cache_post);
 step=0;memset(out,0xa5,sizeof out);assert(!nvmtl_vk_readback(&q,&i,out,8,0,0,2,2,0,0));
 assert(!memcmp(out,staging,16)&&out[16]==0xa5&&step==5);
 puts("PASS actual readback implementation: dependency/copy/restore/wait order, exact padded output, stable identity, timeout retains memory and leaves CPU output untouched, failure cleanup, noncoherent rejection; 20000 dedicated desktop allocations reclaimed, known pre-submit failure freed, app/non-target TLS preserved;1000 cached full-copy cycles allocate once, uncertain lease quarantined and skipped");
}
'''
f=r/'desktop-build/test-readback-recording.c';f.write_text(pre+s[a:b]+post)
subprocess.run(['xcrun','clang','-arch','x86_64','-fsanitize=address,undefined','-g','-I',str(r/'Vulkan-Headers/include'),'-I',str(r/'NullMoth/plugin'),str(f),'-o',str(r/'desktop-build/test-readback-recording')],check=True)
subprocess.run([str(r/'desktop-build/test-readback-recording')],check=True)
