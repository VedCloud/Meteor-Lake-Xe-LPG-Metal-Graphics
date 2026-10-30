from owned_monitor import owned_monitor
"""Read-only HMP sampling of the nested x86 firmware test, not Dell performance."""
from pathlib import Path
import socket,json,re,time,subprocess,bisect,collections
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);command=json.loads((d/'valid/COMMAND.json').read_text());print(type(command).__name__)
args=command if isinstance(command,list) else command.get('args',command.get('command'))
if not isinstance(args,list):raise SystemExit(str(command)[:700])
monitor=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0]
syms=[]
for l in subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--numeric-sort','--print-size','--defined-only',str(r/'UnicornTest.elf')],text=True).splitlines():
 m=re.match(r'([0-9a-f]+) ([0-9a-f]+) [Tt] (.*)',l)
 if m:syms.append((int(m[1],16),int(m[2],16),m[3]))
entry=next(x[0] for x in syms if x[2]=='efi_main');text=(d/'valid/serial.log').read_text(errors='replace');base=int(re.search(r'EFI entry ([0-9a-fA-F]+)',text)[1],16)-entry
samples=[]
with owned_monitor(d) as s:
 s.connect(monitor);s.settimeout(.08)
 def receive():
  data=b''
  while True:
   try:data+=s.recv(65536)
   except socket.timeout:return data.decode(errors='replace')
 receive()
 for i in range(64):
  s.sendall(b'info registers\n');out=receive();m=re.search(r'RIP=([0-9a-fA-F]+)',out)
  if m:
   pc=int(m[1],16);rel=pc-base;found=next((n for at,size,n in syms if at<=rel<at+size),'outside sized static firmware functions');samples.append({'RIP':hex(pc),'symbol':found})
  time.sleep(.08)
report={'scope':'read-only sampled outer firmware PCs in nested QEMU, not physical Dell','run':str(d),'EFI_SHA256':a['EFI_SHA256'],'samples':samples,'counts':dict(collections.Counter(x['symbol'] for x in samples))}
(d/'FIRMWARE-PC-SAMPLES.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report['counts'],indent=2))
