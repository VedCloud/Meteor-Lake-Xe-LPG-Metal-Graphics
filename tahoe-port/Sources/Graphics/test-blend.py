from pathlib import Path
import subprocess
p=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');s=(p/'plugin/NVMTLDevice.m').read_text()
def function(name):
 a=s.index(name);i=s.index('{',a)+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[a:i]
code='''#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <vulkan/vulkan_core.h>
#include "nvmtl_vk.h"
#include <assert.h>
#include <math.h>
#define nvlog(...) ((void)0)
int nvmtl_vk_dual_src(void){return 1;}
#define NVMTL_MTLBlendFactorUnspecialized MTLBlendFactorUnspecialized
#define NVMTL_MTLBlendOperationUnspecialized MTLBlendOperationUnspecialized
'''+''.join(function(x)+'\n' for x in ['static int nvmtl_blend_factor(','static int nvmtl_blend_op(','static uint32_t nvmtl_a8_factor(','static int nvmtl_blend_from('])+r'''
static double factor(unsigned f,int alpha,int a8){
 double src[4]={.1,.3,.5,.8},dst[4]={.2,.4,.6,.7},secondary[4]={.3,.1,.6,.4},constant[4]={.8,.1,.5,.2};int lane=alpha?3:0;
 if(a8){for(int i=0;i<4;i++)src[i]=.8;dst[0]=.7;dst[3]=1;}
 switch(f){case 0:return 0;case 1:return 1;case 2:return src[lane];case 3:return 1-src[lane];case 4:return dst[lane];case 5:return 1-dst[lane];case 6:return src[3];case 7:return 1-src[3];case 8:return dst[3];case 9:return 1-dst[3];case 10:return constant[lane];case 11:return 1-constant[lane];case 12:return constant[3];case 13:return 1-constant[3];case 14:return alpha?1:fmin(src[3],1-dst[3]);case 15:return secondary[lane];case 16:return 1-secondary[lane];case 17:return secondary[3];case 18:return 1-secondary[3];default:assert(0);return 0;}}
int main(){@autoreleasepool{
 for(unsigned f=0;f<=18;f++)assert(fabs(factor(f,1,0)-factor(nvmtl_a8_factor(f),0,1))<1e-9);
 MTLRenderPipelineColorAttachmentDescriptor*ca=[MTLRenderPipelineColorAttachmentDescriptor new];
 for(NSUInteger mask=0;mask<16;mask++){ca.writeMask=mask;nvk_blend_state b;assert(!nvmtl_blend_from(ca,0,1,&b));assert(b.write_mask==((mask&MTLColorWriteMaskAlpha)?VK_COLOR_COMPONENT_R_BIT:0));assert(!nvmtl_blend_from(ca,1,1,&b)&&!b.write_mask);}
 ca.blendingEnabled=YES;ca.sourceAlphaBlendFactor=MTLBlendFactorDestinationAlpha;ca.destinationAlphaBlendFactor=MTLBlendFactorOneMinusSourceAlpha;
 nvk_blend_state b;assert(!nvmtl_blend_from(ca,0,1,&b));assert(nvmtl_a8_factor(b.src_alpha)==VK_BLEND_FACTOR_DST_COLOR);
 puts("PASS actual A8 blend functions: all 19 alpha factors including dual-source, constants and saturation; 16 write masks; disabled writes; descriptor conversion.");}}
'''
f=p/'build/test-blend.m';f.write_text(code);subprocess.run(['xcrun','clang','-fobjc-arc','-I'+str(r/'Vulkan-Headers/include'),'-I'+str(p/'plugin'),str(f),'-framework','Foundation','-framework','Metal','-o',str(p/'build/test-blend')],check=True);subprocess.run([str(p/'build/test-blend')],check=True)
