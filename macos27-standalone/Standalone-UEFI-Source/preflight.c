/* SPDX-License-Identifier: MIT
 * Read-only firmware admission check. This is not an ARM runtime or OS loader. */
#include <Uefi.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/GraphicsOutput.h>
#include "boot_input.h"
#include "profile.h"
static EFI_SYSTEM_TABLE *system_table;
static EFI_BOOT_SERVICES *services;
static void say(CHAR16 *s){system_table->ConOut->OutputString(system_table->ConOut,s);}
static void number(UINT64 n){CHAR16 text[24];UINTN i=23;text[i]=0;do{text[--i]=(CHAR16)('0'+n%10);n/=10;}while(n);say(text+i);}
void *memcpy(void *to,const void *from,size_t n){uint8_t*d=to;const uint8_t*s=from;for(size_t i=0;i<n;i++)d[i]=s[i];return to;}
void *memset(void *to,int v,size_t n){uint8_t*d=to;for(size_t i=0;i<n;i++)d[i]=(uint8_t)v;return to;}
static EFI_STATUS inspect_file(EFI_FILE_PROTOCOL *root,const D27Input *input,UINT8 *chunk,UINT8 *prefix){
 EFI_FILE_PROTOCOL *file=NULL;EFI_STATUS status=root->Open(root,&file,input->path,EFI_FILE_MODE_READ,0);
 if(EFI_ERROR(status))return status;
 UINT64 size=0;status=file->SetPosition(file,MAX_UINT64);
 if(!EFI_ERROR(status))status=file->GetPosition(file,&size);
 if(!EFI_ERROR(status))status=file->SetPosition(file,0);
 if(EFI_ERROR(status)||size!=input->bytes){file->Close(file);return EFI_COMPROMISED_DATA;}
 D27Sha256 hash;d27_sha_init(&hash);UINT64 offset=0;UINTN prefix_size=0;
 while(offset<size){
  UINTN take=(size-offset>65536)?65536:(UINTN)(size-offset);
  UINTN got=take;status=file->Read(file,&got,chunk);
  if(EFI_ERROR(status)||got!=take){file->Close(file);return EFI_DEVICE_ERROR;}
  d27_sha_update(&hash,chunk,got);
  if(input->kernel&&prefix_size<D27_PREFIX_BYTES){
   UINTN copy=got;if(copy>D27_PREFIX_BYTES-prefix_size)copy=D27_PREFIX_BYTES-prefix_size;
   memcpy(prefix+prefix_size,chunk,copy);prefix_size+=copy;
  }
  offset+=got;
 }
 file->Close(file);UINT8 digest[32];d27_sha_final(&hash,digest);UINT8 different=0;
 for(UINTN i=0;i<32;i++)different|=digest[i]^input->sha256[i];
 if(different)return EFI_SECURITY_VIOLATION;
 if(input->kernel&&d27_kernel_header(prefix,prefix_size,size))return EFI_LOAD_ERROR;
 return EFI_SUCCESS;
}
static EFI_STATUS memory_inventory(void){
 UINTN size=0,key=0,stride=0;UINT32 version=0;
 EFI_STATUS s=services->GetMemoryMap(&size,NULL,&key,&stride,&version);
 if(s!=EFI_BUFFER_TOO_SMALL||stride<sizeof(EFI_MEMORY_DESCRIPTOR)||stride>4096||size>16*1024*1024)return EFI_COMPROMISED_DATA;
 UINTN capacity=size+8*stride;EFI_MEMORY_DESCRIPTOR *map=NULL;
 s=services->AllocatePool(EfiLoaderData,capacity,(VOID**)&map);if(EFI_ERROR(s))return s;
 size=capacity;s=services->GetMemoryMap(&size,map,&key,&stride,&version);
 if(!EFI_ERROR(s)){
  if(!stride||stride<sizeof(EFI_MEMORY_DESCRIPTOR)||size>capacity||size%stride)s=EFI_COMPROMISED_DATA;
  else{
   UINT64 free_pages=0;for(UINTN at=0;at<size;at+=stride){EFI_MEMORY_DESCRIPTOR*d=(VOID*)((UINT8*)map+at);
    if(d->Type==EfiConventionalMemory){if(d->NumberOfPages>MAX_UINT64-free_pages){s=EFI_COMPROMISED_DATA;break;}free_pages+=d->NumberOfPages;}
   }
   if(!EFI_ERROR(s)){say(L"Conventional firmware RAM (MiB): ");number(free_pages/256);say(L"\r\n");}
  }
 }
 services->FreePool(map);return s;
}
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image,EFI_SYSTEM_TABLE *table){
 system_table=table;services=table->BootServices;
 say(L"Dell macOS 27 standalone preflight 0.0.1\r\n");
 say(L"Diagnostic only: no ARM runtime, no kernel execution.\r\n");
 say(L"Profile: macOS 27.0 / 26A428 / Mac14,3\r\n\r\n");
 EFI_STATUS status=memory_inventory();
 if(EFI_ERROR(status)){say(L"FAIL: firmware memory-map inventory.\r\n");return status;}
 EFI_GUID graphics_guid=EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;EFI_GRAPHICS_OUTPUT_PROTOCOL*gop=NULL;
 status=services->LocateProtocol(&graphics_guid,NULL,(VOID**)&gop);
 if(!EFI_ERROR(status)&&gop&&gop->Mode&&gop->Mode->Info){
  say(L"Current GOP: ");number(gop->Mode->Info->HorizontalResolution);say(L" x ");number(gop->Mode->Info->VerticalResolution);
  say(L", pixels/row ");number(gop->Mode->Info->PixelsPerScanLine);say(L"\r\n");
 }else say(L"GOP unavailable; no display hardware was changed.\r\n");
 EFI_GUID loaded_guid=EFI_LOADED_IMAGE_PROTOCOL_GUID,fs_guid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
 EFI_LOADED_IMAGE_PROTOCOL*loaded=NULL;EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*fs=NULL;EFI_FILE_PROTOCOL*root=NULL;
 status=services->HandleProtocol(image,&loaded_guid,(VOID**)&loaded);if(EFI_ERROR(status))return status;
 status=services->HandleProtocol(loaded->DeviceHandle,&fs_guid,(VOID**)&fs);if(EFI_ERROR(status))return status;
 status=fs->OpenVolume(fs,&root);if(EFI_ERROR(status))return status;
 UINT8*chunk=NULL,*prefix=NULL;
 status=services->AllocatePool(EfiLoaderData,65536,(VOID**)&chunk);
 if(!EFI_ERROR(status))status=services->AllocatePool(EfiLoaderData,D27_PREFIX_BYTES,(VOID**)&prefix);
 if(!EFI_ERROR(status))for(UINTN i=0;i<sizeof(inputs)/sizeof(inputs[0]);i++){
  say(L"Checking ");say(inputs[i].path);say(L" ... ");
  status=inspect_file(root,&inputs[i],chunk,prefix);
  if(EFI_ERROR(status)){say(L"FAIL (status ");number(status&~MAX_BIT);say(L")\r\n");break;}
  say(L"SHA-256 verified\r\n");
 }
 if(prefix)services->FreePool(prefix);if(chunk)services->FreePool(chunk);root->Close(root);
 if(EFI_ERROR(status)){say(L"Rejected boot inputs. No guest was executed.\r\n");return status;}
 say(L"\r\nPASS: standalone firmware entry and pinned boot-input admission.\r\n");
 say(L"STOP: ARM-to-x86 execution runtime is UNIMPLEMENTED.\r\n");
 say(L"Boot services remain active. GPU registers, NVRAM and storage untouched.\r\n");
 say(L"Press a key to return to firmware.\r\n");
 if(table->ConIn){UINTN index;services->WaitForEvent(1,&table->ConIn->WaitForKey,&index);}
 return EFI_UNSUPPORTED;
}
