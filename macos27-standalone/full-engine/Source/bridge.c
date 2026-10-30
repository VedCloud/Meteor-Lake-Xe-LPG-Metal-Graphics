/* SPDX-License-Identifier: MIT
 * Real UEFI service bridge for the separately licensed Unicorn engine.
 * Internal C/varargs use SysV; EFIAPI entry points use MS ABI. */
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Protocol/SerialIo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "firmware-memory.h"
#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#include "nanoprintf.h"
EFI_BOOT_SERVICES *gBS;
static EFI_SYSTEM_TABLE *table;
static uint64_t tsc_hz;
static int graphics_muted;
static void graphics_output(const char*);
UINT32 PcdMask=1; /* Enable the upstream jump code's CET state handling. */
static void output(const char *s){if(graphics_muted){graphics_output(s);return;}CHAR16 line[128];UINTN n=0;while(*s){if(*s=='\n')line[n++]='\r';line[n++]=(UINT8)*s++;if(n>124){line[n]=0;table->ConOut->OutputString(table->ConOut,line);n=0;}}if(n){line[n]=0;table->ConOut->OutputString(table->ConOut,line);}}
static void fatal(const char*s){output("FATAL firmware bridge: ");output(s);output("\n");for(;;)__asm__ volatile("cli; hlt");}
VOID *EFIAPI CopyMem(VOID*d,CONST VOID*s,UINTN n){return d27_memory_copy(d,s,n);}
VOID *EFIAPI SetMem(VOID*d,UINTN n,UINT8 c){return d27_memory_set(d,n,c);}
INTN EFIAPI CompareMem(CONST VOID*a,CONST VOID*b,UINTN n){const unsigned char*x=a,*y=b;for(UINTN i=0;i<n;i++)if(x[i]!=y[i])return (INTN)x[i]-(INTN)y[i];return 0;}
UINTN EFIAPI AsciiStrLen(CONST CHAR8*s){UINTN n=0;while(s[n])n++;return n;}
INTN EFIAPI AsciiStrCmp(CONST CHAR8*a,CONST CHAR8*b){while(*a&&*a==*b){a++;b++;}return (unsigned char)*a-(unsigned char)*b;}
CHAR8 *EFIAPI AsciiStrStr(CONST CHAR8*s,CONST CHAR8*t){if(!*t)return (CHAR8*)s;for(;*s;s++){UINTN i=0;while(t[i]&&s[i]==t[i])i++;if(!t[i])return (CHAR8*)s;}return NULL;}
char*strcpy(char*d,const char*s){char*r=d;while((*d++=*s++)) {}return r;}
char*strcat(char*d,const char*s){strcpy(d+AsciiStrLen(d),s);return d;}
char*strncpy(char*d,const char*s,size_t n){size_t i=0;for(;i<n&&s[i];i++)d[i]=s[i];for(;i<n;i++)d[i]=0;return d;}
int strncmp(const char*a,const char*b,size_t n){for(size_t i=0;i<n;i++){if(a[i]!=b[i])return (unsigned char)a[i]-(unsigned char)b[i];if(!a[i])break;}return 0;}
char*strstr(const char*s,const char*t){return AsciiStrStr(s,t);}
static void swap(unsigned char*a,unsigned char*b,size_t n){while(n--){unsigned char t=*a;*a++=*b;*b++=t;}}
static void down(unsigned char*a,size_t root,size_t n,size_t size,int(*cmp)(const void*,const void*)){while(root<n/2){size_t child=root*2+1;if(child+1<n&&cmp(a+child*size,a+(child+1)*size)<0)child++;if(cmp(a+root*size,a+child*size)>=0)return;swap(a+root*size,a+child*size,size);root=child;}}
void qsort(void*base,size_t n,size_t size,int(*cmp)(const void*,const void*)){if(n<2)return;if(!base||!size||n>SIZE_MAX/size||!cmp)fatal("invalid qsort bounds");unsigned char*a=base;for(size_t i=n/2;i>0;i--)down(a,i-1,n,size,cmp);for(size_t i=n-1;i>0;i--){swap(a,a+i*size,size);down(a,0,i,size,cmp);}}
void*bsearch(const void*key,const void*base,size_t n,size_t size,int(*cmp)(const void*,const void*)){if(!size||n>SIZE_MAX/size||!cmp||(!base&&n))return NULL;const unsigned char*a=base;while(n){size_t i=n/2;int c=cmp(key,a+i*size);if(!c)return(void*)(a+i*size);if(c>0){a+=(i+1)*size;n-=i+1;}else n=i;}return NULL;}
VOID*EFIAPI AllocatePages(UINTN pages){if(!pages||pages>MAX_UINTN/4096)return NULL;EFI_PHYSICAL_ADDRESS p=0;EFI_STATUS s=gBS->AllocatePages(AllocateAnyPages,EfiBootServicesData,pages,&p);return EFI_ERROR(s)?NULL:(VOID*)(UINTN)p;}
VOID EFIAPI FreePages(VOID*p,UINTN pages){if(!p||!pages||EFI_ERROR(gBS->FreePages((UINTN)p,pages)))fatal("FreePages failed");}
UINT64 EFIAPI GetPerformanceCounter(VOID){UINT32 lo,hi;__asm__ volatile("lfence; rdtsc":"=a"(lo),"=d"(hi)::"memory");return ((UINT64)hi<<32)|lo;}
__uint128_t __udivmodti4(__uint128_t,__uint128_t,__uint128_t*);
__uint128_t __udivti3(__uint128_t n,__uint128_t d){return __udivmodti4(n,d,NULL);}
UINT64 EFIAPI GetTimeInNanoSecond(UINT64 ticks){if(!tsc_hz)fatal("timer not initialized");__uint128_t ns=__udivmodti4((__uint128_t)ticks*1000000000,tsc_hz,NULL);if(ns>MAX_UINT64)return MAX_UINT64;return (UINT64)ns;}
/* Keep va_copy in a SysV function: Clang selects the MS intrinsic inside
 * an ms_abi function even when its parameter is a SysV va_list. */
