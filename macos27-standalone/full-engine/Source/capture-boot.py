from pathlib import Path
import subprocess,socket,json,time,re
r=Path(__file__).resolve().parent;w=r.parent;fw=w/'darwin-vm/firmware';sock=r/'reference-qmp.sock';sock.unlink(missing_ok=True)
args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-aarch64'),'-M','darwin','-accel','tcg,thread=single','-bootkc',str(fw/'bootkc'),'-dtree',str(fw/'dtree'),'-tc',str(fw/'ramdisk.tc'),'-ramdisk',str(fw/'ramdisk.dmg'),'-sptm',str(fw/'sptm'),'-txm',str(fw/'txm'),'-args','rd=md0 serial=3 -v -noprogress wdt=-1 wlan-olyhal-abort','-display','none','-serial','file:'+str(r/'reference-serial.log'),'-m','8G','-S','-qmp','unix:'+str(sock)+',server=on,wait=off']
with (r/'reference-qemu.log').open('w') as log:
 p=subprocess.Popen(args,stdout=log,stderr=log)
 try:
  deadline=time.monotonic()+20
  while not sock.exists():
   if p.poll() is not None:raise RuntimeError((r/'reference-qemu.log').read_text())
   if time.monotonic()>deadline:raise RuntimeError('QMP startup timeout')
   time.sleep(.1)
  with socket.socket(socket.AF_UNIX) as s:
   s.connect(str(sock));f=s.makefile('rwb',buffering=0);print(json.loads(f.readline())['QMP']['version'])
   def call(name,arguments=None):
    msg={'execute':name};
    if arguments:msg['arguments']=arguments
    f.write((json.dumps(msg)+'\n').encode())
    while True:
     result=json.loads(f.readline())
     if 'error' in result:raise RuntimeError(result)
     if 'return' in result:return result['return']
   call('qmp_capabilities')
   regs=call('human-monitor-command',{'command-line':'info registers'});(r/'reference-reset-registers.txt').write_text(regs);print(regs[:1800])
   mtree=call('human-monitor-command',{'command-line':'info mtree'});(r/'reference-memory-map.txt').write_text(mtree)
   pc=int(re.search(r'PC=([0-9a-fA-F]+)',regs)[1],16);x0=int(re.search(r'X00=([0-9a-fA-F]+)',regs)[1],16)
   # The segment parser below derives the SPTM's entry offset from the exact
   # pinned image; no guessed host physical address is dereferenced.
   import struct,hashlib
   data=(fw/'sptm').read_bytes();count=struct.unpack_from('<I',data,16)[0];at=32;base=2**64-1;entry=None
   for _ in range(count):
    kind,size=struct.unpack_from('<II',data,at)
    if kind==0x19:base=min(base,struct.unpack_from('<Q',data,at+24)[0])
    if kind==5:entry=struct.unpack_from('<Q',data,at+272)[0]
    at+=size
   assert entry is not None
   physical=pc-(entry-base);end=(x0+16384+16383)&~16383;length=end-physical
   assert physical%16384==0 and 0<length<64*1024*1024
   snapshot=r/'reference-boot-state.bin';call('pmemsave',{'val':physical,'size':length,'filename':str(snapshot)})
   metadata={'source':'Paused qemu-sptm reset state, before any guest execution','guestBase':physical,'guestBytes':length,'PC':pc,'X0':x0,'PSTATE':0x400003c9,'sha256':hashlib.sha256(snapshot.read_bytes()).hexdigest(),'scope':'SPTM-to-boot-args initialized span only, not complete RAM or bootable board'}
   (r/'REFERENCE-BOOT.json').write_text(json.dumps(metadata,indent=2)+'\n');print(metadata)
   digest=bytes.fromhex(metadata['sha256']);(r/'reference_boot.h').write_text('/* Private locally generated reference capture metadata. */\n#define REF_BASE '+hex(physical)+'ULL\n#define REF_BYTES '+str(length)+'ULL\n#define REF_PC '+hex(pc)+'ULL\n#define REF_X0 '+hex(x0)+'ULL\nstatic const uint8_t ref_sha256[32]={'+','.join(str(x) for x in digest)+'};\n')
   call('quit')
  p.wait(timeout=5)
 finally:
  if p.poll() is None:p.terminate();p.wait(timeout=5)
