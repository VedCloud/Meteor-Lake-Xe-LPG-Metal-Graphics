#include "frame-equal.h"
uint64_t bridge_video_now_ns(void);
int gop_comparison_uses_sse2(void);
static int benchmark_scan(EFI_SYSTEM_TABLE*t){
 EFI_PHYSICAL_ADDRESS left=0,right=0;
 if(!gop_comparison_uses_sse2())return 0;
 if(EFI_ERROR(t->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,256,&left)))return 0;
 if(EFI_ERROR(t->BootServices->AllocatePages(AllocateAnyPages,EfiLoaderData,256,&right))){t->BootServices->FreePages(left,256);return 0;}
 unsigned char*a=(void*)(UINTN)left,*b=(void*)(UINTN)right;for(UINTN i=0;i<1048576;i++)a[i]=b[i]=(unsigned char)i;
 volatile unsigned result=0;uint64_t elapsed[6];
 for(unsigned trial=0;trial<6;trial++){
  uint64_t start=bridge_video_now_ns();
  for(unsigned repeat=0;repeat<32;repeat++){
   __asm__ volatile("":::"memory");
   result+=trial%3==0?d27_frame_equal_scalar(a,b,1048576):trial%3==1?d27_frame_equal_unrolled(a,b,1048576):d27_frame_equal_sse2(a,b,1048576);
  }
  elapsed[trial]=bridge_video_now_ns()-start;
 }
 char text[360];snprintf(text,sizeof(text),"GOP scan benchmark: bytes=1048576 iterations=32 scalar_ns=%llu unrolled_ns=%llu sse2_ns=%llu scalar_repeat_ns=%llu unrolled_repeat_ns=%llu sse2_repeat_ns=%llu\n",(unsigned long long)elapsed[0],(unsigned long long)elapsed[1],(unsigned long long)elapsed[2],(unsigned long long)elapsed[3],(unsigned long long)elapsed[4],(unsigned long long)elapsed[5]);bridge_output(text);
 t->BootServices->FreePages(left,256);t->BootServices->FreePages(right,256);return result==192;
}
