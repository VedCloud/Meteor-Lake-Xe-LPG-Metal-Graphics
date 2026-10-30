from owned_monitor import owned_monitor
# SPDX-License-Identifier: MIT
"""Read-only selected process metadata in the owned test's kernel memory.
Offsets come from disassembly of the exact supplied 26A428 kernel. Does not
execute guest code, modify RAM, read PAC keys, or read process address spaces.
"""
from pathlib import Path
import socket,re,json,subprocess,struct,datetime
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);args=json.loads((d/'valid/COMMAND.json').read_text());monitor=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0]
syms={}
for line in subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--defined-only',str(r/'UnicornTest.elf')],text=True).splitlines():
 m=re.match(r'([0-9a-f]+) \w (.*)',line)
 if m:syms[m[2]]=int(m[1],16)
entry=int(re.search(r'EFI entry ([0-9a-fA-F]+)',(d/'valid/serial.log').read_text(errors='replace'))[1],16);base=entry-syms['efi_main']
with owned_monitor(d) as s:
 s.connect(monitor);s.settimeout(5)
 def receive():
  out=b''
  while not out.endswith(b'(qemu) '):
   b=s.recv(65536)
   if not b:raise RuntimeError('monitor closed')
   out+=b
  return out.decode(errors='replace')
 def call(c):s.sendall((c+'\n').encode());return receive()
 def read(addr,n):
  assert 0x1000<=addr and 0<n<=4096 and addr+n<=13*1024**3, ('outer physical bounds',hex(addr),n)
  out=call('xp /'+str(n)+'bx '+hex(addr));b=bytearray()
  for line in out.splitlines():
   if re.match(r'^[0-9a-fA-F]+:',line):b.extend(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]{2})\b',line.split(':',1)[1]))
  if len(b)!=n:raise RuntimeError('short physical read '+str((hex(addr),n,len(b))))
  return bytes(b)
 def u64(addr):return struct.unpack('<Q',read(addr,8))[0]
 receive();snapshots=[]
 for i in range(8):
  call('stop')
  try:
   p=base+syms['root_device'];b=read(p,408)
   fields={'reads':struct.unpack_from('<Q',b,376)[0],'failed':struct.unpack_from('<Q',b,384)[0],'notifications':struct.unpack_from('<Q',b,392)[0],'invalid':struct.unpack_from('<I',b,400)[0],'size':struct.unpack_from('<H',b,72)[0],'lastAvail':struct.unpack_from('<H',b,74)[0],'usedIndex':struct.unpack_from('<H',b,76)[0],'vector':struct.unpack_from('<H',b,78)[0],'pending':b[82],'status':b[83],'isr':b[84],'enabled':b[85],'MSIXControl':hex(struct.unpack_from('<H',b,86+0xc2)[0])}
   fields['role']='ROOT';fields['appleType']=struct.unpack('<I',read(p+456,4))[0]
   ap=base+syms['aux_device'];ab=read(ap,464)
   aux={'role':'AUX','appleType':struct.unpack_from('<I',ab,456)[0],'bytes':struct.unpack_from('<Q',ab,16)[0],'reads':struct.unpack_from('<Q',ab,376)[0],'writes':struct.unpack_from('<Q',ab,448)[0],'failed':struct.unpack_from('<Q',ab,384)[0],'notifications':struct.unpack_from('<Q',ab,392)[0],'invalid':struct.unpack_from('<I',ab,400)[0],'usedIndex':struct.unpack_from('<H',ab,76)[0],'pending':ab[82],'status':ab[83],'isr':ab[84],'enabled':ab[85]}
   snapshots.append({'root':fields,'aux':aux})
  finally:call('cont')
  import time;time.sleep(.5)
 result={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(d),'scope':'Read-only owned virtual block device counters and public PCI transport state. No disk payload or keys read.','snapshots':snapshots}
(d/'LIVE-VIRTIO.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
