from pathlib import Path
import tempfile,subprocess,json
p=Path(__file__).resolve().parent;block=(p/'INSTALLER-CALLBACK.sh').read_text();actual=(p.parent/'Tahoe-Combined-Runtime/install-windowserver.sh').read_text();assert block in actual
results=[]
with tempfile.TemporaryDirectory(prefix='dell-branding-callback-') as tmp:
 root=Path(tmp);here=root/'Graphics';(here/'Branding').mkdir(parents=True);report=root/'Report';report.mkdir();marker=root/'called'
 for mode,code in [('full',0),('full',42),('accelerator-only',42)]:
  if marker.exists():marker.unlink()
  stub=here/'Branding/install-about-this-mac.sh';stub.write_text('#!/bin/bash\nprintf called > '+repr(str(marker))+'\nexit '+str(code)+'\n')
  script=root/'test.sh';script.write_text('set -euo pipefail\nHERE='+repr(str(here))+'\nREPORT='+repr(str(report))+'\nMODE='+mode+'\nsuccess=1\n'+block+'\n[ "$success" = 1 ]\nexit 0\n')
  result=subprocess.run(['/bin/bash',str(script)],text=True,capture_output=True)
  assert result.returncode==(code if mode=='full' else 0),result.stdout+result.stderr
  assert marker.exists()==(mode=='full')
  if code==42 and mode=='full':assert 'Graphics installed successfully; About This Mac branding failed' in result.stdout
  results.append({'mode':mode,'stubExit':code,'actualExit':result.returncode})
(p/'CALLBACK-TESTS.json').write_text(json.dumps({'actualProductionBlockTested':True,'systemChangesMocked':True,'cases':results},indent=2)+'\n');print('PASS actual installer callback: successful branding, explicit cosmetic failure after graphics commit, and accelerator-only skip. No system writes.')
