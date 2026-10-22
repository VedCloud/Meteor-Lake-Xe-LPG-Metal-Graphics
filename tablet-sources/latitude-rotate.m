// Explicit manual rotation validation, not an auto-rotation daemon.
#import <Foundation/Foundation.h>
#import <ApplicationServices/ApplicationServices.h>
#import <IOKit/IOKitLib.h>
#include <sys/sysctl.h>
#include <unistd.h>
static id value(io_service_t s,CFStringRef k){return CFBridgingRelease(IORegistryEntryCreateCFProperty(s,k,kCFAllocatorDefault,0));}
static unsigned flagsFor(unsigned angle){switch(angle){case 90:return 0x30;case 180:return 0x60;case 270:return 0x50;default:return 0;}}
static BOOL acknowledged(io_service_t s,CGDirectDisplayID display,unsigned angle){id n=value(s,CFSTR("IOFBTransform"));return [n isKindOfClass:NSNumber.class]&&(([n unsignedLongLongValue]&0xf0)==flagsFor(angle))&&fabs(CGDisplayRotation(display)-angle)<0.1;}
static BOOL waitFor(io_service_t s,CGDirectDisplayID display,unsigned angle){for(unsigned i=0;i<40;i++){if(acknowledged(s,display,angle))return YES;usleep(100000);}return NO;}
int main(int argc,const char*argv[]){@autoreleasepool{
 int arm=0;size_t len=sizeof arm;sysctlbyname("hw.optional.arm64",&arm,&len,NULL,0);if(arm){fprintf(stderr,"Run on the Intel Dell.\n");return 2;}
 BOOL diagnose=argc==2&&!strcmp(argv[1],"--diagnose");unsigned angle=0;
 if(!diagnose){if(argc!=3||strcmp(argv[1],"--rotate")||(strcmp(argv[2],"0")&&strcmp(argv[2],"90")&&strcmp(argv[2],"180")&&strcmp(argv[2],"270"))){fprintf(stderr,"Usage: latitude-rotate --diagnose | --rotate 0|90|180|270\n");return 2;}angle=(unsigned)atoi(argv[2]);}
 CGDirectDisplayID ids[32],display=0;uint32_t count=0,builtIn=0;
 if(CGGetOnlineDisplayList(32,ids,&count)){fprintf(stderr,"Display enumeration failed.\n");return 2;}
 for(unsigned i=0;i<count;i++)if(CGDisplayIsBuiltin(ids[i])){display=ids[i];builtIn++;}
 io_iterator_t iterator=0;io_service_t service=0,found=0;unsigned framebuffers=0;
 if(!IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("NMXVGAFB"),&iterator)){
  while((found=IOIteratorNext(iterator))){id validated=value(found,CFSTR("LatitudePanelValidated"));if([validated respondsToSelector:@selector(boolValue)]&&[validated boolValue]){framebuffers++;if(!service){service=found;continue;}}IOObjectRelease(found);}IOObjectRelease(iterator);
 }
 if(builtIn!=1||framebuffers!=1){if(service)IOObjectRelease(service);fprintf(stderr,"Expected one built-in display and one validated Latitude framebuffer; stopped.\n");return 2;}
 double before=CGDisplayRotation(display);unsigned original=(unsigned)llround(before);
 NSMutableDictionary*report=[@{@"version":@"0.1.0",@"displayID":@(display),@"beforeDegrees":@(before),@"width":@(CGDisplayPixelsWide(display)),@"height":@(CGDisplayPixelsHigh(display)),@"touchDriverAlreadyUsesIOFBTransform":@YES,@"automaticRotationEnabled":@NO} mutableCopy];
 id transform=value(service,CFSTR("IOFBTransform"));if(transform)report[@"beforeTransform"]=transform;
 int result=0;
 if(!diagnose){
  if(original!=0&&original!=90&&original!=180&&original!=270){IOObjectRelease(service);fprintf(stderr,"Unknown starting rotation; stopped.\n");return 2;}
  fprintf(stderr,"Manual rotation validation: lift all fingers/pen before running. No automatic input suppression.\n");
  IOReturn rc=IOServiceRequestProbe(service,0x400|(flagsFor(angle)<<16));report[@"requestResult"]=@((uint32_t)rc);report[@"requestedDegrees"]=@(angle);
  BOOL accepted=rc==kIOReturnSuccess&&waitFor(service,display,angle);report[@"acknowledged"]=@(accepted);
  if(!accepted){
   result=1;
   if(!acknowledged(service,display,original)){IOReturn back=IOServiceRequestProbe(service,0x400|(flagsFor(original)<<16));report[@"rollbackRequestResult"]=@((uint32_t)back);report[@"rollbackAcknowledged"]=@(back==kIOReturnSuccess&&waitFor(service,display,original));}
  }
  report[@"afterDegrees"]=@(CGDisplayRotation(display));id after=value(service,CFSTR("IOFBTransform"));if(after)report[@"afterTransform"]=after;
 }
 IOObjectRelease(service);NSData*data=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];if(!data)return 1;fwrite(data.bytes,1,data.length,stdout);puts("");return result;
}}
