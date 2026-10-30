# SPDX-License-Identifier: MIT
"""Bounded read-only live WindowServer/process observer for the owned candidate."""
from pathlib import Path
import json,time,subprocess,datetime
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);end=time.monotonic()+1800;last=None
with (d/'WINDOWSERVER-STATE.jsonl').open('a') as out:
 while time.monotonic()<end:
  try:
   p=subprocess.run(['/Volumes/Dell27Build/deps/bin/python3',str(r/'inspect-live-processes.py')],capture_output=True,text=True,timeout=15,check=True);x=json.loads(p.stdout);proc=x.get('currentProcess');ws=[q for q in x['processes'] if q['name']=='WindowServer'];state={'UTC':x['UTC'],'PC':x['PC'],'currentProcess':proc,'WindowServerProcesses':ws,'compositorState':x.get('compositorState')}
   out.write(json.dumps(state)+'\n');out.flush()
   if state['compositorState'] or (ws and last!=ws):print(json.dumps(state),flush=True)
   last=ws
  except (OSError,ValueError,subprocess.SubprocessError) as e:print('Observer read incomplete:',str(e),flush=True)
  time.sleep(10)
print('Bounded read-only observer finished; boot left running.',flush=True)
