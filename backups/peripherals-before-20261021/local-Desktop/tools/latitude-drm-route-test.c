#include <xf86drm.h>
#include <i915_drm.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
int main(void) {
 setbuf(stdout,NULL);
 drmDevicePtr devices[8]={0};int count=drmGetDevices2(DRM_DEVICE_GET_PCI_REVISION,devices,8),found=0;
 printf("DISCOVERY route=%s count=%d errno=%d\n",getenv("NMX_DRM_TEST_TRANSPORT") ?: "auto",count,errno);
 for(int n=0;n<count;n++){
  drmDevicePtr d=devices[n];if(d->bustype==DRM_BUS_PCI){printf("PCI %04x:%04x nodes=0x%x\n",d->deviceinfo.pci->vendor_id,d->deviceinfo.pci->device_id,d->available_nodes);if(d->deviceinfo.pci->vendor_id==0x8086&&d->deviceinfo.pci->device_id==0x7d45)found=1;}
  drmFreeDevice(&devices[n]);
 }
 if(!found){fputs("FAIL target absent during DRM enumeration\n",stderr);return 1;}
 int fd=drmNmxOpen("/dev/dri/renderD128",O_RDWR|O_CLOEXEC);
 if(fd<0){fprintf(stderr,"FAIL DRM open errno=%d %s\n",errno,strerror(errno));return 1;}
 int type=drmGetNodeTypeFromFd(fd);printf("NODE type=%d expected=%d\n",type,DRM_NODE_RENDER);
 if(type!=DRM_NODE_RENDER){close(fd);return 1;}
 drmDevicePtr d=NULL;int result=drmGetDevice2(fd,DRM_DEVICE_GET_PCI_REVISION,&d);
 if(result||!d){fprintf(stderr,"FAIL device-from-fd rc=%d errno=%d\n",result,errno);close(fd);return 1;}
 int valid=d->bustype==DRM_BUS_PCI&&d->deviceinfo.pci->vendor_id==0x8086&&d->deviceinfo.pci->device_id==0x7d45;drmFreeDevice(&d);
 int chipset=-1;struct drm_i915_getparam p={.param=I915_PARAM_CHIPSET_ID,.value=&chipset};errno=0;
 result=drmNmxIoctl(fd,DRM_IOCTL_I915_GETPARAM,&p);
 printf("GETPARAM chipset rc=%d errno=%d value=0x%x\n",result,errno,chipset);
 close(fd);
 if(!valid||result||chipset!=0x7d45)return 1;
 puts("PASS DRM route: enumeration, open, node classification, fd identity and nested-pointer ioctl");return 0;
}
