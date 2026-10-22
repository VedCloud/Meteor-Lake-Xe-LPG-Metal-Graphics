from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/plugin/NVMTLDevice.m').read_text()
a=s.index('#define NVLOG_SITES');b=s.index('\nconst char *nvmtl_log_path',a);rate=s[a:b]
a=s.index('void nvmtl_desktop_trace(');b=s.index('\n#define NVMTL_OOM_STREAK',a);body=s[a:b]
pre='''#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include "nvmtl_log_failure.h"
#define NVMTL_RELEASE 1
#define LOG_ERR 3
static const char *process="WindowServer";
#define getprogname() process
static unsigned output,first,stage;static char last[1024];
static void syslog(int priority,const char *fmt,...){va_list ap;va_start(ap,fmt);vsnprintf(last,sizeof last,fmt,ap);va_end(ap);output++;if(strstr(last,"first-submit:"))first++;if(strstr(last,"desktop-trace:"))stage++;}
'''
post='''int main(void){
 for(int i=1;i<=24;i++)nvlog("first-submit: #%d queue result %d",i,0);
 assert(first==24&&strstr(last,"#24"));
 for(int i=0;i<300;i++)nvmtl_desktop_trace("queue-submit-begin","surface readback",0);assert(stage==256);
 unsigned before=output;process="normal-app";nvmtl_desktop_trace("should-not-appear","",0);assert(output==before);
 for(int i=0;i<24;i++)nvlog("first-submit: app normal throttle %d",i);assert(first==32);
 for(int i=0;i<24;i++)nvlog("vk: repeated FAIL %d",i);assert(output==before+16);
 before=output;nvlog("memory-report0.3.6: heap 0 bytes 8589934592 flags 1");assert(output==before+1);
 before=output;nvlog("pipeline-cache0.3.7: graphics result=0 elapsed_us=200 cache=1");assert(output==before+1);
 before=output;nvlog("publication0.3.15: surface=1 staged=1 prepare_ms=5 iosurface_lock_ms=0");assert(output==before+1);
 before=output;nvlog("glyph-layout0.3.15: process=Weather fmt=10 pitch=128");assert(output==before+1);
 before=output;nvlog("ordinary per-draw detail");assert(output==before);
 before=output;for(int i=0;i<64;i++)nvlog("publication0.3.15: bounded fixture %d",i);assert(output==before+64);
 before=output;for(int i=0;i<32;i++)nvlog("glyph-layout0.3.15: bounded fixture %d",i);assert(output==before+32);
 puts("PASS actual release logger: all24 finite desktop submissions survive site throttle,256-stage cap, non-WindowServer trace disabled, application/failure throttling preserved; no file-write path");}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.c').write_text(pre+rate+body+post)
 subprocess.run(['xcrun','clang','-g','-fsanitize=address,undefined','-I',str(r/'NullMoth/plugin'),str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