static __attribute__((noinline)) UINTN format_list(char*buf,UINTN n,const char*fmt,va_list list,int length_only){
 va_list copy;va_copy(copy,list);int result=npf_vsnprintf(buf,n,fmt,copy);va_end(copy);
 if(result<0)return 0;
 if(length_only)return (UINTN)result;
 return n?((UINTN)result<n?(UINTN)result:n-1):0;
}
UINTN EFIAPI AsciiVSPrint(CHAR8*buf,UINTN n,CONST CHAR8*fmt,VA_LIST list){return format_list(buf,n,fmt,list,0);}
UINTN EFIAPI SPrintLengthAsciiFormat(CONST CHAR8*fmt,VA_LIST list){return format_list(NULL,0,fmt,list,1);}
BOOLEAN EFIAPI DebugAssertEnabled(VOID){return TRUE;}
BOOLEAN EFIAPI DebugPrintEnabled(VOID){return TRUE;}
BOOLEAN EFIAPI DebugPrintLevelEnabled(CONST UINTN level){return (level&DEBUG_ERROR)!=0;}
VOID EFIAPI DebugVPrint(UINTN level,CONST CHAR8*fmt,VA_LIST args){if(!(level&DEBUG_ERROR))return;char normalized[1024],buf[2048];size_t i=0;for(;fmt[i]&&i+1<sizeof(normalized);i++)normalized[i]=(fmt[i]=='a'&&i&&fmt[i-1]=='%')?'s':fmt[i];normalized[i]=0;AsciiVSPrint(buf,sizeof(buf),normalized,args);output(buf);}
/* DebugPrint is a true MS-varargs EFI API. Build its MS list directly; these
 * formats come from assertions and do not use the engine's SysV va_list. */
VOID EFIAPI DebugPrint(UINTN level,CONST CHAR8*fmt,...){
 if(!(level&DEBUG_ERROR))return;
 __builtin_ms_va_list ms;__builtin_ms_va_start(ms,fmt);
 /* SysV's overflow-only va_list reads the same eight-byte integer/pointer
  * slots as MS x64 varargs. No aggregate or long-double formats are exposed. */
 struct {unsigned gp,fp;void*stack;void*save;} layout={48,304,ms,NULL};
 va_list args;_Static_assert(sizeof(args)==sizeof(layout),"unexpected SysV va_list");
 CopyMem(&args,&layout,sizeof(args));DebugVPrint(level,fmt,args);
 __builtin_ms_va_end(ms);
}
VOID EFIAPI DebugAssert(CONST CHAR8*file,UINTN line,CONST CHAR8*description){char buf[512];npf_snprintf(buf,sizeof(buf),"%s:%llu %s",file,(unsigned long long)line,description);fatal(buf);}
VOID EFIAPI InternalAssertJumpBuffer(BASE_LIBRARY_JUMP_BUFFER*p){if(!p||((UINTN)p&7))fatal("invalid jump buffer");}
VOID EFIAPI InternalLongJump(BASE_LIBRARY_JUMP_BUFFER*,UINTN) __attribute__((noreturn));
VOID EFIAPI LongJump(BASE_LIBRARY_JUMP_BUFFER*p,UINTN value){InternalAssertJumpBuffer(p);if(!value)fatal("zero longjmp value");InternalLongJump(p,value);}
void bridge_init(EFI_SYSTEM_TABLE*t){table=t;gBS=t->BootServices;UINT64 before=GetPerformanceCounter();EFI_STATUS s=gBS->Stall(10000);UINT64 after=GetPerformanceCounter();if(EFI_ERROR(s)||after<=before||after-before>MAX_UINT64/100)fatal("timer calibration failed");tsc_hz=(after-before)*100;}
void bridge_output(const char*s){output(s);}

