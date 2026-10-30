/* SPDX-License-Identifier: MIT — bounded reference replay, not a Darwin board */
#include "glue.h"
#include <unicorn/unicorn.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include "../Standalone/boot_input.h"
#include "reference_boot.h"
void bridge_output(const char*);void bridge_address(const char*,const void*);
static void require(uc_err err,const char*where){if(err){bridge_output("FAIL reference replay ");bridge_output(where);bridge_output(uc_strerror(err));bridge_output("\n");abort();}}
static uint64_t visits,last_pc,bad_address;static uc_mem_type bad_type;
static void trace(uc_engine*uc,uint64_t pc,uint32_t size,void*data){(void)uc;(void)size;(void)data;visits++;last_pc=pc;}
static bool invalid_memory(uc_engine*uc,uc_mem_type type,uint64_t addr,int size,int64_t value,void*data){(void)uc;(void)size;(void)value;(void)data;bad_address=addr;bad_type=type;return false;}
static void cp(uc_engine*uc,unsigned op1,unsigned crn,unsigned crm,unsigned op2,uint64_t value){uc_arm64_cp_reg r={.op0=3,.op1=op1,.crn=crn,.crm=crm,.op2=op2,.val=value};require(uc_reg_write(uc,UC_ARM64_REG_CP_REG,&r),"set reset register");}
EFI_STATUS run_reference_entry(EFI_HANDLE image,EFI_SYSTEM_TABLE*t){
 EFI_GUID loaded_guid=EFI_LOADED_IMAGE_PROTOCOL_GUID,fs_guid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;EFI_LOADED_IMAGE_PROTOCOL*loaded=NULL;EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*fs=NULL;EFI_FILE_PROTOCOL*root=NULL,*file=NULL;void*bytes=NULL;
 EFI_STATUS s=t->BootServices->HandleProtocol(image,&loaded_guid,(VOID**)&loaded);
 if(!EFI_ERROR(s))s=t->BootServices->HandleProtocol(loaded->DeviceHandle,&fs_guid,(VOID**)&fs);
 if(!EFI_ERROR(s))s=fs->OpenVolume(fs,&root);
 if(!EFI_ERROR(s))s=root->Open(root,&file,L"\\EngineProbe\\reference-boot-state.bin",EFI_FILE_MODE_READ,0);
 if(!EFI_ERROR(s))s=t->BootServices->AllocatePool(EfiLoaderData,REF_BYTES,&bytes);
 if(!EFI_ERROR(s)){
  UINT64 size=0;s=file->SetPosition(file,MAX_UINT64);if(!EFI_ERROR(s))s=file->GetPosition(file,&size);
  if(!EFI_ERROR(s)&&size!=REF_BYTES)s=EFI_COMPROMISED_DATA;
  if(!EFI_ERROR(s))s=file->SetPosition(file,0);
  for(UINTN at=0;!EFI_ERROR(s)&&at<REF_BYTES;){UINTN got=REF_BYTES-at;if(got>65536)got=65536;UINTN wanted=got;s=file->Read(file,&got,(UINT8*)bytes+at);if(!EFI_ERROR(s)&&got!=wanted)s=EFI_DEVICE_ERROR;at+=got;}
 }
 if(file)file->Close(file);if(root)root->Close(root);
 if(!EFI_ERROR(s)){D27Sha256 hash;uint8_t digest[32];d27_sha_init(&hash);d27_sha_update(&hash,bytes,REF_BYTES);d27_sha_final(&hash,digest);if(memcmp(digest,ref_sha256,32))s=EFI_SECURITY_VIOLATION;}
 if(EFI_ERROR(s)){if(bytes)t->BootServices->FreePool(bytes);bridge_output("FAIL reference capture admission\n");return s;}
 bridge_output("Verified paused-reference memory capture; beginning bounded SPTM replay\n");
 uc_engine*uc=NULL;require(uc_open(UC_ARCH_ARM64,UC_MODE_ARM,&uc),"open");require(uc_ctl_set_cpu_model(uc,UC_CPU_ARM64_APPLE_SPRR),"CPU model");
 require(uc_mem_map(uc,0x800000000ULL,160*1024*1024,UC_PROT_ALL),"private RAM");require(uc_mem_write(uc,REF_BASE,bytes,REF_BYTES),"load actual reference state");t->BootServices->FreePool(bytes);
 cp(uc,6,1,1,0,0x501);cp(uc,0,1,0,2,0x300000);cp(uc,4,1,1,2,0x33ff);
 uint32_t pstate=0x400003c9;require(uc_reg_write(uc,UC_ARM64_REG_PSTATE,&pstate),"EL2 reset");uint64_t x0=REF_X0;require(uc_reg_write(uc,UC_ARM64_REG_X0,&x0),"actual x0");
 uc_hook hook1,hook2;require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,1,0),"trace hook");require(uc_hook_add(uc,&hook2,UC_HOOK_MEM_INVALID,invalid_memory,NULL,1,0),"fault hook");
 uc_err err=uc_emu_start(uc,REF_PC,0,0,1000000);uint64_t pc=0;require(uc_reg_read(uc,UC_ARM64_REG_PC,&pc),"final PC");
 char report[256];snprintf(report,sizeof(report),"SPTM replay stopped: %s; visited=%llu; pc=0x%llx; last=0x%llx; memory_type=%u; address=0x%llx\n",uc_strerror(err),(unsigned long long)visits,(unsigned long long)pc,(unsigned long long)last_pc,(unsigned)bad_type,(unsigned long long)bad_address);bridge_output(report);
 uint32_t instruction=0;if(!uc_mem_read(uc,pc,&instruction,4)){snprintf(report,sizeof(report),"Stopped instruction: 0x%08x\n",instruction);bridge_output(report);}
 require(uc_close(uc),"close");bridge_output("Reference replay is diagnostic evidence, not kernel boot.\n");return EFI_SUCCESS;
}
