from pathlib import Path
import subprocess,tempfile,json,os
r=Path(__file__).resolve().parent.parent
pre=r'''#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#include <unistd.h>
#include <assert.h>
static unsigned nextObject;
static kern_return_t matching(mach_port_t p,CFDictionaryRef match,io_iterator_t*out){CFRelease(match);*out=1;return 0;}
static io_service_t next(io_iterator_t i){return ++nextObject==1?11:0;}
static kern_return_t release(io_object_t i){return 0;}
static kern_return_t set(io_registry_entry_t e,CFTypeRef value){assert(e==11);NSDictionary*d=(__bridge NSDictionary*)value;assert([d[@"LatitudeReadHotkeyCounters"] isEqual:@YES]);return getenv("FAIL_QUERY")?kIOReturnBadArgument:0;}
static CFTypeRef get(io_registry_entry_t e,CFStringRef key,CFAllocatorRef a,IOOptionBits o){
 id v=nil;NSString*k=(__bridge NSString*)key;
 if([k isEqual:@"LatitudeHotkeyCounters"])v=@{@"Query66Entries":@2};
 if([k isEqual:@"LatitudeHotkeySnapshotStage"])v=getenv("FAIL_QUERY")?@"decode scalar":@"complete";
 if([k isEqual:@"LatitudeHotkeySnapshotCounter"])v=@"LastBRT6Argument";
 if([k isEqual:@"LatitudeHotkeySnapshotResult"])v=@(getenv("FAIL_QUERY")?kIOReturnBadArgument:0);
 return v?CFBridgingRetain(v):NULL;
}
#define geteuid() 0
#define IOServiceGetMatchingServices matching
#define IOIteratorNext next
#define IOObjectRelease release
#define IORegistryEntrySetCFProperties set
#define IORegistryEntryCreateCFProperty get
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.m').write_text(pre+(r/'desktop-sources/latitude-key-state.m').read_text())
 subprocess.run(['xcrun','clang','-fobjc-arc','-fsanitize=address,undefined','-framework','Foundation','-framework','IOKit',str(p/'test.m'),'-o',str(p/'test')],check=True)
 for fail in [False,True]:
  env=dict(os.environ)
  if fail:env['FAIL_QUERY']='1'
  c=subprocess.run([str(p/'test')],env=env,text=True,capture_output=True)
  assert c.returncode==(3 if fail else 0),c.stderr
  d=json.loads(c.stdout);assert d['snapshotFresh']==(not fail)
  assert ('counters' in d)==(not fail)
  assert d['LatitudeHotkeySnapshotStage']==('decode scalar' if fail else 'complete')
 print('PASS actual key-state helper: successful fresh counters, failure-stage metadata and cached counters excluded after failed request. IOKit calls mocked; no device access.')
