// Read-only inventory; no ACPI evaluation, input suppression, feature writes or rotation.
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <ApplicationServices/ApplicationServices.h>
#include <sys/sysctl.h>
static id prop(io_registry_entry_t s,NSString*k){return CFBridgingRelease(IORegistryEntryCreateCFProperty(s,(__bridge CFStringRef)k,kCFAllocatorDefault,0));}
static NSString* path(io_registry_entry_t s,const char*plane){io_string_t p;return !IORegistryEntryGetPath(s,plane,p)?[NSString stringWithUTF8String:p]:@"";}
static NSDictionary* item(io_registry_entry_t s,const char*plane){
 NSMutableDictionary*d=[@{@"path":path(s,plane)} mutableCopy];
 for(NSString*k in @[@"name",@"compatible",@"IOName",@"_HID",@"PrimaryUsagePage",@"PrimaryUsage",@"Product",@"Transport",@"VendorID",@"ProductID",@"IOClass",@"IOProviderClass",@"LatitudePanelOptedIn"]){id v=prop(s,k);if([v isKindOfClass:[NSData class]])v=[v description];if(v&&[NSJSONSerialization isValidJSONObject:@[v]])d[k]=v;}
 return d;
}
int main(){@autoreleasepool{
 int arm=0;size_t len=sizeof arm;sysctlbyname("hw.optional.arm64",&arm,&len,NULL,0);if(arm){fprintf(stderr,"Run on the Intel Dell.\n");return 2;}
 NSMutableArray*acpi=[NSMutableArray array],*hid=[NSMutableArray array],*sensors=[NSMutableArray array],*displays=[NSMutableArray array];
 io_registry_entry_t root=IORegistryGetRootEntry(kIOMainPortDefault);io_iterator_t it=0;io_registry_entry_t s;
 kern_return_t result=IORegistryEntryCreateIterator(root,"IOACPIPlane",kIORegistryIterateRecursively,&it);IOObjectRelease(root);
 if(!result){while((s=IOIteratorNext(it))){NSDictionary*d=item(s,"IOACPIPlane");NSString*desc=[d description];BOOL match=NO;for(NSString*needle in @[@"HIDD",@"ISHD",@"ISHC",@"BIND",@"CIND",@"DIND",@"INTC1077",@"INT33D2",@"INT33D3",@"INT33D4"])if([desc rangeOfString:needle].location!=NSNotFound)match=YES;if(match)[acpi addObject:d];IOObjectRelease(s);}IOObjectRelease(it);}
 kern_return_t hidResult=IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("IOHIDDevice"),&it);
 if(!hidResult){while((s=IOIteratorNext(it))){NSDictionary*d=item(s,kIOServicePlane);[hid addObject:d];BOOL sensor=[d[@"PrimaryUsagePage"] unsignedIntValue]==0x20;id pairs=prop(s,@"DeviceUsagePairs");if([pairs isKindOfClass:[NSArray class]])for(id pair in pairs)if([pair isKindOfClass:[NSDictionary class]]&&[pair[@"DeviceUsagePage"] unsignedIntValue]==0x20)sensor=YES;if(sensor)[sensors addObject:d];IOObjectRelease(s);}IOObjectRelease(it);}
 CGDirectDisplayID ids[32];uint32_t count=0;CGError displayResult=CGGetOnlineDisplayList(32,ids,&count);
 if(!displayResult)for(uint32_t i=0;i<count;i++)[displays addObject:@{@"displayID":@(ids[i]),@"builtIn":@(CGDisplayIsBuiltin(ids[i])),@"rotation":@(CGDisplayRotation(ids[i])),@"width":@(CGDisplayPixelsWide(ids[i])),@"height":@(CGDisplayPixelsHigh(ids[i]))}];
 NSDictionary*report=@{@"version":@"0.1.0",@"acpiQueryResult":@(result),@"hidQueryResult":@(hidResult),@"displayQueryResult":@(displayResult),@"acpiDevices":acpi,@"hidDevices":hid,@"sensorDevices":sensors,@"displays":displays,@"tabletState":@"unknown: no firmware method evaluated",@"automaticRotationReady":@NO};
 NSData*json=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];if(!json)return 1;fwrite(json.bytes,1,json.length,stdout);puts("");return result||hidResult||displayResult?1:0;
}}
