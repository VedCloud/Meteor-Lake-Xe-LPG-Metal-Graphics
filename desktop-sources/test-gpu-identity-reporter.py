from pathlib import Path
import tempfile,subprocess,os,json
r=Path(__file__).resolve().parent.parent
icd=r'''
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <string.h>
static int mode(const char*s){return getenv("MODE")&&!strcmp(getenv("MODE"),s);}
static VkResult create(const VkInstanceCreateInfo*i,const VkAllocationCallbacks*a,VkInstance*out){*out=(VkInstance)1;return mode("create-fail")?VK_ERROR_INITIALIZATION_FAILED:VK_SUCCESS;}
static void destroy(VkInstance i,const VkAllocationCallbacks*a){}
static VkResult enumerate(VkInstance i,uint32_t*n,VkPhysicalDevice*d){*n=mode("no-devices")?0:mode("many-devices")?9:mode("duplicate")?2:1;if(d&&*n<=8)for(unsigned j=0;j<*n;j++)d[j]=(VkPhysicalDevice)(uintptr_t)(j+1);return mode("enum-fail")?VK_ERROR_DEVICE_LOST:VK_SUCCESS;}
static void props(VkPhysicalDevice d,VkPhysicalDeviceProperties*p){memset(p,0,sizeof(*p));p->vendorID=mode("wrong-vendor")?0x10de:0x8086;p->deviceID=0x7d55;p->deviceType=mode("discrete")?VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;memset(p->deviceName,mode("unterminated-name")?'x':0,sizeof(p->deviceName));if(!mode("unterminated-name"))strcpy(p->deviceName,"Driver supplied test identity");}
static void memory(VkPhysicalDevice d,VkPhysicalDeviceMemoryProperties*p){memset(p,0,sizeof(*p));p->memoryHeapCount=mode("bad-count")?VK_MAX_MEMORY_HEAPS+1:1;p->memoryTypeCount=1;p->memoryTypes[0].heapIndex=mode("bad-index")?1:0;p->memoryHeaps[0].flags=mode("no-local-heap")?0:VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;const char*b=getenv("HEAP_BYTES");p->memoryHeaps[0].size=b?strtoull(b,0,10):(12ull<<30);}
PFN_vkVoidFunction vk_icdGetInstanceProcAddr(VkInstance i,const char*n){
 if(!strcmp(n,"vkCreateInstance"))return(PFN_vkVoidFunction)create;
 if(!strcmp(n,"vkDestroyInstance"))return(PFN_vkVoidFunction)destroy;
 if(!strcmp(n,"vkEnumeratePhysicalDevices"))return(PFN_vkVoidFunction)enumerate;
 if(!strcmp(n,"vkGetPhysicalDeviceProperties"))return(PFN_vkVoidFunction)props;
 if(!strcmp(n,"vkGetPhysicalDeviceMemoryProperties"))return(PFN_vkVoidFunction)memory;
 return 0;
}
'''
io=r'''
#import <Foundation/Foundation.h>
#include <IOKit/IOKitLib.h>
#include <unistd.h>
static NSDictionary *published;
static int iter=0;
static BOOL mode(const char*s){return getenv("MODE")&&!strcmp(getenv("MODE"),s);}
uid_t identity_mock_geteuid(void){return mode("nonroot")?501:0;}
int identity_mock_sysctl(const char*n,void*out,size_t*size,void*value,size_t len){uint64_t ram=16ull<<30;if(*size<8)return -1;memcpy(out,&ram,8);*size=8;return 0;}
kern_return_t IOServiceGetMatchingServices(mach_port_t m,CFDictionaryRef d,io_iterator_t*i){CFRelease(d);*i=1;iter=0;return 0;}
CFMutableDictionaryRef IOServiceMatching(const char*n){return CFDictionaryCreateMutable(NULL,0,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);}
io_object_t IOIteratorNext(io_iterator_t i){if(mode("no-framebuffer"))return 0;if(iter++==0)return 10;return 0;}
kern_return_t IOObjectRelease(io_object_t o){return 0;}
kern_return_t IORegistryEntryGetParentEntry(io_registry_entry_t e,const io_name_t p,io_registry_entry_t*out){*out=e+1;return 0;}
kern_return_t IORegistryEntrySetCFProperties(io_registry_entry_t e,CFTypeRef p){if(mode("publish-fail"))return kIOReturnUnsupported;published=[(__bridge NSDictionary*)p objectForKey:@"LatitudeGPUIdentity"];return 0;}
CFTypeRef IORegistryEntryCreateCFProperty(io_registry_entry_t e,CFStringRef key,CFAllocatorRef a,IOOptionBits o){
 NSString*k=(__bridge NSString*)key;id value=nil;
 if([k isEqual:@"vendor-id"]||[k isEqual:@"device-id"]){uint32_t v=[k isEqual:@"vendor-id"]?0x8086:0x7d55;value=[NSData dataWithBytes:&v length:4];}
 else if([k isEqual:@"nmxgpu-claimed"])value=mode("unclaimed")?@NO:@YES;
 else if([k isEqual:@"model"]){NSMutableData*d=[[published[@"name"] dataUsingEncoding:NSUTF8StringEncoding] mutableCopy];char zero=0;[d appendBytes:&zero length:1];value=d;}
 else if([k isEqual:@"VRAM,totalMB"])value=mode("bad-readback")?@7:@([published[@"sharedHeapBytes"] unsignedLongLongValue]>>20);
 return value?CFBridgingRetain(value):NULL;
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'icd.c').write_text(icd);(p/'io.m').write_text(io)
 subprocess.run(['xcrun','clang','-dynamiclib','-fsanitize=address,undefined','-I',str(r/'Vulkan-Headers/include'),str(p/'icd.c'),'-o',str(p/'icd.dylib')],check=True)
 subprocess.run(['xcrun','clang','-fobjc-arc','-fsanitize=address,undefined','-g','-DLATITUDE_IDENTITY_RETRIES=1','-Dgeteuid=identity_mock_geteuid','-Dsysctlbyname=identity_mock_sysctl','-DLATITUDE_IDENTITY_ICD="'+str(p/'icd.dylib')+'"','-I',str(r/'Vulkan-Headers/include'),'-I',str(r/'desktop-sources'),str(r/'desktop-sources/latitude-gpu-identity.m'),str(p/'io.m'),'-framework','Foundation','-framework','IOKit','-o',str(p/'test')],check=True)
 for heap in [1537<<20,3<<30,8<<30,12<<30,16<<30]:
  e=os.environ.copy();e.update(MODE='success',HEAP_BYTES=str(heap))
  c=subprocess.run([str(p/'test')],env=e,capture_output=True,text=True);assert c.returncode==0,c.stderr
  data=json.loads(c.stdout[:c.stdout.index('}\n')+1]);assert data['sharedHeapBytes']==heap and data['name']=='Driver supplied test identity';assert 'VERIFIED' in c.stdout
 for mode in ['unclaimed','create-fail','enum-fail','no-devices','many-devices','duplicate','wrong-vendor','discrete','unterminated-name','bad-count','bad-index','no-local-heap','no-framebuffer','publish-fail','bad-readback','nonroot']:
  e=os.environ.copy();e.update(MODE=mode,HEAP_BYTES=str(12<<30));c=subprocess.run([str(p/'test')],env=e,capture_output=True,text=True);assert c.returncode!=0 and 'VERIFIED' not in c.stdout,(mode,c.stdout,c.stderr)
 for heap in [0,1,17<<30,2**64-1]:
  e=os.environ.copy();e.update(MODE='success',HEAP_BYTES=str(heap));c=subprocess.run([str(p/'test')],env=e,capture_output=True,text=True);assert c.returncode!=0,(heap,c.stdout,c.stderr)
 print('PASS actual reporter ASan/UBSan with simulated ICD/IOKit: driver names and varying >4GiB capacities preserved, PCI publication readback, 19 failure guards, no Vulkan logical device/queues/rendering')
