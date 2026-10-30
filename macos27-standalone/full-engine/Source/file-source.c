/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include "../Standalone/boot_input.h"
#ifdef TEST_BOOT_CONTEXT_PROFILE
#include "BootContextExperiment/vmapple_reference_boot.h"
#elif defined(TEST_HFS_PROFILE)
#include "HFSExperiment/vmapple_reference_boot.h"
#elif defined(TEST_STORAGE_PROFILE)
#include "StorageExperiment/vmapple_reference_boot.h"
#elif defined(TEST_FRAMEBUFFER_PROFILE)
#include "FramebufferExperiment/vmapple_reference_boot.h"
#elif defined(TEST_INSTALLER_PROFILE)
#include "InstallerExperiment/vmapple_reference_boot.h"
#else
#include "vmapple_reference_boot.h"
#endif
void bridge_output(const char*);
static EFI_STATUS open_on(EFI_SYSTEM_TABLE*t,EFI_HANDLE device,EFI_FILE_PROTOCOL**root,EFI_FILE_PROTOCOL**file,CHAR16*path){
 EFI_GUID guid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*fs=NULL;
 EFI_STATUS s=t->BootServices->HandleProtocol(device,&guid,(VOID**)&fs);if(EFI_ERROR(s))return s;
 s=fs->OpenVolume(fs,root);if(EFI_ERROR(s))return s;
 s=(*root)->Open(*root,file,path,EFI_FILE_MODE_READ,0);if(EFI_ERROR(s)){(*root)->Close(*root);*root=NULL;}return s;
}
static EFI_STATUS search(EFI_SYSTEM_TABLE*t,EFI_FILE_PROTOCOL**root,EFI_FILE_PROTOCOL**file){
 EFI_GUID guid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;EFI_HANDLE*handles=NULL;UINTN count=0;
 EFI_STATUS s=t->BootServices->LocateHandleBuffer(ByProtocol,&guid,NULL,&count,&handles);if(EFI_ERROR(s))return s;
 s=EFI_NOT_FOUND;for(UINTN i=0;i<count;i++){s=open_on(t,handles[i],root,file,VMREF_INPUT_PATH);if(!EFI_ERROR(s))break;}
 t->BootServices->FreePool(handles);return s;
}
static EFI_STATUS load_exfat(EFI_HANDLE image,EFI_SYSTEM_TABLE*t,EFI_HANDLE device){
 EFI_FILE_PROTOCOL*root=NULL,*file=NULL;EFI_STATUS s=open_on(t,device,&root,&file,L"\\EFI\\Drivers\\exfat_x64.efi");if(EFI_ERROR(s))return s;
 UINT64 size=0;void*data=NULL;s=file->SetPosition(file,MAX_UINT64);if(!EFI_ERROR(s))s=file->GetPosition(file,&size);
 if(!EFI_ERROR(s)&&size!=53248)s=EFI_SECURITY_VIOLATION;
 if(!EFI_ERROR(s))s=file->SetPosition(file,0);
 if(!EFI_ERROR(s))s=t->BootServices->AllocatePool(EfiLoaderData,(UINTN)size,&data);
 if(!EFI_ERROR(s)){UINTN got=size;s=file->Read(file,&got,data);if(!EFI_ERROR(s)&&got!=size)s=EFI_DEVICE_ERROR;}
 file->Close(file);root->Close(root);
 static const uint8_t expected[32]={0x3f,0x16,0xa8,0xb7,0x2e,0xa8,0x1a,0x6e,0x7f,0x9d,0xea,0x0f,0xe9,0x2a,0x10,0x8c,0xd9,0xb9,0x80,0xe2,0x9c,0x21,0xf0,0x58,0xdd,0xb2,0xf2,0x73,0x00,0x27,0xec,0x81};
 if(!EFI_ERROR(s)){D27Sha256 hash;uint8_t digest[32];d27_sha_init(&hash);d27_sha_update(&hash,data,size);d27_sha_final(&hash,digest);if(memcmp(digest,expected,32))s=EFI_SECURITY_VIOLATION;}
 EFI_HANDLE driver=NULL;
 if(!EFI_ERROR(s))s=t->BootServices->LoadImage(FALSE,image,NULL,data,(UINTN)size,&driver);
 if(data)t->BootServices->FreePool(data);
 if(!EFI_ERROR(s)){s=t->BootServices->StartImage(driver,NULL,NULL);if(EFI_ERROR(s))t->BootServices->UnloadImage(driver);}
 if(EFI_ERROR(s))return s;
 EFI_GUID block=EFI_BLOCK_IO_PROTOCOL_GUID;EFI_HANDLE*handles=NULL;UINTN count=0;
 s=t->BootServices->LocateHandleBuffer(ByProtocol,&block,NULL,&count,&handles);if(EFI_ERROR(s))return s;
 for(UINTN i=0;i<count;i++)t->BootServices->ConnectController(handles[i],NULL,NULL,TRUE);
 t->BootServices->FreePool(handles);bridge_output("Verified EfiFs 1.13 exFAT driver started; rescanning input volumes\n");return EFI_SUCCESS;
}
EFI_STATUS open_vmapple_input(EFI_HANDLE image,EFI_SYSTEM_TABLE*t,EFI_FILE_PROTOCOL**root,EFI_FILE_PROTOCOL**file){
 *root=NULL;*file=NULL;EFI_GUID guid=EFI_LOADED_IMAGE_PROTOCOL_GUID;EFI_LOADED_IMAGE_PROTOCOL*loaded=NULL;
 EFI_STATUS s=t->BootServices->HandleProtocol(image,&guid,(VOID**)&loaded);if(EFI_ERROR(s))return s;
 s=open_on(t,loaded->DeviceHandle,root,file,VMREF_INPUT_PATH);if(!EFI_ERROR(s))return s;
 s=search(t,root,file);if(!EFI_ERROR(s))return s;
 s=load_exfat(image,t,loaded->DeviceHandle);if(EFI_ERROR(s))return s;
 return search(t,root,file);
}
