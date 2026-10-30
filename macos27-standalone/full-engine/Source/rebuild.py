# SPDX-License-Identifier: MIT
"""Rebuild engine and EDK2 context-switch assembly in the current tool layout."""
from pathlib import Path
import subprocess,sys
r=Path(__file__).resolve().parent;w=r.parent
nasm=w/'deps/bin/nasm';edk=w/'edk2'
for unit in ['SetJump','LongJump']:
 subprocess.run([str(nasm),'-f','elf64','-I',str(edk/'MdePkg/Include/X64')+'/', '-p',str(r/'autogen.inc'),str(edk/'MdePkg/Library/BaseLib/X64'/f'{unit}.nasm'),'-o',str(r/f'{unit}.o')],check=True)
subprocess.run([sys.executable,str(r/'compile.py')],check=True)
subprocess.run([sys.executable,str(r/'link.py'),*sys.argv[1:]],check=True)
