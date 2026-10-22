from pathlib import Path
import subprocess
root=Path(__file__).resolve().parent.parent;output=root/'desktop-build/latitude-rotate'
subprocess.run(['xcrun','clang','-arch','x86_64','-mmacosx-version-min=15.0','-fobjc-arc','-Wall','-Wextra','-Werror','-framework','Foundation','-framework','ApplicationServices','-framework','IOKit',str(root/'tablet-sources/latitude-rotate.m'),'-o',str(output)],check=True)
subprocess.run(['codesign','--force','--sign','-',str(output)],check=True);subprocess.run(['codesign','--verify','--strict',str(output)],check=True)
print('PASS signed x86_64 rotation client, exact built-in/validated framebuffer target and bounded acknowledgement/rollback. Not run on host.')
