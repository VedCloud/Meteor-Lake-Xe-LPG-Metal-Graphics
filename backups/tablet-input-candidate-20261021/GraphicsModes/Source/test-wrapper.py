from pathlib import Path
import tempfile,subprocess,os
source=Path(__file__).parent/'install-windowserver-wrapper.sh'
with tempfile.TemporaryDirectory() as tmp:
 r=Path(tmp);(r/'Desktop').mkdir();(r/'install-windowserver.sh').write_bytes(source.read_bytes())
 (r/'Desktop/install-windowserver.sh').write_text('#!/bin/bash\nexit "$INSTALL_RESULT"\n')
 (r/'switch-graphics-mode.sh').write_text('#!/bin/bash\necho "$1" >> "'+str(r/'calls')+'"\nexit "$MODE_RESULT"\n')
 for install,mode in [(0,0),(7,0),(0,6)]:
  (r/'calls').unlink(missing_ok=True);env=os.environ.copy();env.update(INSTALL_RESULT=str(install),MODE_RESULT=str(mode));c=subprocess.run(['/bin/bash',str(r/'install-windowserver.sh')],env=env,capture_output=True,text=True)
  assert c.returncode==(install or mode),c.stdout+c.stderr
  assert (r/'calls').exists()==(install==0)
  if install==0:assert (r/'calls').read_text()=='accelerated\n'
print('PASS installer wrapper: persistent mode after successful install only; installation and mode-selection failures preserved.')
