// Brightness keys: system DisplayServices state and native OSD, no MMIO or key logging.
#import <AppKit/AppKit.h>
#import <IOKit/graphics/IOGraphicsLib.h>
#import <IOKit/IOKitLib.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h>
#include <objc/message.h>
#include "latitude_brightness_policy.h"
static CFMachPortRef tap;
static io_service_t panel(void){
 io_iterator_t it=0;io_service_t found=0,s;unsigned matches=0;
 if(IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("AppleBacklightDisplay"),&it))return 0;
 while((s=IOIteratorNext(it))){io_string_t path;
  if(!IORegistryEntryGetPath(s,kIOServicePlane,path)&&strstr(path,"/LatitudePanelClaim/")&&strstr(path,"/NMXIntelVGAFB/")){matches++;if(!found){found=s;continue;}}
  IOObjectRelease(s);
 }
 IOObjectRelease(it);if(matches!=1){if(found)IOObjectRelease(found);return 0;}return found;
}
@protocol LatitudeOSD
- (void)showImage:(long long)image onDisplayID:(CGDirectDisplayID)display priority:(unsigned)priority msecUntilFade:(unsigned)milliseconds filledChiclets:(unsigned)filled totalChiclets:(unsigned)total locked:(BOOL)locked;
@end
static int (*getBrightness)(CGDirectDisplayID,float*);
static int (*setBrightness)(CGDirectDisplayID,float);
static bool (*canBrightness)(CGDirectDisplayID);
static void (*brightnessChanged)(CGDirectDisplayID,double);
static void loadServices(void){
 static dispatch_once_t once;dispatch_once(&once,^{
  void *h=dlopen("/System/Library/PrivateFrameworks/DisplayServices.framework/DisplayServices",RTLD_LAZY|RTLD_LOCAL);
  if(h){getBrightness=dlsym(h,"DisplayServicesGetBrightness");setBrightness=dlsym(h,"DisplayServicesSetBrightness");canBrightness=dlsym(h,"DisplayServicesCanChangeBrightness");brightnessChanged=dlsym(h,"DisplayServicesBrightnessChanged");}
 });
}
static CGDirectDisplayID panelID(io_service_t service){
 NSNumber *vendor=CFBridgingRelease(IORegistryEntryCreateCFProperty(service,CFSTR("DisplayVendorID"),kCFAllocatorDefault,0));
 NSNumber *model=CFBridgingRelease(IORegistryEntryCreateCFProperty(service,CFSTR("DisplayProductID"),kCFAllocatorDefault,0));
 CGDirectDisplayID ids[16],found=0;uint32_t count=0,matches=0;
 if(![vendor isKindOfClass:[NSNumber class]]||![model isKindOfClass:[NSNumber class]]||CGGetOnlineDisplayList(16,ids,&count)!=kCGErrorSuccess)return 0;
 for(uint32_t i=0;i<count;i++)if(CGDisplayVendorNumber(ids[i])==vendor.unsignedIntValue&&CGDisplayModelNumber(ids[i])==model.unsignedIntValue){found=ids[i];matches++;}
 return matches==1?found:0;
}
static void showBrightness(CGDirectDisplayID display,float value){
 static dispatch_once_t once;dispatch_once(&once,^{(void)dlopen("/System/Library/PrivateFrameworks/OSD.framework/OSD",RTLD_LAZY|RTLD_LOCAL);});
 Class cls=NSClassFromString(@"OSDManager");SEL shared=NSSelectorFromString(@"sharedManager");
 if(!cls||![cls respondsToSelector:shared]){fprintf(stderr,"Native brightness OSD service unavailable.\n");return;}
 id manager=((id(*)(id,SEL))objc_msgSend)((id)cls,shared);
 SEL selector=@selector(showImage:onDisplayID:priority:msecUntilFade:filledChiclets:totalChiclets:locked:);
 if([manager respondsToSelector:selector])[(id<LatitudeOSD>)manager showImage:1 onDisplayID:display priority:0x1f4 msecUntilFade:1000 filledChiclets:(unsigned)lroundf(value*16) totalChiclets:16 locked:NO];
 else fprintf(stderr,"Native brightness OSD selector unavailable.\n");
}
static int diagnose(void){
 loadServices();io_service_t s=panel();CGDirectDisplayID display=s?panelID(s):0;float systemValue=-1,linear=-1,brightness=-1;
 int result=display&&getBrightness?getBrightness(display,&systemValue):-1;
 if(s){IODisplayGetFloatParameter(s,0,CFSTR("linear-brightness"),&linear);IODisplayGetFloatParameter(s,0,CFSTR("brightness"),&brightness);IOObjectRelease(s);}
 NSDictionary *d=@{@"version":@"0.2.0",@"displayID":@(display),@"DisplayServicesAvailable":@(getBrightness&&setBrightness),@"systemQueryResult":@(result),@"systemBrightness":@(systemValue),@"linearBrightness":@(linear),@"displayBrightness":@(brightness),@"accessibilityTrusted":@(AXIsProcessTrusted())};
 NSData *json=[NSJSONSerialization dataWithJSONObject:d options:NSJSONWritingPrettyPrinted error:nil];fwrite(json.bytes,1,json.length,stdout);puts("");return result?1:0;
}
static int step(int dir){
 loadServices();io_service_t s=panel();if(!s){fprintf(stderr,"Latitude panel unavailable; accelerated framebuffer must be active.\n");return 1;}
 CGDirectDisplayID display=panelID(s);IOObjectRelease(s);
 if(!display||!getBrightness||!setBrightness||(canBrightness&&!canBrightness(display))){fprintf(stderr,"System brightness API unavailable for the Latitude display; no change.\n");return 1;}
 float value=0;int result=getBrightness(display,&value);
 if(!result&&isfinite(value)&&value>=0&&value<=1){
  value=latitudeBrightnessStep(value,dir);result=setBrightness(display,value);
  if(!result){if(brightnessChanged)brightnessChanged(display,value);showBrightness(display,value);}
 }else if(!result)result=kIOReturnBadArgument;
 if(result){fprintf(stderr,"System brightness API failed: 0x%08x\n",result);return 1;}return 0;
}

