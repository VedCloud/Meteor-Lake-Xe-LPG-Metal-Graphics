// Brightness fallback: existing IODisplay API only, no MMIO or key logging.
#import <AppKit/AppKit.h>
#import <IOKit/graphics/IOGraphicsLib.h>
#import <IOKit/IOKitLib.h>
#include <math.h>
#include <string.h>
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
static int step(int dir){
 io_service_t s=panel();if(!s){fprintf(stderr,"Latitude panel unavailable; accelerated framebuffer must be active.\n");return 1;}
 float value=0;IOReturn rc=IODisplayGetFloatParameter(s,0,CFSTR("linear-brightness"),&value);
 if(!rc&&isfinite(value)&&value>=0&&value<=1){value=latitudeBrightnessStep(value,dir);rc=IODisplaySetFloatParameter(s,0,CFSTR("linear-brightness"),value);}
 else if(!rc)rc=kIOReturnBadArgument;
 IOObjectRelease(s);if(rc){fprintf(stderr,"Brightness API failed: 0x%08x\n",rc);return 1;}return 0;
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
 if(argc==2&&(!strcmp(argv[1],"+")||!strcmp(argv[1],"-")))return step(argv[1][0]=='+'?1:-1);
 if(argc!=2||strcmp(argv[1],"daemon")){fprintf(stderr,"Usage: latitude-brightness +|-|daemon\n");return 2;}
 // One login-session event tap. Explicit user-granted Accessibility permission is required.
 if(!AXIsProcessTrusted()){fprintf(stderr,"Enable Accessibility for latitude-brightness, then start again. No permission request was made.\n");return 3;}
 tap=CGEventTapCreate(kCGSessionEventTap,kCGHeadInsertEventTap,kCGEventTapOptionDefault,CGEventMaskBit(NX_SYSDEFINED)|CGEventMaskBit(kCGEventKeyDown),onEvent,NULL);
 if(!tap){fprintf(stderr,"Unable to create brightness event tap.\n");return 3;}
 CFRunLoopSourceRef source=CFMachPortCreateRunLoopSource(NULL,tap,0);if(!source){CFRelease(tap);return 1;}
 CFRunLoopAddSource(CFRunLoopGetCurrent(),source,kCFRunLoopCommonModes);CGEventTapEnable(tap,true);CFRunLoopRun();CFRelease(source);CFRelease(tap);return 0;
}}
