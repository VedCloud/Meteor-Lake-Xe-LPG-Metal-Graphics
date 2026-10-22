#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <IOKit/IOKitLib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int fail(NSString *message) { fprintf(stderr,"FAIL: %s\n",message.UTF8String); return 1; }
static BOOL complete(id<MTLCommandBuffer> cb) {
    [cb commit]; [cb waitUntilCompleted];
    if (cb.status != MTLCommandBufferStatusCompleted) {
        fprintf(stderr,"Command failed: %s\n",cb.error.description.UTF8String); return NO;
    }
    return YES;
}
int main(void) { @autoreleasepool {
    setbuf(stdout,NULL);
    id<MTLDevice> gpu=nil;
    for (id<MTLDevice> candidate in MTLCopyAllDevices()) {
        io_service_t service=IOServiceGetMatchingService(kIOMainPortDefault,IORegistryEntryIDMatching(candidate.registryID));
        BOOL match=service && IOObjectConformsTo(service,"NMXAccel");
        if(service) IOObjectRelease(service);
        printf("Metal device: %s registry=0x%llx NMXAccel=%s\n",candidate.name.UTF8String,(unsigned long long)candidate.registryID,match?"yes":"no");
        if(match) gpu=candidate;
    }
    if(!gpu) return fail(@"No Metal device backed by NMXAccel; GPU acceleration is not proven.");
    NSString *source=@"#include <metal_stdlib>\nusing namespace metal;\n"
    "kernel void check(device uint *out [[buffer(0)]], constant uint &seed [[buffer(1)]], uint i [[thread_position_in_grid]]) { uint x=i^seed; x=x*1664525u+1013904223u; out[i]=x^(x>>13); }\n"
    "vertex float4 vert(uint i [[vertex_id]]) { float2 p[3]={float2(-1,-1),float2(3,-1),float2(-1,3)}; return float4(p[i],0,1); }\n"
    "fragment float4 frag() { return float4(0.25,0.5,0.75,1); }\n";
    NSError *error=nil; id<MTLLibrary> lib=[gpu newLibraryWithSource:source options:nil error:&error];
    if(!lib) return fail([NSString stringWithFormat:@"Shader compilation: %@",error]);
    id<MTLComputePipelineState> cp=[gpu newComputePipelineStateWithFunction:[lib newFunctionWithName:@"check"] error:&error];
    if(!cp) return fail([NSString stringWithFormat:@"Compute pipeline: %@",error]);
    const NSUInteger count=65536; uint32_t seed=arc4random();
    id<MTLBuffer> output=[gpu newBufferWithLength:count*sizeof(uint32_t) options:MTLResourceStorageModeShared];
    if(!output || !output.contents) return fail(@"GPU buffer allocation failed");
    memset(output.contents,0xa5,output.length);
    id<MTLCommandQueue> queue=[gpu newCommandQueue];
    if(!queue) return fail(@"No GPU command queue");
    id<MTLCommandBuffer> cb=[queue commandBuffer];id<MTLComputeCommandEncoder> ce=[cb computeCommandEncoder];
    if(!cb || !ce) return fail(@"No compute command encoder");
    [ce setComputePipelineState:cp];[ce setBuffer:output offset:0 atIndex:0];[ce setBytes:&seed length:sizeof(seed) atIndex:1];
    NSUInteger width=MIN((NSUInteger)64,cp.maxTotalThreadsPerThreadgroup);
    if(width==0) return fail(@"Invalid GPU threadgroup limit");
    [ce dispatchThreads:MTLSizeMake(count,1,1) threadsPerThreadgroup:MTLSizeMake(width,1,1)];[ce endEncoding];
    if(!complete(cb)) return 1;
    uint32_t *pixels=output.contents;
    for(NSUInteger i=0;i<count;i++) { uint32_t x=(uint32_t)i^seed;x=x*1664525u+1013904223u;uint32_t expected=x^(x>>13);
        if(pixels[i]!=expected) return fail([NSString stringWithFormat:@"Compute mismatch at %lu: %08x != %08x",(unsigned long)i,pixels[i],expected]); }
    printf("PASS compute: %lu values, seed=%u, all results match\n",(unsigned long)count,seed);
    MTLTextureDescriptor *td=[MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:64 height:64 mipmapped:NO];
    td.usage=MTLTextureUsageRenderTarget;td.storageMode=MTLStorageModeShared;
    id<MTLTexture> texture=[gpu newTextureWithDescriptor:td];if(!texture) return fail(@"Render texture allocation failed");
    MTLRenderPipelineDescriptor *pd=[MTLRenderPipelineDescriptor new];pd.vertexFunction=[lib newFunctionWithName:@"vert"];pd.fragmentFunction=[lib newFunctionWithName:@"frag"];pd.colorAttachments[0].pixelFormat=td.pixelFormat;
    id<MTLRenderPipelineState> rp=[gpu newRenderPipelineStateWithDescriptor:pd error:&error];if(!rp)return fail([NSString stringWithFormat:@"Render pipeline: %@",error]);
    MTLRenderPassDescriptor *pass=[MTLRenderPassDescriptor renderPassDescriptor];pass.colorAttachments[0].texture=texture;pass.colorAttachments[0].loadAction=MTLLoadActionClear;pass.colorAttachments[0].storeAction=MTLStoreActionStore;pass.colorAttachments[0].clearColor=MTLClearColorMake(1,0,1,1);
    cb=[queue commandBuffer];id<MTLRenderCommandEncoder> re=[cb renderCommandEncoderWithDescriptor:pass];if(!cb||!re)return fail(@"No render command encoder");
    [re setRenderPipelineState:rp];[re drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];[re endEncoding];if(!complete(cb))return 1;
    uint8_t image[64*64*4];memset(image,0xa5,sizeof(image));[texture getBytes:image bytesPerRow:64*4 fromRegion:MTLRegionMake2D(0,0,64,64) mipmapLevel:0];
    for(NSUInteger i=0;i<64*64;i++) { uint8_t *p=image+4*i;if(abs((int)p[0]-64)>1||abs((int)p[1]-128)>1||abs((int)p[2]-191)>1||p[3]!=255)return fail([NSString stringWithFormat:@"Rendered pixel %lu mismatch: %u,%u,%u,%u",(unsigned long)i,p[0],p[1],p[2],p[3]]); }
    puts("PASS render: 4096 pixels checked after shader execution");
    puts("PASS GPU OUTPUT: NMXAccel-backed Metal compute and rendering. Desktop composition and stability need separate validation.");
    return 0;
} }
