from pathlib import Path
import subprocess,json,concurrent.futures,hashlib
w=Path(__file__).resolve().parent.parent;u=w/'framebuffer-engine';out=Path(__file__).resolve().parent;clang='/Library/Developer/CommandLineTools/usr/bin/clang'
def engine_state():
 changed=subprocess.check_output(['git','diff','--name-only','HEAD'],cwd=u,text=True).splitlines()
 changed+=subprocess.check_output(['git','ls-files','--others','--exclude-standard'],cwd=u,text=True).splitlines()
 return {'revision':subprocess.check_output(['git','rev-parse','HEAD'],cwd=u,text=True).strip(),
         'modifiedFiles':{n:hashlib.sha256((u/n).read_bytes()).hexdigest() for n in sorted(set(changed)) if (u/n).is_file()}}
before_engine=engine_state()
sources=[]
for inf in ['UnicornEngineLib.inf','UnicornArm64Lib.inf','UnicornStubLib.inf']:
 active=False
 for line in (u/'efi'/inf).read_text().splitlines():
  line=line.strip()
  if line.startswith('['):active=line in ['[Sources]','[Sources.X64]'];continue
  if active and line and not line.startswith('#'):sources.append((inf,u/'efi'/line))
incs=[u/'efi/include',u/'efi/include/x86_64',u,u/'include',u/'qemu',u/'qemu/include',u/'qemu/tcg',u/'qemu/tcg/i386',u/'glib_compat',w/'edk2/MdePkg/Include',w/'edk2/MdePkg/Include/X64']
base=[clang,'-target','x86_64-unknown-linux-gnu','-std=gnu11','-ffreestanding','-fshort-wchar','-mno-red-zone','-fno-stack-protector','-fno-builtin','-O1','-nostdinc','-DDELL27_ALWAYS_ON_ASSERTS','-DNO_MSABI_VA_FUNCS','-DEFIAPI=__attribute__((ms_abi))','-fPIC','-fno-asynchronous-unwind-tables','-DUNICORN_FOR_EFI','-DUNICORN_HAS_ARM64','-DUNICORN_FOR_EFI_INTERNAL','-Wno-macro-redefined','-Wno-incompatible-pointer-types','-Wno-unused-command-line-argument']
for inc in incs:base+=['-I',str(inc)]
def compile(item):
 inf,src=item;name=inf+'-'+str(src.resolve().relative_to(u)).replace('/','_');args=base.copy()
 if inf=='UnicornArm64Lib.inf':args+=['-I',str(u/'qemu/target/arm'),'-include','aarch64.h','-DCONFIG_TARGET_HEADER="config-target-aarch64.h"','-DNEED_CPU_H']
 source_hash=hashlib.sha256(src.read_bytes()).hexdigest()
 p=subprocess.run(args+['-c',str(src),'-o',str(out/(name+'.obj'))],capture_output=True,text=True)
 (out/(name+'.log')).write_text(p.stderr)
 assert source_hash==hashlib.sha256(src.read_bytes()).hexdigest(),'source changed during compile'
 return {'sourceSHA256':source_hash,'objectSHA256':hashlib.sha256((out/(name+'.obj')).read_bytes()).hexdigest() if p.returncode==0 else None,'source':str(src.relative_to(u)),'exit':p.returncode,'object':name+'.obj','log':name+'.log'}
with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:results=list(pool.map(compile,sources))
(out/'COMPILE.json').write_text(json.dumps(results,indent=2)+'\n')
passed=sum(r['exit']==0 for r in results);print('Compiled',passed,'of',len(results))
for r in results:
 if r['exit']:print(r['source'],(out/r['log']).read_text().splitlines()[-8:]);break

if passed!=len(results):raise SystemExit(1)
assert engine_state()==before_engine,'engine headers/sources changed during compile'
(out/'ENGINE-COMPILE.json').write_text(json.dumps(before_engine,indent=2)+'\n')
