#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <libkern/OSByteOrder.h>
#include <stdint.h>
#include <string.h>
static uint32_t rd32(const uint8_t*p){uint32_t v;memcpy(&v,p,4);return v;}
static BOOL slice(NSData*d,const uint8_t**out,uint64_t*size){
 if(!d||d.length<32)return NO;const uint8_t*p=d.bytes;uint64_t off=0,sz=d.length;uint32_t m=rd32(p);
 if(m==FAT_CIGAM||m==FAT_CIGAM_64){BOOL wide=m==FAT_CIGAM_64;uint32_t n=OSReadBigInt32(p,4);if(n>64||8ull+(wide?32:20)*n>d.length)return NO;BOOL found=NO;
  for(uint32_t i=0;i<n;i++){const uint8_t*a=p+8+(wide?32:20)*i;if(OSReadBigInt32(a,0)==CPU_TYPE_X86_64){off=wide?OSReadBigInt64(a,8):OSReadBigInt32(a,8);sz=wide?OSReadBigInt64(a,16):OSReadBigInt32(a,12);found=YES;break;}}if(!found||off>d.length||sz>d.length-off||sz<32)return NO;p+=off;
 }if(rd32(p)!=MH_MAGIC_64||rd32(p+4)!=CPU_TYPE_X86_64)return NO;*out=p;*size=sz;return YES;
}
static NSString*uuidBytes(const uint8_t*p,uint64_t sz){
 if(sz<32||rd32(p)!=MH_MAGIC_64||rd32(p+4)!=CPU_TYPE_X86_64)return nil;uint32_t n=rd32(p+16),bytes=rd32(p+20);if(n>10000||32ull+bytes>sz)return nil;uint64_t at=32;NSString*uuid=nil;
 for(uint32_t i=0;i<n;i++){if(at+8>32ull+bytes)return nil;uint32_t cmd=rd32(p+at),len=rd32(p+at+4);if(len<8||at+len>32ull+bytes)return nil;if(cmd==LC_UUID){if(len!=24||uuid)return nil;uuid=[[[NSUUID alloc]initWithUUIDBytes:p+at+8]UUIDString];}at+=len;}return at==32ull+bytes?uuid:nil;
}
static NSString*uuidAt(NSString*path){NSData*d=[NSData dataWithContentsOfFile:path options:NSDataReadingMappedIfSafe error:NULL];const uint8_t*p;uint64_t sz;return slice(d,&p,&sz)?uuidBytes(p,sz):nil;}
static NSDictionary*collectionUUIDs(NSString*root,BOOL*found){
 NSMutableDictionary*out=[NSMutableDictionary dictionary];*found=NO;
 for(NSString*name in @[@"BootKernelExtensions.kc",@"SystemKernelExtensions.kc",@"BaseSystemKernelExtensions.kc"]){NSString*path=[root stringByAppendingPathComponent:[@"System/Library/KernelCollections" stringByAppendingPathComponent:name]];if(![[NSFileManager defaultManager]fileExistsAtPath:path])continue;*found=YES;
  NSData*d=[NSData dataWithContentsOfFile:path options:NSDataReadingMappedIfSafe error:NULL];const uint8_t*p;uint64_t sz;if(!slice(d,&p,&sz)||rd32(p+12)!=MH_FILESET)@throw[NSException exceptionWithName:@"InvalidKernelCollection" reason:path userInfo:nil];uint32_t n=rd32(p+16),bytes=rd32(p+20);if(n>10000||32ull+bytes>sz)return nil;uint64_t at=32;NSMutableSet*ids=[NSMutableSet set];
  for(uint32_t i=0;i<n;i++){if(at+8>32ull+bytes)return nil;uint32_t cmd=rd32(p+at),len=rd32(p+at+4);if(len<8||at+len>32ull+bytes)return nil;
   if(cmd==LC_FILESET_ENTRY){if(len<32)return nil;uint64_t off;memcpy(&off,p+at+16,8);uint32_t str=rd32(p+at+24);if(str<32||str>=len||off<32ull+bytes||off>sz||sz-off<32)return nil;const char*text=(const char*)(p+at+str);const char*end=memchr(text,0,len-str);if(!end)return nil;NSString*idn=[[NSString alloc]initWithBytes:text length:end-text encoding:NSUTF8StringEncoding];if(!idn.length||[ids containsObject:idn])return nil;[ids addObject:idn];NSString*u=uuidBytes(p+off,sz-off);if(!u)return nil;if(out[idn]&&![out[idn]isEqual:u])return nil;out[idn]=u;
   }at+=len;
  }if(at!=32ull+bytes)return nil;
 }return out;
}
static uint32_t number(id x){if([x isKindOfClass:NSNumber.class])return [x unsignedIntValue];if([x isKindOfClass:NSData.class]&&[x length]>=2){const uint8_t*p=[x bytes];return p[0]|((uint32_t)p[1]<<8);}return UINT32_MAX;}
static int hardware(void){io_iterator_t it=0;BOOL found=NO;if(IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("IOPCIDevice"),&it))return 2;io_registry_entry_t e;
 while((e=IOIteratorNext(it))){CFMutableDictionaryRef props=NULL;if(IORegistryEntryCreateCFProperties(e,&props,kCFAllocatorDefault,0)==KERN_SUCCESS){NSDictionary*d=CFBridgingRelease(props);if(number(d[@"vendor-id"])==0x8086&&number(d[@"device-id"])==0x7d45&&number(d[@"subsystem-vendor-id"])==0x1028&&number(d[@"subsystem-id"])==0x0cb6)found=YES;}IOObjectRelease(e);}IOObjectRelease(it);
 if(found){puts("PASS saved Dell PCI identity 8086:7d45 /1028:0cb6");return 0;}fputs("Expected Dell Meteor Lake GPU not found; no changes made.\n",stderr);return 2;
}
static NSMutableDictionary*readPlist(NSString*p){NSData*d=[NSData dataWithContentsOfFile:p];id o=d?[NSPropertyListSerialization propertyListWithData:d options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL]:nil;return[o isKindOfClass:NSMutableDictionary.class]?o:nil;}
static BOOL savePlist(id d,NSString*p){NSData*b=[NSPropertyListSerialization dataWithPropertyList:d format:NSPropertyListXMLFormat_v1_0 options:0 error:NULL];return b&&[b writeToFile:p options:NSDataWritingAtomic error:NULL];}
int main(int argc,const char**argv){@autoreleasepool{@try{
 if(argc==3&&!strcmp(argv[1],"--uuid")){NSString*u=uuidAt(@(argv[2]));if(!u)return 2;puts(u.UTF8String);return 0;}
 if(argc==2&&!strcmp(argv[1],"--hardware"))return hardware();
 if(argc==5&&!strcmp(argv[1],"--families")){NSDictionary*m=readPlist(@(argv[2]));NSArray*a=m[@(argv[3])];if(![a isKindOfClass:NSArray.class]||!a.count){fputs("Unqualified Tahoe build; no files changed.\n",stderr);return 2;}NSString*root=@(argv[4]);
  BOOL hasCollections=NO;NSDictionary*kc=collectionUUIDs(root,&hasCollections);if(!kc){fputs("Malformed or conflicting kernel-collection identities; no files changed.\n",stderr);return 2;}
  for(NSDictionary*r in a){NSString*u=nil;if(hasCollections)u=kc[r[@"id"]];else for(NSString*p in r[@"paths"]){NSString*c=uuidAt([root stringByAppendingPathComponent:p]);if(c){u=c;break;}}
   if(!u||![u isEqual:r[@"uuid"]]){fprintf(stderr,"ABI identity mismatch: %s (found %s; source %s)\n",[r[@"id"] UTF8String],u?u.UTF8String:"missing",hasCollections?"kernel collections":"standalone files");return 2;}printf("PASS %s UUID=%s source=%s\n",[r[@"id"]UTF8String],u.UTF8String,hasCollections?"kernel collections":"standalone file");}
  return 0;
 }
 if(argc==6&&!strcmp(argv[1],"--config")){NSMutableDictionary*d=readPlist(@(argv[2]));NSString*mode=@(argv[4]);if(!d||![mode isEqual:@"accelerated"]&&![mode isEqual:@"recovery"])return 2;
  BOOL active=[mode isEqual:@"accelerated"];BOOL panel=NO;
  for(NSMutableDictionary*k in d[@"Kernel"][@"Add"]){if([k[@"BundlePath"] isEqual:@"LatitudePanelClaim.kext"]){k[@"MinKernel"]=@"24.0.0";k[@"MaxKernel"]=@"25.99.99";k[@"Enabled"]=@(active);k[@"Comment"]=@"Latitude panel reservation; qualified Sequoia and Tahoe runtime";panel=YES;}if([k[@"BundlePath"] isEqual:@"LatitudeMTLFB.kext"])k[@"Enabled"]=@NO;}
  if(!panel)return 2;NSString*guid=@"7C436110-AB2A-4BBB-A880-FE41995C9F82";NSMutableDictionary*add=d[@"NVRAM"][@"Add"][guid];NSMutableArray*del=d[@"NVRAM"][@"Delete"][guid];if(!add||!del)return 2;
  for(NSString*k in @[@"nullmoth-gpu-once",@"nullmoth-panel-once"]){if(![del containsObject:k])[del addObject:k];if(active)add[k]=@"1";else[add removeObjectForKey:k];}
  // argv5 is the frozen source-config digest, recorded by the caller; no unguarded overwrite here.
  if(strlen(argv[5])!=64||!savePlist(d,@(argv[3])))return 2;return 0;
 }
 if(argc==4&&!strcmp(argv[1],"--coremedia")){NSString*in=@(argv[2]);NSMutableDictionary*d=[[NSFileManager defaultManager]fileExistsAtPath:in]?readPlist(in):[NSMutableDictionary dictionary];if(!d)return 2;
  id v=d[@"AllowOnePassMetalScaling"];if(v&&![v isKindOfClass:NSDictionary.class])return 2;
  NSMutableDictionary*a=v?[v mutableCopy]:[NSMutableDictionary dictionary];a[@"Enabled"]=@NO;d[@"AllowOnePassMetalScaling"]=a;return savePlist(d,@(argv[3]))?0:2;
 }
 fputs("Usage: --uuid binary | --hardware | --families plist build root | --config in out accelerated|recovery source-sha256 | --coremedia in out\n",stderr);return 2;
 }@catch(NSException*e){fprintf(stderr,"Invalid input: %s\n",e.name.UTF8String);return 2;}}
}
