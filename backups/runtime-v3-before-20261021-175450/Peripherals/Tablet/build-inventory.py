from pathlib import Path
import subprocess
root=Path(__file__).resolve().parent.parent;output=root/'desktop-build/latitude-tablet-state'
subprocess.run(['xcrun','clang','-arch','x86_64','-mmacosx-version-min=15.0','-Wall','-Wextra','-Werror','-fobjc-arc','-framework','Foundation','-framework','IOKit','-framework','ApplicationServices',str(root/'tablet-sources/latitude-tablet-state.m'),'-o',str(output)],check=True)
subprocess.run(['codesign','--force','--sign','-',str(output)],check=True);subprocess.run(['codesign','--verify','--strict',str(output)],check=True)
print('PASS x86_64 inventory 0.3.0 compiled with warnings as errors; signature verified. No execution on host.')
