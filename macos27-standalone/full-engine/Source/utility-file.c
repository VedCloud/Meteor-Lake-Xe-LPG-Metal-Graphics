/* SPDX-License-Identifier: MIT
 * Only an existing fixed-size file on the EFI application's own volume.
 * No whole-disk access, implicit creation, or host/guest NVRAM sharing.
 */
#include "utility-file.h"
#include <Protocol/LoadedImage.h>
EFI_STATUS utility_file_open(EFI_HANDLE image,EFI_SYSTEM_TABLE*t,UtilityFile*f){
 f->file=NULL;f->bytes=0;EFI_LOADED_IMAGE_PROTOCOL*loaded=NULL;
 EFI_GUID li=EFI_LOADED_IMAGE_PROTOCOL_GUID,fsid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
 EFI_STATUS s=t->BootServices->HandleProtocol(image,&li,(void**)&loaded);if(EFI_ERROR(s))return s;
 EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*fs=NULL;s=t->BootServices->HandleProtocol(loaded->DeviceHandle,&fsid,(void**)&fs);if(EFI_ERROR(s))return s;
 EFI_FILE_PROTOCOL*root=NULL;s=fs->OpenVolume(fs,&root);if(EFI_ERROR(s))return s;
 s=root->Open(root,&f->file,L"\\EngineProbe\\UtilityStorage-v1.bin",EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE,0);root->Close(root);if(EFI_ERROR(s))return s;
 s=f->file->SetPosition(f->file,MAX_UINT64);if(!EFI_ERROR(s))s=f->file->GetPosition(f->file,&f->bytes);
 if(!EFI_ERROR(s)&&f->bytes!=32*1024*1024)s=EFI_COMPROMISED_DATA;
 if(EFI_ERROR(s)){utility_file_close(f);return s;}return EFI_SUCCESS;
}
int utility_file_transfer(void*opaque,uint64_t at,void*data,unsigned bytes,bool write){
 UtilityFile*f=opaque;if(!f||!f->file||at>f->bytes||bytes>f->bytes-at)return 0;
 if(!bytes)return write&&!EFI_ERROR(f->file->Flush(f->file));
 EFI_STATUS s=f->file->SetPosition(f->file,at);if(EFI_ERROR(s))return 0;UINTN n=bytes;
 s=write?f->file->Write(f->file,&n,data):f->file->Read(f->file,&n,data);
 if(EFI_ERROR(s)||n!=bytes)return 0;
 return !write||!EFI_ERROR(f->file->Flush(f->file));
}
void utility_file_close(UtilityFile*f){if(f->file)f->file->Close(f->file);f->file=NULL;}
