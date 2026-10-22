from pathlib import Path
import tempfile,subprocess,os
r=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'bin').mkdir();state=p/'settings';state.write_text('0')
 for n,body in {'uname':'echo x86_64','id':'echo 0','pmset':'''if [ "$1" = -g ]; then printf 'Battery Power:\\n lowpowermode %s\\nAC Power:\\n lowpowermode 0\\n' "$(cat "$MOCK_STATE")"; elif [ "$1" = -b ]; then [ "$2" = lowpowermode ] || exit 9; printf '%s' "$3" > "$MOCK_STATE"; else exit 9; fi'''}.items():
  f=p/'bin'/n;f.write_text('#!/bin/bash\n'+body+'\n');f.chmod(0o755)
 s=(r/'power-sources/configure-battery.sh').read_text().replace("STATE='/Library/Application Support/LatitudeInput'","STATE='"+str(p/'saved')+"'").replace('/usr/bin/pmset','pmset')
 f=p/'profile.sh';f.write_text(s);env={**os.environ,'PATH':str(p/'bin')+':/usr/bin:/bin','MOCK_STATE':str(state)}
 for mode,want in [('apply','1'),('apply','1'),('restore','0')]:
  c=subprocess.run(['/bin/bash',str(f),mode],env=env,capture_output=True,text=True);assert c.returncode==0,c.stdout+c.stderr;assert state.read_text()==want
 state.write_text('0');assert subprocess.run(['/bin/bash',str(f),'restore'],env=env,capture_output=True).returncode==2
print('PASS actual battery profile apply/readback/idempotence/restore and changed-setting refusal; no host power settings touched.')
