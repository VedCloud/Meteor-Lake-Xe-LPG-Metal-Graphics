from pathlib import Path
import subprocess,json
p=Path(__file__).resolve().parent
fixture=(p/'translator/translator/tests/latitude_a8.rs').read_text().split('const FRAG:&str=r#"',1)[1].split('"#;',1)[0]
s=r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include <string.h>
typedef int(*translate_fn)(const char*,const char*,const char*,uint8_t**,size_t*,char*,size_t);
typedef void(*free_fn)(uint8_t*,size_t);
static void check(translate_fn fn,free_fn fr,const char*air,unsigned mask,int old){
 char json[256],err[2048]={0};snprintf(json,sizeof(json),"{\"constants\":{},\"samplers\":{},\"a8_targets\":%u}",mask);uint8_t*b=0;size_t len=0;
 int rc=fn(air,"fragment",json,&b,&len,err,sizeof(err));if(rc)fprintf(stderr,"%s\n",err);assert(!rc&&b&&len%4==0);
 uint32_t*w=(uint32_t*)b;assert(w[0]==0x07230203&&w[3]<1000000);unsigned n=w[3];int*loc=malloc(n*sizeof(int));unsigned char*alpha=calloc(n,1);for(unsigned j=0;j<n;j++)loc[j]=-1;
 for(size_t i=5;i<len/4;){unsigned count=w[i]>>16,op=w[i]&65535;assert(count&&i+count<=len/4);uint32_t*a=w+i+1;if(op==71&&count>=4&&a[1]==30){assert(a[0]<n);loc[a[0]]=a[2];}if(op==79&&count==9){assert(a[1]<n);alpha[a[1]]=a[4]==3&&a[5]==3&&a[6]==3&&a[7]==3;}i+=count;}
 unsigned checked=0;for(size_t i=5;i<len/4;){unsigned count=w[i]>>16,op=w[i]&65535;uint32_t*a=w+i+1;if(op==62){assert(a[0]<n&&a[1]<n);if(loc[a[0]]>=0){assert(alpha[a[1]]==(!old&&((mask>>loc[a[0]])&1)));checked++;}}i+=count;}
 assert(checked==2);free(loc);free(alpha);fr(b,len);
}
int main(int argc,char**argv){assert(argc==3);const char*air=AIR_LITERAL;
 for(int k=1;k<=2;k++){void*h=dlopen(argv[k],RTLD_NOW|RTLD_LOCAL);if(!h)fprintf(stderr,"%s\n",dlerror());assert(h);translate_fn f=(translate_fn)dlsym(h,"nvmtl_translate_samplers");free_fn fr=(free_fn)dlsym(h,"nvmtl_translate_free");assert(f&&fr);for(unsigned mask=0;mask<4;mask++)check(f,fr,air,mask,k==2);dlclose(h);}
 puts("PASS actual x86_64 translator dylibs through exported C ABI: JSON A8 mask reaches selected output stores; original 0.3.20 negative control lacks the correction.");}
'''.replace('AIR_LITERAL',json.dumps(fixture))
f=p/'build/test-ffi.c';f.write_text(s);subprocess.run(['xcrun','clang','-arch','x86_64','-O1',str(f),'-o',str(p/'build/test-ffi')],check=True)
subprocess.run([str(p/'build/test-ffi'),str(p/'build/NVMTLDriver.bundle/Contents/MacOS/libnvmtl_translate.dylib'),str(Path(json.loads((p/'PROVENANCE.json').read_text())['backup'])/'NVMTLDriver.bundle/Contents/MacOS/libnvmtl_translate.dylib')],check=True)
