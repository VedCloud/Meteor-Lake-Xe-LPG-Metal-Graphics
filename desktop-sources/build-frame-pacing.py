from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent
out=r/'latitude-desktop/latitude-frame-pacing'
subprocess.run(['xcrun','clang','-arch','arm64','-arch','x86_64','-mmacosx-version-min=15.0','-fobjc-arc','-fblocks','-Wall','-Wextra','-Werror','-O2',str(r/'desktop-sources/latitude-frame-pacing.m'),'-framework','Foundation','-framework','ScreenCaptureKit','-framework','CoreMedia','-framework','CoreGraphics','-o',str(out)],check=True)
subprocess.run(['codesign','--force','-s','-',str(out)],check=True)
for prefix in [[],['/usr/bin/arch','-x86_64']]:
 subprocess.run(prefix+[str(out),'--self-test'],check=True)
