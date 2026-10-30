# SPDX-License-Identifier: MIT
"""Bounded selected thread-stack observer for the owned WindowServer only."""
from pathlib import Path
import json,time,subprocess
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);end=time.monotonic()+86400;last=None
with (d/'WINDOWSERVER-THREADS-HISTORY.jsonl').open('a') as out:
 while time.monotonic()<end:
  command=json.loads((d/'valid/COMMAND.json').read_text());monitor=command[command.index('-monitor')+1].split('unix:',1)[1].split(',')[0]
  if not Path(monitor).exists():break
  if 'BSD root: md0' not in (d/'valid/serial.log').read_text(errors='replace'):
   time.sleep(10);continue
  try:
   p=subprocess.run(['/Volumes/Dell27Build/deps/bin/python3',str(r/'inspect-live-windowserver-threads.py')],capture_output=True,text=True,check=True,timeout=15);x=json.loads(p.stdout);state={'UTC':x['UTC'],'WindowServerState':x['WindowServerState'],'WindowServerThreads':x['WindowServerThreads']};out.write(json.dumps(state)+'\n');out.flush()
   key=json.dumps(state['WindowServerState'],sort_keys=True)
   if key!=last and state['WindowServerState'] and 'incomplete' not in state['WindowServerState']:print(json.dumps(state),flush=True)
   last=key
  except (OSError,ValueError,subprocess.SubprocessError) as e:print('Read-only snapshot incomplete:',str(e),flush=True)
  time.sleep(30)
print('Thread observer finished; boot left unchanged.',flush=True)
