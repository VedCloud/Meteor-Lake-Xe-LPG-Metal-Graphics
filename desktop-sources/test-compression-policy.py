from pathlib import Path
import tempfile,subprocess
r=Path(__file__).resolve().parent.parent
code='''#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nvmtl_desktop_policy.h"
int main(void){
 unsetenv("INTEL_DEBUG");assert(nvmtl_windowserver_disable_ccs(0x8086,0x7d55,"WindowServer")==1);assert(!strcmp(getenv("INTEL_DEBUG"),"noccs"));
 setenv("INTEL_DEBUG","",1);assert(nvmtl_windowserver_disable_ccs(0x8086,0x7d55,"WindowServer")==1);assert(!strcmp(getenv("INTEL_DEBUG"),"noccs"));
 setenv("INTEL_DEBUG","perf",1);assert(nvmtl_windowserver_disable_ccs(0x8086,0x7d55,"WindowServer")==1);assert(!strcmp(getenv("INTEL_DEBUG"),"perf,noccs"));
 const char *names[]={"normal-app","windowserver",NULL};for(int i=0;i<3;i++){assert(!nvmtl_windowserver_disable_ccs(0x8086,0x7d55,names[i]));assert(!strcmp(getenv("INTEL_DEBUG"),"perf,noccs"));}
 assert(!nvmtl_windowserver_disable_ccs(0x8086,0x7d55,"WindowServer"));assert(!nvmtl_windowserver_disable_ccs(0x10de,0x7d55,"WindowServer"));
 char huge[5000];memset(huge,'x',4999);huge[4999]=0;setenv("INTEL_DEBUG",huge,1);assert(nvmtl_windowserver_disable_ccs(0x8086,0x7d55,"WindowServer")==-E2BIG);assert(strlen(getenv("INTEL_DEBUG"))==4999);
 puts("PASS actual compression policy: exact process/vendor/device scope, existing flags preserved, unset/empty handled, oversized policy rejected without mutation");}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.c').write_text(code);subprocess.run(['xcrun','clang','-g','-fsanitize=address,undefined','-I',str(r/'NullMoth/plugin'),str(p/'test.c'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
s=(r/'NullMoth/plugin/nvmtl_vk.c').read_text();a=s.index('static int nvmtl_vk_initialize(void)');b=s.index('static pthread_once_t g_vk_once',a);s=s[a:b];assert s.index('nvmtl_windowserver_disable_ccs(')<s.index('g_lib = dlopen(')
print('PASS policy is applied before ICD dlopen / process-once debug initialization')
