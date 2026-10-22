/* SPDX-License-Identifier: MIT
 * Firmware admission and bounded A64 instruction probe. Not an XNU bootloader. */
#include <Uefi.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/GraphicsOutput.h>
#include "boot_input.h"
#include "profile.h"
#include "a64_core.h"
#include "probe_payload.h"
#include "privileged_payload.h"
#include "macho_image.h"
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
static EFI_STATUS execute_probe(void){
 UINT8 *code=NULL,*data=NULL;
 EFI_STATUS status=services->AllocatePool(EfiLoaderData,sizeof(probe_code),(VOID**)&code);
 if(!EFI_ERROR(status))status=services->AllocatePool(EfiLoaderData,4096,(VOID**)&data);
 if(!EFI_ERROR(status)){
  memcpy(code,probe_code,sizeof(probe_code));memset(data,0,4096);memcpy(data,probe_data,sizeof(probe_data));
  D27Cpu cpu;memset(&cpu,0,sizeof(cpu));
  D27Region regions[2]={{0,code,sizeof(probe_code),D27_READ|D27_EXEC},{0x41000000,data,4096,D27_READ|D27_WRITE}};
  enum D27Stop stop=d27_a64_run(&cpu,regions,2,10000);
  UINT8 different=0;for(UINTN i=0;i<sizeof(probe_expected);i++)different|=data[i]^probe_expected[i];
  if(stop!=D27_BREAKPOINT||cpu.pc!=0xb0||cpu.retired!=69||
     cpu.syndrome!=((0x3cu<<26)|(1u<<25)|0x27)||different)status=EFI_COMPROMISED_DATA;
  else say(L"PASS: ARM scalar payload executed on x86; 69 instructions, 80 QEMU-matched result bytes.\r\n");
 }
 if(data)services->FreePool(data);if(code)services->FreePool(code);
 return status;
}
static EFI_STATUS execute_privileged_probe(void){
 UINT8 *code=NULL,*data=NULL;
 EFI_STATUS status=services->AllocatePool(EfiLoaderData,sizeof(priv_code),(VOID**)&code);
 if(!EFI_ERROR(status))status=services->AllocatePool(EfiLoaderData,4096,(VOID**)&data);
 if(!EFI_ERROR(status)){
  memcpy(code,priv_code,sizeof(priv_code));memset(data,0,4096);memcpy(data,priv_data,sizeof(priv_data));
  D27Cpu cpu;memset(&cpu,0,sizeof(cpu));cpu.current_el=2;cpu.os_lock=1;
  D27Region regions[2]={{0,code,sizeof(priv_code),D27_READ|D27_EXEC},{0x41000000,data,4096,D27_READ|D27_WRITE}};
  enum D27Stop stop=d27_a64_run(&cpu,regions,2,10000);
  UINT8 different=0;for(UINTN i=0;i<sizeof(priv_expected);i++)different|=data[i]^priv_expected[i];
  if(stop!=D27_BREAKPOINT||cpu.pc!=0x4c||cpu.retired!=19||cpu.os_lock||cpu.daif!=0x300||
     cpu.vbar_el1!=0x12345000||different)status=EFI_COMPROMISED_DATA;
  else say(L"PASS: selected EL2 setup; 19 instructions, 80 QEMU-matched result bytes.\r\n");
 }
 if(data)services->FreePool(data);if(code)services->FreePool(code);return status;
}
static EFI_STATUS inspect_sptm_entry(EFI_FILE_PROTOCOL *root){
 /* A bounded entry probe, not the complete Darwin board's boot layout.
  * x0 deliberately remains unmapped: stop at the first boot-argument read. */
 const D27Input *input=&inputs[2];UINT8 *file_bytes=NULL,*memory=NULL;
 EFI_FILE_PROTOCOL *file=NULL;D27Image plan;D27Region regions[D27_MAX_SEGMENTS];UINT64 entry;
 if(input->bytes>16*1024*1024)return EFI_BAD_BUFFER_SIZE;
 EFI_STATUS status=services->AllocatePool(EfiLoaderData,(UINTN)input->bytes,(VOID**)&file_bytes);
 if(!EFI_ERROR(status))status=root->Open(root,&file,input->path,EFI_FILE_MODE_READ,0);
 if(!EFI_ERROR(status)){
  UINT64 size=0;status=file->SetPosition(file,MAX_UINT64);
  if(!EFI_ERROR(status))status=file->GetPosition(file,&size);
  if(!EFI_ERROR(status)&&size!=input->bytes)status=EFI_COMPROMISED_DATA;
  if(!EFI_ERROR(status))status=file->SetPosition(file,0);
  if(!EFI_ERROR(status)){
   UINTN got=(UINTN)input->bytes;status=file->Read(file,&got,file_bytes);
   if(!EFI_ERROR(status)&&got!=input->bytes)status=EFI_DEVICE_ERROR;
  }
 }
 if(file)file->Close(file);
 if(!EFI_ERROR(status)){
  D27Sha256 hash;UINT8 digest[32],different=0;d27_sha_init(&hash);
  d27_sha_update(&hash,file_bytes,(UINTN)input->bytes);d27_sha_final(&hash,digest);
  for(UINTN i=0;i<32;i++)different|=digest[i]^input->sha256[i];
  if(different)status=EFI_SECURITY_VIOLATION;
 }
 if(!EFI_ERROR(status)&&d27_image_plan(file_bytes,(UINTN)input->bytes,&plan))status=EFI_LOAD_ERROR;
 if(!EFI_ERROR(status))status=services->AllocatePool(EfiLoaderData,(UINTN)plan.bytes,(VOID**)&memory);
 if(!EFI_ERROR(status)&&d27_image_load(file_bytes,(UINTN)input->bytes,0x800000000ULL,
   memory,(UINTN)plan.bytes,regions,D27_MAX_SEGMENTS,&plan,&entry))status=EFI_LOAD_ERROR;
 if(!EFI_ERROR(status)){
  D27Cpu cpu;memset(&cpu,0,sizeof(cpu));cpu.pc=entry;cpu.current_el=2;cpu.os_lock=1;cpu.daif=0x3c0;
  enum D27Stop stop=d27_a64_run(&cpu,regions,plan.count,100);
  if(stop!=D27_DATA_FAULT||cpu.retired!=5||cpu.fault_address||cpu.instruction!=0xf9400280||
     cpu.os_lock||cpu.daif!=0x3c0)status=EFI_COMPROMISED_DATA;
  else say(L"SPTM entry probe: 5 real instructions executed. EXPECTED STOP: unmapped boot arguments.\r\n");
 }
 if(memory)services->FreePool(memory);if(file_bytes)services->FreePool(file_bytes);return status;
}
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image,EFI_SYSTEM_TABLE *table){
 system_table=table;services=table->BootServices;
 say(L"Dell macOS 27 standalone instruction probe 0.0.3\r\n");
 say(L"A64/EL2 entry probes only; no macOS kernel boot.\r\n");
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
 if(prefix)services->FreePool(prefix);if(chunk)services->FreePool(chunk);
 if(EFI_ERROR(status)){root->Close(root);say(L"Rejected boot inputs. No guest was executed.\r\n");return status;}
 say(L"\r\nPASS: standalone firmware entry and pinned boot-input admission.\r\n");
 status=execute_probe();
 if(!EFI_ERROR(status))status=execute_privileged_probe();
 if(!EFI_ERROR(status))status=inspect_sptm_entry(root);
 root->Close(root);
 if(EFI_ERROR(status)){say(L"FAIL: ARM/EL2/SPTM instruction probe.\r\n");return status;}
 say(L"STOP: privileged ARM/XNU execution runtime is UNIMPLEMENTED.\r\n");
 say(L"Boot services remain active. GPU registers, NVRAM and storage untouched.\r\n");
 say(L"Press a key to return to firmware.\r\n");
 if(table->ConIn){UINTN index;services->WaitForEvent(1,&table->ConIn->WaitForKey,&index);}
 return EFI_UNSUPPORTED;
}
