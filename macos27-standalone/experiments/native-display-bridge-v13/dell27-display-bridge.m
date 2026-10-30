// SPDX-License-Identifier: MIT
// Guest-only experimental software presentation. No host display changes in --inspect.
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOSurface/IOSurface.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/graphics/IOGraphicsTypes.h>
#import <objc/runtime.h>
#import <dispatch/dispatch.h>
#import <CoreVideo/CoreVideo.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>
@interface D27Mode:NSObject
- (id)initWithWidth:(unsigned int)w height:(unsigned int)h refreshRate:(CGFloat)hz;
@end
@interface D27Settings:NSObject
@property(retain) NSArray *modes;
@property unsigned int hiDPI;
@end
@interface D27Descriptor:NSObject
@property(retain) dispatch_queue_t queue;
@property(retain) NSString *name;
@property unsigned int maxPixelsWide,maxPixelsHigh,vendorID,productID,serialNum;
@property CGSize sizeInMillimeters;
@end
@interface D27Display:NSObject
- (id)initWithDescriptor:(id)descriptor;
- (BOOL)applySettings:(id)settings;
@property(readonly) CGDirectDisplayID displayID;
@end
static void note(const char*stage,uint64_t value){fprintf(stderr,"D27 display bridge: %s value=%llu\n",stage,(unsigned long long)value);fflush(stderr);}
static const char *plain(const char*p){while(p&&strchr("rnNoORV",*p)&&*p)p++;return p;}
static BOOL method(Class c,const char*n,const char*result,unsigned args){
 Method m=c?class_getInstanceMethod(c,sel_registerName(n)):NULL;
 if(!m){note(n,0);return NO;}NSMethodSignature*s=[NSMethodSignature signatureWithObjCTypes:method_getTypeEncoding(m)];
 const char*r=plain(s.methodReturnType);BOOL ok=s.numberOfArguments==args&&(!strcmp(r,result)||(result[0]=='@'&&r[0]=='@'));
 if(!ok)note(n,s.numberOfArguments);return ok;
}
static BOOL check(Class c,const char*n,const char*result,NSArray<NSString*>*args){
 if(!method(c,n,result,(unsigned)args.count+2))return NO;
 Method m=class_getInstanceMethod(c,sel_registerName(n));NSMethodSignature*s=[NSMethodSignature signatureWithObjCTypes:method_getTypeEncoding(m)];
 for(NSUInteger i=0;i<args.count;i++){const char*t=plain([s getArgumentTypeAtIndex:i+2]);const char*expect=args[i].UTF8String;if((expect[0]=='@'&&t[0]=='@')||!strcmp(t,expect))continue;note(n,i+2);return NO;}return YES;
}
static BOOL abi(void){
 Class d=NSClassFromString(@"CGVirtualDisplayDescriptor"),v=NSClassFromString(@"CGVirtualDisplay"),s=NSClassFromString(@"CGVirtualDisplaySettings"),m=NSClassFromString(@"CGVirtualDisplayMode");
 return check(d,"setName:","v",@[@"@"])&&check(d,"setQueue:","v",@[@"@"])&&check(d,"setMaxPixelsWide:","v",@[@"I"])&&check(d,"setMaxPixelsHigh:","v",@[@"I"])&&check(d,"setSizeInMillimeters:","v",@[@"{CGSize=dd}"])&&check(d,"setVendorID:","v",@[@"I"])&&check(d,"setProductID:","v",@[@"I"])&&check(d,"setSerialNum:","v",@[@"I"])&&check(v,"initWithDescriptor:","@",@[@"@"])&&check(v,"applySettings:","B",@[@"@"])&&check(v,"displayID","I",@[])&&check(s,"setModes:","v",@[@"@"])&&check(s,"setHiDPI:","v",@[@"I"])&&check(m,"initWithWidth:height:refreshRate:","@",@[@"I",@"I",@"d"]);
}
static io_connect_t connection;
static mach_vm_address_t video;
static mach_vm_size_t videoBytes;
static uint64_t videoOffset,width,height,stride;
static BOOL map_video(void){
 io_service_t dev=IOServiceGetMatchingService(kIOMainPortDefault,IOServiceMatching("IOBootFramebuffer"));if(!dev){note("no native boot framebuffer",0);return NO;}
 kern_return_t k=IOServiceOpen(dev,mach_task_self(),0,&connection);IOObjectRelease(dev);if(k){note("native framebuffer server open failed",k);return NO;}
 uint64_t out[2]={0};uint32_t count=2;k=IOConnectCallScalarMethod(connection,2,NULL,0,out,&count);
 if(k||count!=2){note("native mode query failed",k);return NO;}
 uint64_t inputs[3]={out[0],out[1],kIOFBSystemAperture};IOPixelInformation pi={0};size_t bytes=sizeof(pi);
 k=IOConnectCallMethod(connection,1,inputs,3,NULL,0,NULL,NULL,&pi,&bytes);
 if(k||bytes!=sizeof(pi)||pi.bitsPerPixel!=32||pi.componentMasks[0]!=0xff0000||pi.componentMasks[1]!=0xff00||pi.componentMasks[2]!=0xff){note("unsupported native pixel layout",k);return NO;}
 width=pi.activeWidth;height=pi.activeHeight;stride=pi.bytesPerRow;
 if(!width||!height||width>8192||height>8192||stride<width*4||stride>64*1024*1024||height>64*1024*1024/stride){note("invalid native video geometry",stride);return NO;}
 uint64_t aperture=kIOFBSystemAperture;uint32_t one=1;k=IOConnectCallScalarMethod(connection,8,&aperture,1,&videoOffset,&one);
 if(k||one!=1){note("native VRAM offset failed",k);return NO;}
 // Apple IOFramebufferUserClient::clientMemoryForType, kIOFBVRAMMemory=110.
 k=IOConnectMapMemory64(connection,110,mach_task_self(),&video,&videoBytes,kIOMapAnywhere);
 uint64_t need=(height-1)*stride+width*4;
 if(k||!video||videoOffset>videoBytes||need>videoBytes-videoOffset){note("native VRAM mapping failed or short",k);return NO;}
 note("native framebuffer mapped",videoBytes);return YES;
}
static BOOL draw(IOSurfaceRef surface){
 if(!surface||IOSurfaceGetWidth(surface)!=width||IOSurfaceGetHeight(surface)!=height||IOSurfaceGetPixelFormat(surface)!=kCVPixelFormatType_32BGRA||IOSurfaceGetPlaneCount(surface)>1)return NO;
 size_t row=IOSurfaceGetBytesPerRow(surface),size=IOSurfaceGetAllocSize(surface);if(row<width*4||height>SIZE_MAX/row||(height-1)*row+width*4>size)return NO;
 IOReturn k=IOSurfaceLock(surface,kIOSurfaceLockReadOnly,NULL);if(k){note("surface producer synchronization failed",k);return NO;}
 const uint8_t*src=IOSurfaceGetBaseAddress(surface);if(!src){IOSurfaceUnlock(surface,kIOSurfaceLockReadOnly,NULL);return NO;}
 uint8_t*dst=(void*)(uintptr_t)(video+videoOffset);
 for(uint64_t y=0;y<height;y++)memcpy(dst+y*stride,src+y*row,width*4);
 atomic_thread_fence(memory_order_seq_cst);k=IOSurfaceUnlock(surface,kIOSurfaceLockReadOnly,NULL);return k==0;
}
// Recovery 26A428 does not contain ScreenCaptureKit. Resolve its existing
// CoreGraphics capture exports at runtime; refuse missing APIs or denied capture.
typedef CGDisplayStreamRef (*D27CreateStream)(CGDirectDisplayID,size_t,size_t,int32_t,CFDictionaryRef,dispatch_queue_t,void (^)(CGDisplayStreamFrameStatus,uint64_t,IOSurfaceRef,CGDisplayStreamUpdateRef));
typedef CGError (*D27StreamOperation)(CGDisplayStreamRef);
int main(int argc,char**argv){@autoreleasepool{
 if(argc==2&&!strcmp(argv[1],"--inspect")){BOOL ok=abi();note("read-only runtime ABI inspection",ok);return ok?0:2;}
 if(argc!=2||strcmp(argv[1],"--guest-firmware-bridge"))return 2;
 NSString*marker=[NSString stringWithContentsOfFile:@"/System/Library/CoreServices/dell27-firmware-bridge.enabled" encoding:NSUTF8StringEncoding error:NULL];
 if(![marker isEqualToString:@"D27-GUEST-DISPLAY-BRIDGE-V1\n"]){note("guest-only marker absent",0);return 2;}
 io_service_t block=IOServiceGetMatchingService(kIOMainPortDefault,IOServiceMatching("AppleVirtIOBlockStorageDevice"));if(!block){note("required guest VirtIO storage absent",0);return 2;}IOObjectRelease(block);
 if(!abi()||!map_video())return 2;
 dispatch_queue_t q=dispatch_queue_create("com.dell27.software-display",DISPATCH_QUEUE_SERIAL);
 D27Descriptor*descriptor=(id)[NSClassFromString(@"CGVirtualDisplayDescriptor") new];descriptor.name=@"Dell27 Firmware Software Display";descriptor.queue=q;descriptor.maxPixelsWide=(unsigned)width;descriptor.maxPixelsHigh=(unsigned)height;descriptor.vendorID=0xf0ff;descriptor.productID=27;descriptor.serialNum=1;descriptor.sizeInMillimeters=CGSizeMake(width*25.4/96,height*25.4/96);
 __attribute__((objc_precise_lifetime)) D27Display*display=(id)[[NSClassFromString(@"CGVirtualDisplay") alloc]initWithDescriptor:descriptor];if(!display){note("native virtual display rejected",0);return 2;}
 D27Mode*mode=(id)[[NSClassFromString(@"CGVirtualDisplayMode") alloc]initWithWidth:width height:height refreshRate:30.0];if(!mode){note("native virtual mode unavailable",0);return 2;}
 D27Settings*settings=(id)[NSClassFromString(@"CGVirtualDisplaySettings") new];settings.hiDPI=0;settings.modes=@[mode];
 if(![display applySettings:settings]||!display.displayID){note("native virtual mode rejected",0);return 2;}
 CGDirectDisplayID id=display.displayID;note("native virtual display created",id);
 CGDisplayConfigRef config=NULL;CGError cg=CGBeginDisplayConfiguration(&config);if(cg==kCGErrorSuccess){cg=CGConfigureDisplayOrigin(config,id,0,0);if(cg==kCGErrorSuccess)cg=CGCompleteDisplayConfiguration(config,kCGConfigureForSession);else CGCancelDisplayConfiguration(config);}if(cg)note("native display layout result",cg);
 D27CreateStream create=(D27CreateStream)dlsym(RTLD_DEFAULT,"CGDisplayStreamCreateWithDispatchQueue");
 D27StreamOperation start=(D27StreamOperation)dlsym(RTLD_DEFAULT,"CGDisplayStreamStart"),stop=(D27StreamOperation)dlsym(RTLD_DEFAULT,"CGDisplayStreamStop");
 CFStringRef*minimum=dlsym(RTLD_DEFAULT,"kCGDisplayStreamMinimumFrameTime"),*cursor=dlsym(RTLD_DEFAULT,"kCGDisplayStreamShowCursor");
 if(!create||!start||!stop||!minimum||!*minimum||!cursor||!*cursor){note("target recovery capture API unavailable",0);return 2;}
 NSDictionary*properties=@{(__bridge NSString*)*minimum:@(1.0/15.0),(__bridge NSString*)*cursor:@YES};
 __block uint64_t frames=0;
 CGDisplayStreamRef stream=create(id,width,height,kCVPixelFormatType_32BGRA,(__bridge CFDictionaryRef)properties,q,^(CGDisplayStreamFrameStatus status,uint64_t time,IOSurfaceRef surface,CGDisplayStreamUpdateRef update){
  (void)time;(void)update;if(status==kCGDisplayStreamFrameStatusFrameComplete&&draw(surface)){frames++;if(frames==1||frames%120==0)note("actual native surface copied to firmware aperture",frames);}
 });
 if(!stream){note("native capture creation rejected; no permission override",0);return 2;}
 cg=start(stream);if(cg){note("native capture start rejected",cg);CFRelease(stream);return 2;}note("software presentation running; no acceleration claimed",0);

 CFRunLoopRun();stop(stream);CFRelease(stream);(void)display;IOConnectUnmapMemory64(connection,110,mach_task_self(),video);IOServiceClose(connection);return 0;
}}
