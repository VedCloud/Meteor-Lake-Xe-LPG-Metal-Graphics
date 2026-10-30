from pathlib import Path
import subprocess
p=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');s=(p/'plugin/nvmtl_vk.c').read_text()
def fn(name):
 a=s.index('int '+name+'(');i=s.index('{',a)+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[a:i]
pre=r'''
#include <mach/mach.h>
#include <vulkan/vulkan_core.h>
#include "nvmtl_vk.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define nvlog(...) ((void)0)
static unsigned calls=0,destroyed=0;static int fail_create=0,fail_grow=0;static int last_a8=-1;static uint32_t last_level,last_layers;
static void* grow(void*p,size_t n){return fail_grow?NULL:realloc(p,n);}
#define realloc grow
static VkDevice g_dev;
static void VKAPI_CALL destroy(VkDevice d,VkImageView v,const VkAllocationCallbacks*a){destroyed++;}
static VkResult VKAPI_CALL create(VkDevice d,const VkImageViewCreateInfo*v,const VkAllocationCallbacks*a,VkImageView*out){assert(v->components.r==0&&v->components.g==0&&v->components.b==0&&v->components.a==0);*out=(VkImageView)0x1234;return VK_SUCCESS;}
static PFN_vkDestroyImageView pvkDestroyImageView=destroy;
static PFN_vkCreateImageView pvkCreateImageView=create;
static int nvmtl_fmt_stencil(uint32_t f){return f==VK_FORMAT_D24_UNORM_S8_UINT;}
int nvmtl_vk_image_view_create_range(nvk_image*i,uint32_t f,int a8,uint32_t l,uint32_t levels,uint32_t slice,uint32_t layers,uint32_t type,void**out){calls++;last_a8=a8;last_level=l;last_layers=layers;assert(levels==1);if(fail_create)return -1;*out=(void*)0x1234;return 0;}
'''
post=r'''
int main(){nvk_cmdbuf c={0};nvk_image i={0};i.img=(void*)0x10;i.view=(void*)0x11;i.w=64;i.h=32;i.fmt=VK_FORMAT_R8_UNORM;i.mtl_type=2;void*out=0;uint32_t w,h;
 assert(!nvmtl_vk_cmd_attachment_view(&c,&i,0,0,0,&out,&w,&h));assert(calls==1&&last_a8==0&&out!=i.view&&i.view==(void*)0x11&&c.nrv==1&&w==64&&h==32);
 unsigned previous=calls;i.fmt=VK_FORMAT_R8G8B8A8_UNORM;assert(!nvmtl_vk_cmd_attachment_view(&c,&i,0,0,0,&out,&w,&h));assert(calls==previous&&out==i.view);
 assert(!nvmtl_vk_cmd_attachment_view_fmt(&c,&i,0,0,0,VK_FORMAT_R8_UNORM,1,&out,&w,&h));assert(last_a8==0&&c.nrv==2);
 i.layers=3;i.mtl_type=3;assert(!nvmtl_vk_cmd_attachment_view_n(&c,&i,0,0,0,2,VK_FORMAT_R8_UNORM,1,&out,&w,&h));assert(last_a8==0&&last_layers==2&&c.nrv==3);
 i.fmt=VK_FORMAT_R8_UNORM;i.mips=3;assert(!nvmtl_vk_cmd_attachment_view(&c,&i,0,1,0,&out,&w,&h));assert(last_level==1&&w==32&&h==16&&last_a8==0);
 previous=calls;assert(nvmtl_vk_cmd_attachment_view(&c,&i,0,3,0,&out,&w,&h)==-1&&out==NULL&&calls==previous);
 fail_create=1;assert(nvmtl_vk_cmd_attachment_view(&c,&i,0,0,0,&out,&w,&h)==-1&&out==NULL);fail_create=0;
 c.nrv=c.caprv;fail_grow=1;unsigned d=destroyed;assert(nvmtl_vk_cmd_attachment_view(&c,&i,0,0,0,&out,&w,&h)==-1&&out==NULL&&destroyed==d+1);free(c.rvs);
 puts("PASS actual attachment helpers ASan/UBSan: A8 identity render views, unchanged sampling view and ordinary RGBA fast path; format/layer/mip routing; invalid range/create/retirement-allocation failures.");}
'''
f=p/'build/test-attachment.c';f.write_text(pre+''.join(fn(n)+'\n' for n in ['nvmtl_vk_cmd_retire_view','nvmtl_vk_cmd_attachment_view','nvmtl_vk_cmd_attachment_view_fmt','nvmtl_vk_cmd_attachment_view_n'])+post);subprocess.run(['xcrun','clang','-O1','-fsanitize=address,undefined','-I'+str(r/'Vulkan-Headers/include'),'-I'+str(p/'plugin'),str(f),'-o',str(p/'build/test-attachment')],check=True);subprocess.run([str(p/'build/test-attachment')],check=True)
