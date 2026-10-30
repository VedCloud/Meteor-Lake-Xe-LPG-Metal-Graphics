from owned_monitor import owned_monitor
# SPDX-License-Identifier: MIT
"""Watch actual owned VM pixels and runtime milestones; does not classify UI as installer."""
from pathlib import Path
import json,time,socket,subprocess,datetime,hashlib
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);log=Path(a['log']);args=json.loads((d/'valid/COMMAND.json').read_text());mon=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0];tmp=d/'display-watch.ppm';printed=set();next_capture=0
print('Watching actual display:',d,flush=True)
while time.monotonic():
 text=log.read_text(errors='replace');runtime=text.split('Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay',1)[-1] if 'beginning sparse-RAM VMAPPLE kernel replay' in text else ''
 for mark in ['Successfully spawned WindowServer','DIAGNOSTIC: genuine no-Metal timeout','VMAPPLE kernel panic format:','FATAL exception:']:
  if mark in runtime and mark not in printed:print('RUNTIME MILESTONE:',mark,flush=True);printed.add(mark)
 if 'DONE engine integration tests' in text:print('Replay finished; display still needs visual verification.',flush=True);break
 if 'GOP logging: firmware text output suppressed' in runtime and time.monotonic()>=next_capture:
  next_capture=time.monotonic()+30
  try:
   with owned_monitor(d) as s:
    s.settimeout(3);s.connect(mon)
    def recv():
     b=b''
     while not b.endswith(b'(qemu) '):
      part=s.recv(65536)
      if not part:raise OSError('monitor closed')
      b+=part
     return b
    recv();s.sendall(('screendump '+str(tmp)+'\n').encode());recv()
   data=tmp.read_bytes();magic,geom,limit,pixels=data.split(b'\n',3);assert magic==b'P6' and limit==b'255'
   if pixels.strip(b'\0'):
    stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ');ppm=d/('display-change-'+stamp+'.ppm');tmp.rename(ppm);png=ppm.with_suffix('.png');subprocess.run(['/usr/bin/sips','-s','format','png',str(ppm),'--out',str(png)],check=True,capture_output=True)
    result={'scope':'Actual framebuffer changed from black; visual inspection required. No installer claim from pixels alone.','UTC':stamp,'run':str(d),'png':str(png),'PPM_SHA256':hashlib.sha256(data).hexdigest(),'installerVerified':False};(d/'DISPLAY-CHANGE.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2),flush=True);break
   tmp.unlink()
  except (OSError,ValueError,AssertionError) as e:
   if not Path(mon).exists():print('Owned monitor unavailable; watcher stopped:',str(e),flush=True);break
 time.sleep(3)
