#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include <unistd.h>
int main(){@autoreleasepool{
 if(geteuid()!=0){fprintf(stderr,"Counter snapshot requires root.\n");return 2;}
 io_iterator_t iter=0;kern_return_t rc=IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("NMXIntelVGAFB"),&iter);
 if(rc!=KERN_SUCCESS)return 2;io_service_t fb=IOIteratorNext(iter),extra=IOIteratorNext(iter);IOObjectRelease(iter);
 if(!fb||extra){if(fb)IOObjectRelease(fb);if(extra)IOObjectRelease(extra);fprintf(stderr,"Expected one Latitude framebuffer; no snapshot requested.\n");return 2;}
 rc=IORegistryEntrySetCFProperties(fb,(__bridge CFTypeRef)@{@"LatitudeReadHotkeyCounters":@YES});
 CFTypeRef value=IORegistryEntryCreateCFProperty(fb,CFSTR("LatitudeHotkeyCounters"),kCFAllocatorDefault,0);
 NSMutableDictionary *report=[@{@"requestResult":@(rc),@"scope":@"Read ACPI wrapper counters only; no key injection, hardware I/O or settings changes"} mutableCopy];
 if(value&&CFGetTypeID(value)==CFDictionaryGetTypeID())report[@"counters"]=(__bridge NSDictionary *)value;
 NSData *json=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:NULL];if(json){fwrite(json.bytes,1,json.length,stdout);puts("");}
 if(value)CFRelease(value);IOObjectRelease(fb);return rc==KERN_SUCCESS?0:3;
}}