static CGEventRef onEvent(CGEventTapProxy proxy,CGEventType type,CGEventRef event,void *ctx){
 (void)proxy;(void)ctx;
 if(type==kCGEventTapDisabledByTimeout||type==kCGEventTapDisabledByUserInput){if(tap)CGEventTapEnable(tap,true);return event;}
 int direction=0;
 if(type==NX_SYSDEFINED){NSEvent *e=[NSEvent eventWithCGEvent:event];if(e&&e.subtype==8){unsigned code=((unsigned)e.data1>>16)&0xffff;unsigned state=((unsigned)e.data1>>8)&0xff;
   direction=latitudeBrightnessSystem((unsigned)e.subtype,code,state);
 }}else if(type==kCGEventKeyDown){
  CGEventFlags mods=CGEventGetFlags(event)&(kCGEventFlagMaskCommand|kCGEventFlagMaskAlternate|kCGEventFlagMaskControl|kCGEventFlagMaskShift);
  unsigned code=(unsigned)CGEventGetIntegerValueField(event,kCGKeyboardEventKeycode);
  // F6/F7, or Control+Option+F6/F7 on an external keyboard. Fn flag is permitted.
  direction=latitudeBrightnessKey(code,mods);
 }
 if(direction&&step(direction)==0)return NULL;return event;
}
int main(int argc,const char **argv){@autoreleasepool{
 if(argc==2&&!strcmp(argv[1],"diagnose"))return diagnose();
 if(argc==2&&(!strcmp(argv[1],"+")||!strcmp(argv[1],"-")))return step(argv[1][0]=='+'?1:-1);
 if(argc!=2||strcmp(argv[1],"daemon")){fprintf(stderr,"Usage: latitude-brightness +|-|daemon|diagnose\n");return 2;}
 // One login-session event tap. Explicit user-granted Accessibility permission is required.
 if(!AXIsProcessTrusted()){fprintf(stderr,"Enable Accessibility for latitude-brightness, then start again. No permission request was made.\n");return 3;}
 tap=CGEventTapCreate(kCGSessionEventTap,kCGHeadInsertEventTap,kCGEventTapOptionDefault,CGEventMaskBit(NX_SYSDEFINED)|CGEventMaskBit(kCGEventKeyDown),onEvent,NULL);
 if(!tap){fprintf(stderr,"Unable to create brightness event tap.\n");return 3;}
 CFRunLoopSourceRef source=CFMachPortCreateRunLoopSource(NULL,tap,0);if(!source){CFRelease(tap);return 1;}
 CFRunLoopAddSource(CFRunLoopGetCurrent(),source,kCFRunLoopCommonModes);CGEventTapEnable(tap,true);CFRunLoopRun();CFRelease(source);CFRelease(tap);return 0;
}}