/* Calendar is sampled once; subsequent virtual RTC reads use the calibrated
 * monotonic TSC, never repeated firmware calls or physical RTC writes. */
uint64_t bridge_video_now_ns(void){return GetTimeInNanoSecond(GetPerformanceCounter());}
static uint64_t rtc_epoch_us,rtc_tsc;
static int leap(unsigned y){return y%4==0&&(y%100!=0||y%400==0);}
static int calendar_us(const EFI_TIME*t,uint64_t*out){
 static const unsigned days[]={31,28,31,30,31,30,31,31,30,31,30,31};
 if(t->Year<1970||t->Year>9999||!t->Month||t->Month>12||!t->Day||t->Day>days[t->Month-1]+(t->Month==2&&leap(t->Year))||t->Hour>23||t->Minute>59||t->Second>59||t->Nanosecond>=1000000000)return 0;
 if(t->TimeZone!=EFI_UNSPECIFIED_TIMEZONE&&(t->TimeZone < -1440||t->TimeZone > 1440))return 0;
 int64_t n=0;for(unsigned y=1970;y<t->Year;y++)n+=365+leap(y);
 for(unsigned m=1;m<t->Month;m++)n+=days[m-1]+(m==2&&leap(t->Year));
 int64_t seconds=((n+t->Day-1)*24+t->Hour)*3600+t->Minute*60+t->Second;
 if(t->TimeZone!=EFI_UNSPECIFIED_TIMEZONE)seconds-=(int64_t)t->TimeZone*60;
 if(seconds<0)return 0;*out=(uint64_t)seconds*1000000+t->Nanosecond/1000;return 1;
}
int bridge_rtc_initialize(void){EFI_TIME time;EFI_STATUS status=table->RuntimeServices->GetTime(&time,NULL);if(EFI_ERROR(status)||!calendar_us(&time,&rtc_epoch_us))return 0;rtc_tsc=GetPerformanceCounter();output(time.TimeZone==EFI_UNSPECIFIED_TIMEZONE?"VMAPPLE RTC: firmware calendar, timezone unspecified; monotonic progression\n":"VMAPPLE RTC: firmware calendar normalized to UTC; monotonic progression\n");return 1;}
uint64_t bridge_rtc_clock(void*unused){(void)unused;return rtc_epoch_us+GetTimeInNanoSecond(GetPerformanceCounter()-rtc_tsc)/1000;}

void bridge_address(const char *label,const void *p){char b[128];npf_snprintf(b,sizeof(b),"%s %p\n",label,p);output(b);}

