from owned_monitor import owned_monitor
"""Read only selected timer/GIC fields from this owned outer firmware guest."""
from pathlib import Path
import socket,json,re,subprocess,datetime
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);args=json.loads((d/'valid/COMMAND.json').read_text());monitor=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0];layout=json.loads((r.parent/'FramebufferBuild/References/timer-layout-probe.json').read_text());syms={}
for line in subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--defined-only',str(r/'UnicornTest.elf')],text=True).splitlines():
 m=re.match(r'([0-9a-f]+) \w (.*)',line)
 if m:syms[m[2]]=int(m[1],16)
entry=int(re.search(r'EFI entry ([0-9a-fA-F]+)',(d/'valid/serial.log').read_text(errors='replace'))[1],16);base=entry-syms['efi_main']
with owned_monitor(d) as s:
 s.connect(monitor);s.settimeout(.12)
 def receive():
  data=b''
  while True:
   try:data+=s.recv(65536)
   except socket.timeout:return data.decode(errors='replace')
 def call(c):s.sendall((c+'\n').encode());return receive()
 def read(addr,width=8):
  assert 0x100000<=addr<12*1024**3
  out=call('xp /1'+('g' if width==8 else 'w')+'x '+hex(addr));m=re.search(r'[0-9a-fA-F]+:\s+0x([0-9a-fA-F]+)',out)
  if not m:raise RuntimeError(out[-1000:])
  return int(m[1],16)
 receive();response=call('qom-get /machine/unattached/device[0] tsc-frequency');lines=response.splitlines();value=next((int(x.strip()) for x in lines if x.strip().isdigit()),None);calibrated=read(base+syms['tsc_hz']);result={'scope':'Read-only outer QEMU advertised TSC rate versus firmware calibration; no clock changed','calibratedTSC_Hz':calibrated,'advertisedTSC_Hz':value,'relativeError':(calibrated-value)/value if value else None}
(d/'LIVE-CLOCK-CALIBRATION.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
