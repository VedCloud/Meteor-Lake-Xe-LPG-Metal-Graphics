/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include "sparse-ram.h"
/* Guest RAM is sparse, but every committed byte needs actual firmware memory.
 * Use only currently conventional pages for budgeting. Keep half available
 * for translated code, mapping metadata and firmware; reserve another 128 MiB.
 * This is an allocation ceiling, never a promise that allocation will succeed.
 */
EFI_STATUS host_memory_budget(EFI_SYSTEM_TABLE*t,uint64_t logical,uint64_t initialized,uint64_t*budget){
 if(!t||!budget||initialized>logical)return EFI_INVALID_PARAMETER;
 UINTN bytes=0,key=0,stride=0;UINT32 version=0;EFI_MEMORY_DESCRIPTOR*map=NULL;
 EFI_STATUS s=t->BootServices->GetMemoryMap(&bytes,map,&key,&stride,&version);
 if(s!=EFI_BUFFER_TOO_SMALL||!stride||bytes>MAX_UINTN-16*stride)return EFI_DEVICE_ERROR;
 for(unsigned retry=0;retry<4;retry++){
  UINTN capacity=bytes+16*stride;
  s=t->BootServices->AllocatePool(EfiLoaderData,capacity,(VOID**)&map);if(EFI_ERROR(s))return s;
  bytes=capacity;s=t->BootServices->GetMemoryMap(&bytes,map,&key,&stride,&version);
  if(s==EFI_BUFFER_TOO_SMALL){t->BootServices->FreePool(map);map=NULL;continue;}
  if(EFI_ERROR(s)||stride<sizeof(*map)||bytes%stride){t->BootServices->FreePool(map);return EFI_DEVICE_ERROR;}
  uint64_t available=0;
  for(UINTN at=0;at<bytes;at+=stride){EFI_MEMORY_DESCRIPTOR*d=(void*)((unsigned char*)map+at);
   if(d->Type==EfiConventionalMemory){if(d->NumberOfPages>(UINT64_MAX-available)/4096){t->BootServices->FreePool(map);return EFI_DEVICE_ERROR;}available+=d->NumberOfPages*4096;}}
  t->BootServices->FreePool(map);
  uint64_t reserve=128ULL*1024*1024,limit=available/2;
  if(limit<=reserve)return EFI_OUT_OF_RESOURCES;
  limit=(limit-reserve)/RAM_BLOCK*RAM_BLOCK;if(limit>logical)limit=logical;
  if(limit<initialized||limit-initialized<64ULL*1024*1024)return EFI_OUT_OF_RESOURCES;
  *budget=limit;return EFI_SUCCESS;
 }
 return EFI_OUT_OF_RESOURCES;
}
