/* SPDX-License-Identifier: MIT — real firmware presentation, no GPU identity. */
#include "glue.h"
#include <Protocol/GraphicsOutput.h>
#include <unicorn/unicorn.h>
#include <Library/BaseMemoryLib.h>
#include "firmware-memory.h"
void bridge_output(const char*);
int bridge_graphics_start(EFI_HANDLE);
uint64_t bridge_video_now_ns(void);
static EFI_GRAPHICS_OUTPUT_PROTOCOL*gop;
static EFI_SYSTEM_TABLE*system_table;
static void*shadow;static void*presented_frame;static UINTN pages;static unsigned width,height,stride;
static int presented_valid;
static uint64_t next_present;
static uc_engine*engine;
void gop_framebuffer_poll(void){
 if(!shadow)return;uint64_t now=bridge_video_now_ns();if(now<next_present)return;next_present=now+16666667;
 UINTN visible=(UINTN)stride*height;
 if(presented_valid){
  const d27_unaligned_word*a=shadow,*b=presented_frame;UINTN i=0;
  for(;i<visible/sizeof(*a);i++)if(a[i]!=b[i])break;
  if(i==visible/sizeof(*a))return;
 }
 /* The single guest CPU is suspended in this callback. Keep the submitted
  * copy outside guest RAM, so BLT consumes a stable snapshot of these pixels. */
 CopyMem(presented_frame,shadow,visible);
 EFI_STATUS s=gop->Blt(gop,presented_frame,EfiBltBufferToVideo,0,0,0,0,width,height,stride);
 if(EFI_ERROR(s)){presented_valid=0;bridge_output("FAIL firmware GOP presentation\n");uc_emu_stop(engine);}else presented_valid=1;
}
void gop_framebuffer_close(void){if(presented_frame){system_table->BootServices->FreePages((UINTN)presented_frame,pages);presented_frame=NULL;}if(shadow){system_table->BootServices->FreePages((UINTN)shadow,pages);shadow=NULL;}presented_valid=0;next_present=0;}
uc_err bind_gop_framebuffer(uc_engine*u,EFI_HANDLE image,EFI_SYSTEM_TABLE*t,uint64_t bootargs){
 EFI_GUID guid=EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;EFI_STATUS status=t->BootServices->LocateProtocol(&guid,NULL,(VOID**)&gop);
 if(EFI_ERROR(status)||!gop||!gop->Mode||!gop->Mode->Info)return UC_ERR_RESOURCE;
 EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*i=gop->Mode->Info;
 char line[220];snprintf(line,sizeof(line),"GOP hardware mode: %ux%u pixels-per-row=%u format=%u aperture=%llu\n",i->HorizontalResolution,i->VerticalResolution,i->PixelsPerScanLine,i->PixelFormat,(unsigned long long)gop->Mode->FrameBufferSize);bridge_output(line);
 if(!i->HorizontalResolution||!i->VerticalResolution||i->HorizontalResolution>8191||i->VerticalResolution>8192||i->PixelFormat>=PixelFormatMax)return UC_ERR_ARG;
 width=i->HorizontalResolution;height=i->VerticalResolution;stride=(width*4+63)&~63u;
 uint64_t visible=(uint64_t)stride*height;pages=(visible+128+4095)/4096;
 if(pages>64*1024*1024/4096)return UC_ERR_RESOURCE;
 EFI_PHYSICAL_ADDRESS address=0;status=t->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,pages,&address);if(EFI_ERROR(status))return UC_ERR_NOMEM;
 system_table=t;shadow=(void*)(UINTN)address;engine=u;memset(shadow,0,pages*4096);
 address=0;status=t->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,pages,&address);
 if(EFI_ERROR(status)){gop_framebuffer_close();return UC_ERR_NOMEM;}
 presented_frame=(void*)(UINTN)address;presented_valid=0;next_present=0;
 const uint64_t guest=0x600000000ULL;uc_err e=uc_mem_map_ptr(u,guest,pages*4096,UC_PROT_READ|UC_PROT_WRITE,shadow);if(e){gop_framebuffer_close();return e;}
 uint64_t video[6]={guest,1,stride,width,height,32},old[6];e=uc_mem_read(u,bootargs+40,old,sizeof(old));if(e)return e;for(unsigned n=0;n<6;n++)if(old[n])return UC_ERR_ARG;
 e=uc_mem_write(u,bootargs+40,video,sizeof(video));if(e)return e;
 /* GOP BLT takes BGRX independent of the physical mode's pixel order. The
  * extra aperture bytes the native framebuffer asks for remain owned RAM. */
 snprintf(line,sizeof(line),"GOP framebuffer bound: %ux%u stride=%u guest=0x%llx owned-bytes=%llu BGRX32; unaccelerated\n",width,height,stride,(unsigned long long)guest,(unsigned long long)pages*4096);bridge_output(line);
 if(!bridge_graphics_start(image))return UC_ERR_RESOURCE;
 bridge_output("GOP logging: firmware text output suppressed; serial/private file active\n");gop_framebuffer_poll();return UC_ERR_OK;
}
