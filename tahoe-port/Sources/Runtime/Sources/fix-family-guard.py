from pathlib import Path
b=Path(__file__).resolve().parent;f=b/'tahoe-runtime-check.m';s=f.read_text();start=s.index('static NSString *uuidAt(');end=s.index('static uint32_t number',start)
new=r'''static BOOL slice(NSData*d,const uint8_t**out,uint64_t*size){
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
'''
s=s[:start]+new+s[end:]
start=s.index('  for(NSDictionary*r in a){NSString*u=nil;');end=s.index('  return 0;',start)
s=s[:start]+'''  BOOL hasCollections=NO;NSDictionary*kc=collectionUUIDs(root,&hasCollections);if(!kc){fputs("Malformed or conflicting kernel-collection identities; no files changed.\\n",stderr);return 2;}
  for(NSDictionary*r in a){NSString*u=nil;if(hasCollections)u=kc[r[@"id"]];else for(NSString*p in r[@"paths"]){NSString*c=uuidAt([root stringByAppendingPathComponent:p]);if(c){u=c;break;}}
   if(!u||![u isEqual:r[@"uuid"]]){fprintf(stderr,"ABI identity mismatch: %s (found %s; source %s)\\n",[r[@"id"] UTF8String],u?u.UTF8String:"missing",hasCollections?"kernel collections":"standalone files");return 2;}printf("PASS %s UUID=%s source=%s\\n",[r[@"id"]UTF8String],u.UTF8String,hasCollections?"kernel collections":"standalone file");}
'''+s[end:]
f.write_text(s)
