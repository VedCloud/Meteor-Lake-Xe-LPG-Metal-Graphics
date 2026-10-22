#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#include <math.h>
int main(void){@autoreleasepool{
 NSMutableDictionary *d=[NSMutableDictionary dictionaryWithDictionary:@{@"measurement":@"Battery telemetry snapshot; not CPU package power or a runtime benchmark",@"time":@([[NSDate date] timeIntervalSince1970])}];
 io_service_t s=IOServiceGetMatchingService(kIOMainPortDefault,IOServiceMatching("AppleSmartBattery"));
 if(s){CFMutableDictionaryRef props=NULL;IOReturn rc=IORegistryEntryCreateCFProperties(s,&props,kCFAllocatorDefault,0);IOObjectRelease(s);
  if(!rc&&props){NSDictionary *p=CFBridgingRelease(props);NSMutableDictionary *battery=[NSMutableDictionary dictionary];
   for(NSString *key in @[@"Voltage",@"Amperage",@"InstantAmperage",@"IsCharging",@"ExternalConnected",@"CurrentCapacity",@"MaxCapacity",@"DesignCapacity",@"CycleCount",@"Temperature",@"AppleRawCurrentCapacity",@"AppleRawMaxCapacity"]){id v=p[key];if([v isKindOfClass:[NSNumber class]])battery[key]=v;}
   d[@"battery"]=battery;NSNumber *voltage=battery[@"Voltage"],*amp=battery[@"Amperage"];
   if(voltage&&amp){int64_t ma=amp.longLongValue;if(ma>INT32_MAX&&ma<=UINT32_MAX)ma=(int32_t)(uint32_t)ma;
    double mv=voltage.doubleValue;if(mv>1000&&mv<30000&&llabs(ma)<50000){d[@"signed_current_ma"]=@(ma);
     if(![battery[@"ExternalConnected"] boolValue]&&![battery[@"IsCharging"] boolValue])d[@"discharge_power_w"]=@(fabs((double)ma)*mv/1e6);
    }
   }
  }else d[@"status"]=@"battery_query_failed";
 }else d[@"status"]=@"battery_service_missing";
 NSData *json=[NSJSONSerialization dataWithJSONObject:d options:NSJSONWritingPrettyPrinted error:nil];if(!json)return 1;fwrite(json.bytes,1,json.length,stdout);puts("");return 0;
}}
