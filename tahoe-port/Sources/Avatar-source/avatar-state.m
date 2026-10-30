#import <Foundation/Foundation.h>
#import <OpenDirectory/OpenDirectory.h>
#import <ImageIO/ImageIO.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOKit/IOKitLib.h>
#include <unistd.h>
#include <pwd.h>
static void output(NSDictionary *d){NSData*j=[NSJSONSerialization dataWithJSONObject:d options:NSJSONWritingPrettyPrinted error:nil];fwrite(j.bytes,1,j.length,stdout);puts("");}
static NSData* jpeg(NSData*data){
 if(!data.length)return nil;
 CGImageSourceRef src=CGImageSourceCreateWithData((__bridge CFDataRef)data,NULL);if(!src)return nil;
 CGImageRef img=CGImageSourceCreateThumbnailAtIndex(src,0,(__bridge CFDictionaryRef)@{(__bridge NSString*)kCGImageSourceCreateThumbnailFromImageAlways:@YES,(__bridge NSString*)kCGImageSourceCreateThumbnailWithTransform:@YES,(__bridge NSString*)kCGImageSourceThumbnailMaxPixelSize:@512});CFRelease(src);if(!img)return nil;
 size_t w=CGImageGetWidth(img),h=CGImageGetHeight(img);if(!w||!h||w>512||h>512){CGImageRelease(img);return nil;}
 CGColorSpaceRef cs=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
 CGContextRef ctx=CGBitmapContextCreate(NULL,w,h,8,w*4,cs,kCGImageAlphaNoneSkipLast|kCGBitmapByteOrder32Big);CGColorSpaceRelease(cs);if(!ctx){CGImageRelease(img);return nil;}
 CGContextSetRGBFillColor(ctx,1,1,1,1);CGContextFillRect(ctx,CGRectMake(0,0,w,h));CGContextDrawImage(ctx,CGRectMake(0,0,w,h),img);CGImageRelease(img);CGImageRef normalized=CGBitmapContextCreateImage(ctx);CGContextRelease(ctx);if(!normalized)return nil;
 NSMutableData*out=[NSMutableData data];CGImageDestinationRef dst=CGImageDestinationCreateWithData((__bridge CFMutableDataRef)out,CFSTR("public.jpeg"),1,NULL);if(!dst){CGImageRelease(normalized);return nil;}
 CGImageDestinationAddImage(dst,normalized,(__bridge CFDictionaryRef)@{(__bridge NSString*)kCGImageDestinationLossyCompressionQuality:@0.92});BOOL ok=CGImageDestinationFinalize(dst);CFRelease(dst);CGImageRelease(normalized);return ok?out:nil;
}
static BOOL targetDell(void){io_iterator_t it=0;if(IOServiceGetMatchingServices(kIOMainPortDefault,IOServiceMatching("IOPCIDevice"),&it))return NO;BOOL found=NO;io_service_t s;while((s=IOIteratorNext(it))){CFTypeRef v=IORegistryEntryCreateCFProperty(s,CFSTR("vendor-id"),kCFAllocatorDefault,0),d=IORegistryEntryCreateCFProperty(s,CFSTR("device-id"),kCFAllocatorDefault,0);if(v&&d&&CFGetTypeID(v)==CFDataGetTypeID()&&CFGetTypeID(d)==CFDataGetTypeID()&&CFDataGetLength(v)>=2&&CFDataGetLength(d)>=2){const UInt8*a=CFDataGetBytePtr(v),*b=CFDataGetBytePtr(d);found|=(a[0]==0x86&&a[1]==0x80&&b[0]==0x45&&b[1]==0x7d);}if(v)CFRelease(v);if(d)CFRelease(d);IOObjectRelease(s);}IOObjectRelease(it);return found;}
static BOOL put(ODRecord*r,NSString*a,NSArray*v,NSError**e){return v.count?[r setValue:v forAttribute:a error:e]:[r removeValuesForAttribute:a error:e];}
int main(int argc,const char*argv[]){@autoreleasepool{
 NSString*arg=argc>1?@(argv[1]):@"--inspect";
 if([arg isEqual:@"--self-test"]){unsigned char pixels[16]={255,0,0,255,0,255,0,255,0,0,255,255,255,255,255,255};CGColorSpaceRef cs=CGColorSpaceCreateDeviceRGB();CGContextRef c=CGBitmapContextCreate(pixels,2,2,8,8,cs,(CGBitmapInfo)kCGImageAlphaNoneSkipLast);CGImageRef i=CGBitmapContextCreateImage(c);NSMutableData*png=[NSMutableData data];CGImageDestinationRef d=CGImageDestinationCreateWithData((__bridge CFMutableDataRef)png,CFSTR("public.png"),1,NULL);CGImageDestinationAddImage(d,i,NULL);BOOL ok=CGImageDestinationFinalize(d);CFRelease(d);CGImageRelease(i);CGContextRelease(c);CGColorSpaceRelease(cs);NSData*j=jpeg(png);NSData*bad=jpeg([@"invalid"dataUsingEncoding:NSUTF8StringEncoding]);output(@{@"selfTest":(ok&&j.length&&!bad)?@"PASS":@"FAIL",@"directoryRecordsModified":@NO});return ok&&j.length&&!bad?0:1;}
 BOOL restore=[arg isEqual:@"--restore"];BOOL repair=[arg isEqual:@"--repair"];if(!restore&&!repair&&![arg isEqual:@"--inspect"]){fprintf(stderr,"Use --inspect, --repair, --restore backup.plist or --self-test\n");return 2;}
 NSString*user=getenv("SUDO_USER")?@(getenv("SUDO_USER")):NSUserName();if(!user.length||[user isEqual:@"root"]){output(@{@"error":@"Run through sudo as your normal Dell account; root is not a target."});return 2;}
 NSError*error=nil;ODNode*node=[ODNode nodeWithSession:[ODSession defaultSession] type:kODNodeTypeLocalNodes error:&error];NSArray*attrs=@[kODAttributeTypeJPEGPhoto,kODAttributeTypePicture,kODAttributeTypeUniqueID];ODRecord*record=[node recordWithRecordType:kODRecordTypeUsers name:user attributes:attrs error:&error];if(!record){output(@{@"error":error.localizedDescription?:@"Local account lookup failed"});return 2;}
 NSArray*photos=[record valuesForAttribute:kODAttributeTypeJPEGPhoto error:nil]?:@[];NSArray*pictures=[record valuesForAttribute:kODAttributeTypePicture error:nil]?:@[];NSArray*ids=[record valuesForAttribute:kODAttributeTypeUniqueID error:nil]?:@[];
 if(restore){
  if(geteuid()!=0||!targetDell()||argc!=3){output(@{@"error":@"Restore requires root on the Dell and a known photo backup path."});return 2;}
  NSString*path=@(argv[2]);if(![path hasPrefix:@"/Library/Logs/LatitudeAvatar/photo-"]){output(@{@"error":@"Use the backup path printed by the repair."});return 2;}
  NSData*data=[NSData dataWithContentsOfFile:path];NSDictionary*saved=data?[NSPropertyListSerialization propertyListWithData:data options:NSPropertyListImmutable format:NULL error:&error]:nil;
  if(![saved isKindOfClass:[NSDictionary class]]||![saved[@"user"]isEqual:user]||![saved[@"JPEGPhoto"]isKindOfClass:[NSArray class]]||![saved[@"Picture"]isKindOfClass:[NSArray class]]){output(@{@"error":@"Backup does not match this local account."});return 2;}
  BOOL ok=put(record,kODAttributeTypeJPEGPhoto,saved[@"JPEGPhoto"],&error);if(ok)ok=put(record,kODAttributeTypePicture,saved[@"Picture"],&error);
  if(!ok){put(record,kODAttributeTypeJPEGPhoto,photos,nil);put(record,kODAttributeTypePicture,pictures,nil);}
  output(@{@"restored":@(ok),@"user":user,@"error":error.localizedDescription?:@""});return ok?0:2;
 }
 NSData*source=nil;NSMutableArray*paths=[NSMutableArray array];NSUInteger bytes=0;
 for(id value in photos)if([value isKindOfClass:[NSData class]]){bytes+=[value length];if(!source&&jpeg(value))source=value;}
 for(id value in pictures)if([value isKindOfClass:[NSString class]]){NSString*path=value;BOOL readable=[[NSFileManager defaultManager]isReadableFileAtPath:path];NSData*data=readable?[NSData dataWithContentsOfFile:path]:nil;BOOL decodes=jpeg(data)!=nil;NSDictionary*a=[[NSFileManager defaultManager]attributesOfItemAtPath:path error:nil];[paths addObject:@{@"path":path,@"readableToCollector":@(readable),@"imageDecodes":@(decodes),@"fileMode":a[NSFilePosixPermissions]?:@0,@"fileOwnerUID":a[NSFileOwnerAccountID]?:@(-1)}];if(!source&&decodes)source=data;}
 NSMutableDictionary*result=[@{@"user":user,@"JPEGPhotoCount":@(photos.count),@"JPEGPhotoBytes":@(bytes),@"pictureFiles":paths,@"usableLocalPhoto":@(source!=nil),@"repairRequested":@(repair),@"source":@"Local login-photo record; Apple Account may display a different cached representation",@"rawImageSavedToReport":@NO}mutableCopy];
 if(!repair){output(result);return 0;}
 if(geteuid()!=0||!targetDell()){result[@"error"]=@"Repair is restricted to root on the Dell target (Intel PCI 8086:7d45).";output(result);return 2;}
 NSData*normalized=jpeg(source);if(!normalized){result[@"error"]=@"No valid local login photo. Select your photo in System Settings > Users & Groups; an Apple Account photo alone is not sufficient.";output(result);return 2;}
 NSString*uid=ids.firstObject;if(![uid isKindOfClass:[NSString class]]||!uid.length||[uid rangeOfCharacterFromSet:[[NSCharacterSet decimalDigitCharacterSet]invertedSet]].location!=NSNotFound){result[@"error"]=@"Invalid local account UID; no changes made.";output(result);return 2;}
 NSFileManager*fm=[NSFileManager defaultManager];NSString*backupDir=@"/Library/Logs/LatitudeAvatar";[fm createDirectoryAtPath:backupDir withIntermediateDirectories:YES attributes:@{NSFilePosixPermissions:@0700}error:&error];NSString*stamp=[NSString stringWithFormat:@"%.0f",[NSDate date].timeIntervalSince1970];NSString*backup=[backupDir stringByAppendingPathComponent:[NSString stringWithFormat:@"photo-%@-%@.plist",uid,stamp]];NSData*saved=[NSPropertyListSerialization dataWithPropertyList:@{@"user":user,@"JPEGPhoto":photos,@"Picture":pictures}format:NSPropertyListBinaryFormat_v1_0 options:0 error:&error];if(!saved||![saved writeToFile:backup options:NSDataWritingAtomic error:&error]){result[@"error"]=error.localizedDescription?:@"Cannot back up photo attributes";output(result);return 2;}[fm setAttributes:@{NSFilePosixPermissions:@0600}ofItemAtPath:backup error:nil];
 NSString*folder=@"/Library/Application Support/LatitudeAvatar";if(![fm createDirectoryAtPath:folder withIntermediateDirectories:YES attributes:@{NSFilePosixPermissions:@0755}error:&error]){result[@"error"]=error.localizedDescription;output(result);return 2;}
 NSString*path=[folder stringByAppendingPathComponent:[NSString stringWithFormat:@"Latitude-%@-%@.jpg",uid,stamp]];if(![normalized writeToFile:path options:NSDataWritingAtomic error:&error]){result[@"error"]=error.localizedDescription;output(result);return 2;}[fm setAttributes:@{NSFilePosixPermissions:@0644,NSFileOwnerAccountID:@0,NSFileGroupOwnerAccountID:@0}ofItemAtPath:path error:nil];
 BOOL ok=[record setValue:@[normalized]forAttribute:kODAttributeTypeJPEGPhoto error:&error];if(ok)ok=[record setValue:@[path]forAttribute:kODAttributeTypePicture error:&error];if(!ok){put(record,kODAttributeTypeJPEGPhoto,photos,nil);put(record,kODAttributeTypePicture,pictures,nil);result[@"error"]=error.localizedDescription?:@"Photo record update failed; prior attributes restored";output(result);return 2;}
 result[@"repaired"]=@YES;result[@"backup"]=backup;result[@"pictureFile"]=path;result[@"scope"]=@"Same local photo normalized to readable 8-bit sRGB JPEG; identity attributes unchanged; no Apple Account API called";output(result);return 0;
}}
