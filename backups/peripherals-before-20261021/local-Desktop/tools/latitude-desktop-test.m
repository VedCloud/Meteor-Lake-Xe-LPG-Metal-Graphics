#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOKit/IOKitLib.h>
#include <math.h>
int main(void) { @autoreleasepool {
 setbuf(stdout,NULL);
 CGDirectDisplayID displays[16];uint32_t count=0;CGError rc=CGGetOnlineDisplayList(16,displays,&count);
 printf("Online displays rc=%d count=%u\n",rc,count);
 for(uint32_t i=0;i<count;i++) {
  id<MTLDevice> gpu=CGDirectDisplayCopyCurrentMetalDevice(displays[i]);
  io_service_t s=gpu?IOServiceGetMatchingService(kIOMainPortDefault,IORegistryEntryIDMatching(gpu.registryID)):0;
  BOOL ours=s&&IOObjectConformsTo(s,"NMXAccel");
  printf("Display %u %zux%zu Metal=%s registry=0x%llx NMXAccel=%s\n",displays[i],CGDisplayPixelsWide(displays[i]),CGDisplayPixelsHigh(displays[i]),gpu?gpu.name.UTF8String:"none",gpu?(unsigned long long)gpu.registryID:0,ours?"yes":"no");
  if(s)IOObjectRelease(s);
 }
 [NSApplication sharedApplication];[NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
 NSScreen *screen=NSScreen.mainScreen;if(!screen){fprintf(stderr,"No desktop screen\n");return 1;}
 NSRect area=screen.visibleFrame;NSPoint base=NSMakePoint(NSMidX(area)-180,NSMidY(area)-100);
 NSWindow *window=[[NSWindow alloc] initWithContentRect:NSMakeRect(base.x,base.y,360,200) styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:NO];
 window.title=@"Latitude desktop composition test";[window setReleasedWhenClosed:NO];[window orderFrontRegardless];
 NSTextField *label=[NSTextField labelWithString:@"Checking display presentation for 6 seconds"];
 label.frame=NSMakeRect(20,80,330,30);[window.contentView addSubview:label];
 __block unsigned frames=0;
 [NSTimer scheduledTimerWithTimeInterval:1.0/30.0 repeats:YES block:^(NSTimer *timer){
  frames++;[window setFrameOrigin:NSMakePoint(base.x+80*sin(frames*0.12),base.y+40*cos(frames*0.1))];
  window.backgroundColor=[NSColor colorWithHue:(frames%180)/180.0 saturation:0.45 brightness:0.85 alpha:1];
  if(frames>=180){[timer invalidate];[window orderOut:nil];[NSApp stop:nil];}
 }];
 [NSApp run];printf("Desktop stimulus complete: %u window updates. This alone does not prove GPU composition.\n",frames);
 return 0;
} }
