#include <xf86drm.h>
#include <i915_drm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
extern void *drmNmxMmap(size_t,int,int,int,off_t);
extern int drmNmxMunmap(void *,size_t);
int main(void) {
 setbuf(stdout,NULL);
 int fd=drmNmxOpen("/dev/dri/renderD128",O_RDWR|O_CLOEXEC);
 if(fd<0){fprintf(stderr,"DRM open failed: %s\n",strerror(errno));return 1;}
 const int params[]={I915_PARAM_CHIPSET_ID,I915_PARAM_REVISION,I915_PARAM_CS_TIMESTAMP_FREQUENCY,I915_PARAM_HAS_EXECBUF2,I915_PARAM_MMAP_GTT_VERSION};
 const char *names[]={"chipset","revision","timestamp_frequency","execbuf2","mmap_gtt_version"};
 for(unsigned i=0;i<sizeof(params)/sizeof(params[0]);i++) { int value=-1;struct drm_i915_getparam p={.param=params[i],.value=&value};errno=0;int rc=drmNmxIoctl(fd,DRM_IOCTL_I915_GETPARAM,&p);printf("GETPARAM %s rc=%d errno=%d value=%d (0x%x)\n",names[i],rc,errno,value,value); }
 const uint64_t ids[]={DRM_I915_QUERY_TOPOLOGY_INFO,DRM_I915_QUERY_GEOMETRY_SUBSLICES,DRM_I915_QUERY_ENGINE_INFO,DRM_I915_QUERY_MEMORY_REGIONS,DRM_I915_QUERY_HWCONFIG_BLOB};
 for(unsigned i=0;i<sizeof(ids)/sizeof(ids[0]);i++) {
  struct drm_i915_query_item item={.query_id=ids[i]};struct drm_i915_query q={.num_items=1,.items_ptr=(uint64_t)(uintptr_t)&item};errno=0;int rc=drmNmxIoctl(fd,DRM_IOCTL_I915_QUERY,&q);printf("QUERY id=%llu size rc=%d errno=%d length=%d\n",(unsigned long long)ids[i],rc,errno,item.length);
  if(rc==0&&item.length>0&&item.length<1048576) { void *data=calloc(1,item.length);if(!data)return 1;item.data_ptr=(uint64_t)(uintptr_t)data;errno=0;rc=drmNmxIoctl(fd,DRM_IOCTL_I915_QUERY,&q);printf("QUERY id=%llu data rc=%d errno=%d length=%d\n",(unsigned long long)ids[i],rc,errno,item.length);free(data); }
 }
 const uint64_t sizes[]={4096,65536,2097152};
 const uint64_t modes[]={I915_MMAP_OFFSET_WB,I915_MMAP_OFFSET_WC};
 const char *mode_names[]={"WB","WC"};
 for(unsigned n=0;n<3;n++) {
  struct drm_i915_gem_create create={.size=sizes[n]};errno=0;
  int rc=drmNmxIoctl(fd,DRM_IOCTL_I915_GEM_CREATE,&create),saved=errno;
  printf("GEM_CREATE size=%llu rc=%d errno=%d (%s) handle=%u actual_size=%llu\n",(unsigned long long)sizes[n],rc,saved,strerror(saved),create.handle,(unsigned long long)create.size);
  if(rc)continue;
  for(unsigned m=0;m<2;m++) {
   struct drm_i915_gem_mmap_offset mo={.handle=create.handle,.flags=modes[m]};errno=0;
   rc=drmNmxIoctl(fd,DRM_IOCTL_I915_GEM_MMAP_OFFSET,&mo);saved=errno;
   printf("MMAP_OFFSET mode=%s rc=%d errno=%d (%s) offset=0x%llx\n",mode_names[m],rc,saved,strerror(saved),(unsigned long long)mo.offset);
   if(rc)continue;
   errno=0;void *map=drmNmxMmap(create.size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(off_t)mo.offset);saved=errno;
   printf("NMX_MAP mode=%s size=%llu ptr=%p errno=%d (%s)\n",mode_names[m],(unsigned long long)create.size,map,saved,strerror(saved));
   if(map==MAP_FAILED)continue;
   volatile uint32_t *words=map;size_t count=create.size/sizeof(*words);uint32_t seed=arc4random();
   for(size_t i=0;i<count;i++)words[i]=seed^(uint32_t)i;
   size_t mismatches=0;for(size_t i=0;i<count;i++)if(words[i]!=(seed^(uint32_t)i))mismatches++;
   printf("CPU_MAP_READBACK mode=%s words=%zu mismatches=%zu (CPU access only)\n",mode_names[m],count,mismatches);
   errno=0;rc=drmNmxMunmap(map,create.size);saved=errno;printf("NMX_UNMAP rc=%d errno=%d (%s)\n",rc,saved,strerror(saved));
  }
  struct drm_gem_close close_bo={.handle=create.handle};errno=0;rc=drmNmxIoctl(fd,DRM_IOCTL_GEM_CLOSE,&close_bo);saved=errno;printf("GEM_CLOSE rc=%d errno=%d (%s)\n",rc,saved,strerror(saved));
 }
 close(fd);return 0;
}