/* Keep firmware ConOut from drawing over the guest-owned GOP surface. */
static EFI_SERIAL_IO_PROTOCOL*graphics_serial;
static EFI_FILE_PROTOCOL*graphics_log;
static char graphics_buffer[4096];static UINTN graphics_used;
static void graphics_flush(void){if(graphics_log&&graphics_used){UINTN n=graphics_used;EFI_STATUS s=graphics_log->Write(graphics_log,&n,graphics_buffer);if(EFI_ERROR(s)||n!=graphics_used){graphics_log->Close(graphics_log);graphics_log=NULL;}graphics_used=0;}}
static void graphics_output(const char*s){UINTN total=AsciiStrLen(s);if(graphics_serial){UINTN at=0;while(at<total){UINTN n=total-at;EFI_STATUS status=graphics_serial->Write(graphics_serial,&n,(VOID*)(s+at));if(EFI_ERROR(status)||!n){graphics_serial=NULL;break;}at+=n;}}if(graphics_log){for(UINTN i=0;i<total;i++){graphics_buffer[graphics_used++]=s[i];if(graphics_used==sizeof(graphics_buffer))graphics_flush();if(!graphics_log)break;}}}
int bridge_graphics_start(EFI_HANDLE image){
 EFI_GUID serial_guid=EFI_SERIAL_IO_PROTOCOL_GUID,loaded_guid=EFI_LOADED_IMAGE_PROTOCOL_GUID,fs_guid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
 table->BootServices->LocateProtocol(&serial_guid,NULL,(VOID**)&graphics_serial);
 EFI_LOADED_IMAGE_PROTOCOL*loaded=NULL;EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*fs=NULL;EFI_FILE_PROTOCOL*root=NULL;
 EFI_STATUS s=table->BootServices->HandleProtocol(image,&loaded_guid,(VOID**)&loaded);
 if(!EFI_ERROR(s))s=table->BootServices->HandleProtocol(loaded->DeviceHandle,&fs_guid,(VOID**)&fs);
 if(!EFI_ERROR(s))s=fs->OpenVolume(fs,&root);
 if(!EFI_ERROR(s))s=root->Open(root,&graphics_log,L"\\EngineProbe\\Dell27-Framebuffer.log",EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE,0);
 if(root)root->Close(root);
 if(!EFI_ERROR(s))s=graphics_log->SetPosition(graphics_log,MAX_UINT64);
 if(EFI_ERROR(s)&&graphics_log){graphics_log->Close(graphics_log);graphics_log=NULL;}
 if(!graphics_serial&&!graphics_log)return 0;
 graphics_muted=1;graphics_output("\nBEGIN Dell27 framebuffer recovery experiment\n");return 1;
}
void bridge_finish_output(void){graphics_flush();if(graphics_log){graphics_log->Flush(graphics_log);graphics_log->Close(graphics_log);graphics_log=NULL;}}

static int compare_ints(const void*a,const void*b){int x=*(const int*)a,y=*(const int*)b;return (x>y)-(x<y);}
static UINTN format_sysv(char*buffer,UINTN n,const char*fmt,...){va_list a;va_start(a,fmt);UINTN result=AsciiVSPrint(buffer,n,fmt,a);va_end(a);return result;}
int bridge_selftest(void){
 EFI_TIME cal={0};uint64_t date;cal.Year=2000;cal.Month=1;cal.Day=1;cal.TimeZone=0;
 if(!calendar_us(&cal,&date)||date!=946684800000000ULL)return 0;
 cal.TimeZone=60;if(!calendar_us(&cal,&date)||date!=946681200000000ULL)return 0;
 cal.Year=2100;cal.Month=2;cal.Day=29;if(calendar_us(&cal,&date))return 0;
 cal.Year=2000;cal.Day=29;cal.TimeZone=0;if(!calendar_us(&cal,&date)||date!=951782400000000ULL)return 0;

 char text[128];if(format_sysv(text,sizeof(text),"%s %llu %x %d","format",0x100000002ULL,0xbeef,-42)!=sizeof("format 4294967298 beef -42")-1||AsciiStrCmp(text,"format 4294967298 beef -42"))return 0;
 char small[4];if(format_sysv(small,sizeof(small),"%s","abcdef")!=3||AsciiStrCmp(small,"abc"))return 0;
 char overlap[8]="abcdef";CopyMem(overlap+1,overlap,5);if(CompareMem(overlap,"aabcde",6))return 0;
 int array[]={8,1,5,-3,2,7,0,4,6};qsort(array,9,sizeof(int),compare_ints);for(unsigned i=1;i<9;i++)if(array[i]<array[i-1])return 0;
 int key=5;if(!bsearch(&key,array,9,sizeof(int),compare_ints))return 0;key=99;if(bsearch(&key,array,9,sizeof(int),compare_ints))return 0;
 VOID*p=AllocatePages(1);if(!p||((UINTN)p&4095))return 0;SetMem(p,4096,0xa5);if(((UINT8*)p)[4095]!=0xa5)return 0;FreePages(p,1);
 BASE_LIBRARY_JUMP_BUFFER jump;volatile UINTN value=SetJump(&jump);if(!value)LongJump(&jump,42);if(value!=42)return 0;
 UINT64 ticks=GetPerformanceCounter();if(EFI_ERROR(gBS->Stall(1000)))return 0;if(GetTimeInNanoSecond(GetPerformanceCounter()-ticks)<500000)return 0;
 DebugPrint(DEBUG_ERROR,"BRIDGE MSABI format: %a %llu %x %d\n","ok",0x100000002ULL,0xbeef,-42);
 return 1;
}
