"""Run the real collector against read-only command fixtures, not host PM."""
from pathlib import Path
import tempfile,subprocess,os
root=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);bin=p/'bin';bin.mkdir();commands=p/'commands';src=(root/'power-sources/collect-sleep.sh').read_text()
 for name in ['pmset','ioreg','kmutil','log','sw_vers','sysctl']:
  stub=bin/name;stub.write_text('#!/bin/bash\nprintf "%s\\n" "'+name+' $*" >> "$SLEEP_COMMANDS"\necho fixture-'+name+'\n');stub.chmod(0o755)
  src=src.replace('/usr/bin/'+name,str(stub)).replace('/usr/sbin/'+name,str(stub))
 uname=bin/'uname';uname.write_text('#!/bin/sh\necho x86_64\n');uname.chmod(0o755)
 script=p/'collect-sleep.sh';script.write_text(src);env=dict(os.environ,PATH=str(bin)+':'+os.environ['PATH'],SLEEP_COMMANDS=str(commands))
 for phase in ['before','after']:
  subprocess.run(['/bin/bash',str(script),phase],env=env,check=True,stdout=subprocess.DEVNULL)
  out=next((p/'reports').glob('*-'+phase));status=(out/'status.txt').read_text();assert status.count('exit=0')==11,status
 assert subprocess.run(['/bin/bash',str(script),'invalid'],env=env,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode==2
 calls=commands.read_text().splitlines()
 for line in calls:
  if line.startswith('pmset '):assert line.split()[1]=='-g',line
  assert 'sleepnow' not in line and 'disablesleep' not in line
 assert len(list((p/'reports').iterdir()))==2
print('PASS before/after sleep capture, status preservation, argument validation and read-only pmset queries. Host PM never queried.')
