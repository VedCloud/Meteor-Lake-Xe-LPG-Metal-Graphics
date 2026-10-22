#pragma once
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define NVMTL_PUBLICATION_SLOTS 3
#define NVMTL_PUBLICATION_MAX (16u<<20)
typedef struct {pthread_mutex_t lock;void *bytes;size_t capacity;} nvmtl_publication_slot;
static nvmtl_publication_slot nvmtl_publication_slots[3]={
 {PTHREAD_MUTEX_INITIALIZER,NULL,0},{PTHREAD_MUTEX_INITIALIZER,NULL,0},{PTHREAD_MUTEX_INITIALIZER,NULL,0}};
static int nvmtl_publication_acquire(size_t bytes,void **data) {
 if(!bytes || bytes>NVMTL_PUBLICATION_MAX || !data)return -1;
 for(unsigned i=0;i<NVMTL_PUBLICATION_SLOTS;i++){
  nvmtl_publication_slot *s=&nvmtl_publication_slots[i];
  if(pthread_mutex_trylock(&s->lock))continue;
  if(s->capacity<bytes){void *p=realloc(s->bytes,bytes);if(!p){pthread_mutex_unlock(&s->lock);continue;}s->bytes=p;s->capacity=bytes;}
  *data=s->bytes;return (int)i;
 }
 return -1;
}
static void nvmtl_publication_release(int lease){if(lease>=0 && lease<NVMTL_PUBLICATION_SLOTS)pthread_mutex_unlock(&nvmtl_publication_slots[lease].lock);}
static int nvmtl_publication_geometry(size_t width,size_t height,size_t bpp,size_t *packed,size_t *bytes){
 if(!width||!height||!bpp||!packed||!bytes||width>SIZE_MAX/bpp)return -1;
 *packed=width*bpp;if(height>SIZE_MAX/(*packed))return -1;*bytes=height*(*packed);return 0;
}
static int nvmtl_publication_copy(void *dst,size_t pitch,const void *src,size_t packed,size_t rows){
 if(!dst||!src||!rows||!packed||pitch<packed||rows>SIZE_MAX/packed||
    (rows>1&&pitch>(SIZE_MAX-packed)/(rows-1)))return -1;
 if(pitch==packed)memcpy(dst,src,rows*packed);
 else for(size_t y=0;y<rows;y++)memcpy((uint8_t *)dst+y*pitch,(const uint8_t *)src+y*packed,packed);
 return 0;
}
