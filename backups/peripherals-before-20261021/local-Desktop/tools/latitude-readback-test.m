#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <IOSurface/IOSurface.h>
#import <IOKit/IOKitLib.h>
#import <objc/message.h>
#include <pthread.h>
#include <stdatomic.h>
#include <mach/mach.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static unsigned width=64,height=64,iterations=100,threads=1,padding=0;
static BOOL raw=NO,surface=NO,fresh=NO,shared=NO,native=NO,bgra=NO;
static const char *expectedPolicy;
static atomic_int failed;
static FILE *terminal;
static pthread_mutex_t ownership=PTHREAD_MUTEX_INITIALIZER;
static id<MTLDevice> gpu;
static id<MTLRenderPipelineState> pipeline;
static id<MTLTexture> commonInput,commonOutput;
static id<MTLTexture> texture(BOOL output) {
 MTLTextureDescriptor *d=[MTLTextureDescriptor texture2DDescriptorWithPixelFormat:(output&&bgra ? MTLPixelFormatBGRA8Unorm:MTLPixelFormatRGBA8Unorm) width:width height:height mipmapped:NO];
 d.storageMode=MTLStorageModeShared;d.usage=MTLTextureUsageShaderRead|MTLTextureUsageRenderTarget;
 if (!surface || !output) return [gpu newTextureWithDescriptor:d];
 NSDictionary *properties=@{(id)kIOSurfaceWidth:@(width),(id)kIOSurfaceHeight:@(height),
  (id)kIOSurfaceBytesPerElement:@4,(id)kIOSurfaceBytesPerRow:@(width*4),
  (id)kIOSurfaceAllocSize:@((size_t)width*height*4),(id)kIOSurfacePixelFormat:@(bgra?0x42475241:0x52474241)};
 IOSurfaceRef s=IOSurfaceCreate((__bridge CFDictionaryRef)properties);
 if (!s) return nil;
 id<MTLTexture> t=[gpu newTextureWithDescriptor:d iosurface:s plane:0];CFRelease(s);return t;
}
static void error(const char *why,unsigned worker,unsigned frame) {
 if (!atomic_exchange(&failed,1)) {
  fprintf(stderr,"FAIL worker=%u frame=%u: %s\n",worker,frame,why);
  if(terminal)fprintf(terminal,"FAIL worker=%u frame=%u: %s\n",worker,frame,why);
 }
}
static unsigned char channel(unsigned x,unsigned y,unsigned seed,unsigned c) {
 return (unsigned char)(x*17u+y*31u+seed*13u+c*71u);
}
static void *run(void *arg) { @autoreleasepool {
 unsigned worker=(unsigned)(uintptr_t)arg; size_t pitch=(size_t)width*4+padding,bytes=pitch*height;
 uint8_t *input=malloc(bytes),*output=malloc(bytes);
 if(!input||!output){error("host allocation",worker,0);free(input);free(output);return NULL;}
 id<MTLCommandQueue> queue=[gpu newCommandQueue];
 id<MTLTexture> in=shared ? commonInput : texture(NO),out=shared ? commonOutput : texture(YES);
 if(!queue||!in||!out){error("queue/texture allocation",worker,0);free(input);free(output);return NULL;}
 for(unsigned frame=0;frame<iterations&&!atomic_load(&failed);frame++) { @autoreleasepool {
  if(shared)pthread_mutex_lock(&ownership);
  if(atomic_load(&failed)){if(shared)pthread_mutex_unlock(&ownership);break;}
  if(fresh){in=texture(NO);out=texture(YES);}
  unsigned seed=frame+worker*100003u,delta=(seed*19u+1u)&255u;
  if(!in||!out){error("fresh texture allocation",worker,frame);if(shared)pthread_mutex_unlock(&ownership);break;}
  memset(input,0x6d,bytes);memset(output,0xa5,bytes);
  for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++){
   uint8_t *p=input+(size_t)y*pitch+x*4;for(unsigned c=0;c<3;c++)p[c]=channel(x,y,seed,c);p[3]=255;
  }
  [in replaceRegion:MTLRegionMake2D(0,0,width,height) mipmapLevel:0 withBytes:input bytesPerRow:pitch];
  id<MTLTexture> result=in;
  if(!raw){
   MTLRenderPassDescriptor *pass=[MTLRenderPassDescriptor renderPassDescriptor];
   pass.colorAttachments[0].texture=out;pass.colorAttachments[0].loadAction=MTLLoadActionClear;
   pass.colorAttachments[0].storeAction=MTLStoreActionStore;
   pass.colorAttachments[0].clearColor=MTLClearColorMake(1,0,1,1);
   id<MTLCommandBuffer> cb=[queue commandBuffer];id<MTLRenderCommandEncoder> e=[cb renderCommandEncoderWithDescriptor:pass];
   if(!cb||!e){error("command encoding",worker,frame);if(shared)pthread_mutex_unlock(&ownership);break;}
   [e setRenderPipelineState:pipeline];[e setFragmentTexture:in atIndex:0];[e setFragmentBytes:&delta length:sizeof(delta) atIndex:0];
   [e drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];[e endEncoding];[cb commit];[cb waitUntilCompleted];
   if(cb.status!=MTLCommandBufferStatusCompleted){error(cb.error.description.UTF8String ?: "command failed",worker,frame);if(shared)pthread_mutex_unlock(&ownership);break;}
   result=out;
  }
  if(surface && !atomic_load(&failed)) {
   IOSurfaceRef backing=out.iosurface;
   if(!backing || IOSurfaceLock(backing,kIOSurfaceLockReadOnly,NULL)!=kIOReturnSuccess) error("IOSurface backing lock",worker,frame);
   else {
    const uint8_t *base=IOSurfaceGetBaseAddress(backing);size_t stride=IOSurfaceGetBytesPerRow(backing);
    if(!base||stride<(size_t)width*4) error("IOSurface backing geometry",worker,frame);
    else for(unsigned y=0;y<height&&!atomic_load(&failed);y++)for(unsigned x=0;x<width;x++)for(unsigned c=0;c<4;c++) {
     unsigned expected=c==3?255:(channel(x,y,seed,bgra?2-c:c)+delta)&255;
     unsigned got=base[(size_t)y*stride+x*4+c];
     if(got!=expected){char msg[240];snprintf(msg,sizeof msg,"IOSurface backing (%u,%u) c=%u got=%u expected=%u",x,y,c,got,expected);error(msg,worker,frame);break;}
    }
    IOSurfaceUnlock(backing,kIOSurfaceLockReadOnly,NULL);
   }
  }
  if(atomic_load(&failed)){if(shared)pthread_mutex_unlock(&ownership);break;}
  [result getBytes:output bytesPerRow:pitch fromRegion:MTLRegionMake2D(0,0,width,height) mipmapLevel:0];
  for(unsigned y=0;y<height&&!atomic_load(&failed);y++){
   for(unsigned x=0;x<width;x++)for(unsigned c=0;c<4;c++){
    unsigned expected=c==3 ? 255 : (channel(x,y,seed,(!raw&&bgra&&c<3)?2-c:c)+(raw?0:delta))&255;
    unsigned got=output[(size_t)y*pitch+x*4+c];
    if(got!=expected){char message[240];snprintf(message,sizeof message,"pixel (%u,%u) channel=%u got=%u expected=%u seed=%u",x,y,c,got,expected,seed);error(message,worker,frame);break;}
   }
   for(size_t x=(size_t)width*4;x<pitch;x++)if(output[(size_t)y*pitch+x]!=0xa5){error("row padding overwritten",worker,frame);break;}
  }
  if(shared)pthread_mutex_unlock(&ownership);
  if((frame+1)%50==0 || frame+1==iterations){
   struct task_basic_info_64 info;mach_msg_type_number_t n=TASK_BASIC_INFO_64_COUNT;
   kern_return_t kr=task_info(mach_task_self(),TASK_BASIC_INFO_64,(task_info_t)&info,&n);
   printf("progress worker=%u frames=%u/%u resident=%llu\n",worker,frame+1,iterations,kr==KERN_SUCCESS?(unsigned long long)info.resident_size:0ull);
   if(terminal)fprintf(terminal,"progress worker=%u frames=%u/%u resident=%llu\n",worker,frame+1,iterations,kr==KERN_SUCCESS?(unsigned long long)info.resident_size:0ull);
  }
 }}
 free(input);free(output);return NULL;
}}
static unsigned number(const char *s,unsigned max){char *end;unsigned long v=strtoul(s,&end,10);if(!*s||*end||v>max||!v){fprintf(stderr,"Invalid numeric argument: %s\n",s);exit(2);}return (unsigned)v;}
int main(int argc,char **argv) { @autoreleasepool {
 setbuf(stdout,NULL);
 if(!isatty(STDOUT_FILENO)) { terminal=fopen("/dev/tty","w");if(terminal)setbuf(terminal,NULL); }
 for(int i=1;i<argc;i++){
  if(!strcmp(argv[i],"--iosurface"))surface=YES;else if(!strcmp(argv[i],"--raw"))raw=YES;else if(!strcmp(argv[i],"--bgra"))bgra=YES;
  else if(!strcmp(argv[i],"--fresh-textures"))fresh=YES;else if(!strcmp(argv[i],"--shared-texture"))shared=YES;
  else if(!strcmp(argv[i],"--allow-native"))native=YES;
  else if(!strcmp(argv[i],"--expect-policy")&&i+1<argc)expectedPolicy=argv[++i];
  else if(!strcmp(argv[i],"--iterations")&&i+1<argc)iterations=number(argv[++i],10000);
  else if(!strcmp(argv[i],"--threads")&&i+1<argc)threads=number(argv[++i],2);
  else if(!strcmp(argv[i],"--padding")&&i+1<argc)padding=number(argv[++i],4096);
  else if(!strcmp(argv[i],"--size")&&i+1<argc){if(sscanf(argv[++i],"%ux%u",&width,&height)!=2||!width||!height||width>4096||height>4096)return 2;}
  else {fprintf(stderr,"Unknown/incomplete option: %s\n",argv[i]);return 2;}
 }
 if((fresh&&shared)||(raw&&surface)){fputs("Incompatible case options\n",stderr);return 2;}
 for(id<MTLDevice> d in MTLCopyAllDevices()){
  io_service_t service=IOServiceGetMatchingService(kIOMainPortDefault,IORegistryEntryIDMatching(d.registryID));
  BOOL match=service&&IOObjectConformsTo(service,"NMXAccel");if(service)IOObjectRelease(service);
  if(match){gpu=d;break;}
 }
 if(!gpu&&native)gpu=MTLCreateSystemDefaultDevice();
 if(!gpu){fputs("FAIL: no NMXAccel-backed Metal device; boot the isolated GPU configuration first\n",stderr);return 1;}
 if(expectedPolicy){
  unsigned expected=!strcmp(expectedPolicy,"enforced")?47u:!strcmp(expectedPolicy,"enforced-fresh-staging")?63u:0u;
  SEL policy=NSSelectorFromString(@"nvmtlDiagnosticPolicy");
  if(!expected||![gpu respondsToSelector:policy]){fputs("FAIL effective backend policy unavailable/invalid\n",stderr);return 1;}
  unsigned actual=(unsigned)((NSUInteger(*)(id,SEL))objc_msgSend)(gpu,policy);
  printf("POLICY effective=0x%x expected=0x%x profile=%s\n",actual,expected,expectedPolicy);
  if(actual!=expected){fputs("FAIL effective backend policy differs from request; no workload submitted\n",stderr);return 1;}
  puts("PASS effective backend policy: verified inside the loaded driver after release initialization");
 }
 printf("device=%s native-test=%d size=%ux%u pitch=%u iterations=%u threads=%u raw=%d iosurface=%d fresh-textures=%d shared-texture=%d bgra=%d\n",gpu.name.UTF8String,native,width,height,width*4+padding,iterations,threads,raw,surface,fresh,shared,bgra);
 if(!raw){
  NSString *source=@"#include <metal_stdlib>\nusing namespace metal;\n"
   "vertex float4 vert(uint i [[vertex_id]]){float2 p[3]={float2(-1,-1),float2(3,-1),float2(-1,3)};return float4(p[i],0,1);}\n"
   "fragment float4 frag(float4 p [[position]],texture2d<float,access::read> input [[texture(0)]],constant uint &delta [[buffer(0)]]){uint4 v=uint4(round(input.read(uint2(p.xy))*255.0));v.xyz=(v.xyz+delta)&255u;return float4(v)/255.0;}\n";
  NSError *err=nil;id<MTLLibrary> lib=[gpu newLibraryWithSource:source options:nil error:&err];
  MTLRenderPipelineDescriptor *d=[MTLRenderPipelineDescriptor new];d.vertexFunction=[lib newFunctionWithName:@"vert"];d.fragmentFunction=[lib newFunctionWithName:@"frag"];d.colorAttachments[0].pixelFormat=bgra?MTLPixelFormatBGRA8Unorm:MTLPixelFormatRGBA8Unorm;
  pipeline=lib?[gpu newRenderPipelineStateWithDescriptor:d error:&err]:nil;
  if(!pipeline){fprintf(stderr,"FAIL pipeline: %s\n",err.description.UTF8String);return 1;}
 }
 if(shared){commonInput=texture(NO);commonOutput=texture(YES);}
 pthread_t workers[2];unsigned started=0;
 for(unsigned i=0;i<threads;i++){if(pthread_create(&workers[i],NULL,run,(void *)(uintptr_t)i)){error("pthread_create",i,0);break;}started++;}
 for(unsigned i=0;i<started;i++)pthread_join(workers[i],NULL);
 if(!native){
  SEL stats=NSSelectorFromString(@"nvmtlCommandPoolStats");
  NSArray *v=[gpu respondsToSelector:stats]?((id(*)(id,SEL))objc_msgSend)(gpu,stats):nil;
  if(v.count!=8){error("command pool statistics unavailable",0,0);}
  else {
   unsigned long long n[8];for(unsigned i=0;i<8;i++)n[i]=[v[i] unsignedLongLongValue];
   printf("POOL created=%llu destroyed=%llu live=%llu peak=%llu quarantined=%llu pending=%llu completed=%llu abandoned=%llu\n",n[0],n[1],n[2],n[3],n[4],n[5],n[6],n[7]);
   if(!n[0]||!n[3]||n[0]!=n[1]||n[2]||n[4]||n[5]||n[0]!=n[6]+n[7])error("command pool cleanup imbalance",0,0);
   else puts("PASS command-pool cleanup: all created pools destroyed; zero live, pending or quarantined pools");
  }
 }
 if(atomic_load(&failed))return 1;
 if(surface)puts("PASS IOSurface backing: all published CPU pixels checked after GPU completion");
 printf("PASS exact readback: %llu frames, all pixels and row padding checked%s\n",(unsigned long long)iterations*threads,native?" (native harness check only)":"");return 0;
}}
