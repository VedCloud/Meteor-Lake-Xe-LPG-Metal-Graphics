# SPDX-License-Identifier: MIT
"""Build the diagnostic EFI artifact. Does not install it or execute guest code."""
from pathlib import Path
import os,subprocess,hashlib,json
r=Path(__file__).resolve().parent;w=r.parent
clang=os.environ.get('D27_CLANG','/Library/Developer/CommandLineTools/usr/bin/clang')
lld=os.environ.get('D27_LLD',str(w/'deps/bin/lld-link'))
flags=['-target','x86_64-pc-windows-msvc','-ffreestanding','-fshort-wchar','-mno-red-zone','-fno-stack-protector','-fno-builtin','-O2','-Wall','-Wextra','-Werror','-I',str(w/'edk2/MdePkg/Include'),'-I',str(w/'edk2/MdePkg/Include/X64')]
for source in ['preflight','boot_input']:subprocess.run([clang,*flags,'-c',str(r/(source+'.c')),'-o',str(r/(source+'.obj'))],check=True)
subprocess.run([lld,'/subsystem:efi_application','/entry:efi_main','/nodefaultlib','/machine:x64','/out:'+str(r/'Dell27Preflight.efi'),str(r/'preflight.obj'),str(r/'boot_input.obj')],check=True)
report={'artifact':'Dell27Preflight.efi','SHA256':hashlib.sha256((r/'Dell27Preflight.efi').read_bytes()).hexdigest(),'edk2Revision':subprocess.check_output(['git','rev-parse','HEAD'],cwd=w/'edk2',text=True).strip(),'standaloneEFI':True,'kernelExecution':False}
(r/'BUILD.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
