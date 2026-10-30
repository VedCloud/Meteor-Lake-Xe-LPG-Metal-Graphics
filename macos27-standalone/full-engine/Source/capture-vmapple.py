from pathlib import Path
import subprocess,socket,json,time,re,argparse,hashlib
parser=argparse.ArgumentParser();parser.add_argument("--installer",action="store_true");options=parser.parse_args()
def file_sha(path):
 h=hashlib.sha256()
 with path.open('rb') as f:
  for block in iter(lambda:f.read(8*1024*1024),b''):h.update(block)
 return h.hexdigest()
r=Path(__file__).resolve().parent;w=r.parent
if options.installer:
 r=r/'InstallerExperiment';r.mkdir(exist_ok=True)
 import shutil
 shutil.copy2(r.parent/'reference-16GiB.dtree',r/'reference-16GiB.dtree')
fw=w/'darwin-vm/firmware';sock=r/'vmapple-reference-qmp.sock';sock.unlink(missing_ok=True)
args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-aarch64'),'-M','darwin','-accel','tcg,thread=single','-bootkc',str(w/'InstallerInputs/kernelcache.vma2.macho'),'-dtree',str(r/'reference-16GiB.dtree'),'-tc',str(w/'InstallerInputs/BaseSystem/basesystem-26A428.trustcache') if options.installer else str(fw/'ramdisk.tc'),'-ramdisk',str(w/'InstallerInputs/BaseSystem/BaseSystem-26A428.apfs') if options.installer else str(fw/'ramdisk.dmg'),'-args','rd=md0 serial=3 -v -noprogress wdt=-1 wlan-olyhal-abort','-display','none','-serial','file:'+str(r/'vmapple-reference-serial.log'),'-m','16G','-S','-qmp','unix:'+str(sock)+',server=on,wait=off']
with (r/'vmapple-reference-qemu.log').open('w') as log:
 p=subprocess.Popen(args,stdout=log,stderr=log)
 try:
  deadline=time.monotonic()+20
  while not sock.exists():
   if p.poll() is not None:raise RuntimeError((r/'vmapple-reference-qemu.log').read_text())
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
   regs=call('human-monitor-command',{'command-line':'info registers'});(r/'vmapple-reference-reset-registers.txt').write_text(regs);print(regs[:1800])
   mtree=call('human-monitor-command',{'command-line':'info mtree'});(r/'vmapple-reference-memory-map.txt').write_text(mtree)
   pc=int(re.search(r'PC=([0-9a-fA-F]+)',regs)[1],16);x0=int(re.search(r'X00=([0-9a-fA-F]+)',regs)[1],16)
   # The segment parser below derives the SPTM's entry offset from the exact
   # pinned image; no guessed host physical address is dereferenced.
   import struct,hashlib
   data=(w/'InstallerInputs/kernelcache.vma2.macho').read_bytes();count=struct.unpack_from('<I',data,16)[0];at=32;base=2**64-1;entry=None
   for _ in range(count):
    kind,size=struct.unpack_from('<II',data,at)
    if kind==0x19:base=min(base,struct.unpack_from('<Q',data,at+24)[0])
    if kind==5:entry=struct.unpack_from('<Q',data,at+272)[0]
    at+=size
   assert entry is not None
   physical=pc-(entry-base)
   handoff=r/'vmapple-bootargs.bin';call('pmemsave',{'val':x0,'size':4096,'filename':str(handoff)})
   hb=handoff.read_bytes();virt,phys,mem=struct.unpack_from('<QQQ',hb,8)
   top=struct.unpack_from('<Q',hb,32)[0];assert phys<=top<=phys+mem
   kernel_physical=physical;physical=phys # TrustCache and other initialized data can precede the kernel
   dtva=struct.unpack_from('<Q',hb,96)[0];dtlen=struct.unpack_from('<I',hb,104)[0]
   assert dtva>=virt and 0<dtlen<1024*1024
   dtpa=dtva-virt+phys;assert physical<=dtpa<phys+mem and dtpa+dtlen<=phys+mem
   end=(max(x0+16384,dtpa+dtlen,top)+16383)&~16383;length=end-physical
   assert physical%16384==0 and 0<length<(3*1024**3 if options.installer else 768*1024*1024)
   snapshot=r/'vmapple-reference-boot-state.bin';call('pmemsave',{'val':physical,'size':length,'filename':str(snapshot)})
   # Counter frequency must come from this captured platform, not a guessed
   # physical Intel clock. qemu-sptm uses cpu0 clock-frequency as CNTFRQ.
   tree=(r/'reference-16GiB.dtree').read_bytes();frequency=[]
   def parse_node(at):
    np,nc=struct.unpack_from('<II',tree,at);at+=8;props={}
    for i in range(np):
     name=tree[at:at+32].split(b'\0')[0];size=struct.unpack_from('<I',tree,at+32)[0]&0x7fffffff;at+=36;props[name]=tree[at:at+size];at+=(size+3)&~3
    if props.get(b'name',b'').split(b'\0')[0]==b'cpu0':frequency.append(struct.unpack('<I',props[b'clock-frequency'])[0])
    for i in range(nc):at=parse_node(at)
    return at
   parse_node(0);assert len(frequency)==1 and 0<frequency[0]<=1000000000
   assert mem==16*1024**3, 'Reference boot arguments must match the requested 16 GiB profile'
   metadata={'RAMBase':phys,'RAMBytes':mem,'initializedTop':top,'deviceTreePhysical':dtpa,'deviceTreeBytes':dtlen,'counterFrequencyHz':frequency[0],'source':'Paused qemu-sptm reset state, before any guest execution','guestBase':physical,'kernelPhysicalBase':kernel_physical,'guestBytes':length,'PC':pc,'X0':x0,'PSTATE':0x400003c9,'sha256':file_sha(snapshot),'scope':'All initialized RAM through topOfKernelData, including kernel, boot arguments, M2 device tree, trust cache and ramdisk; uninitialized RAM is demand-zero; board remains unqualified'}
   (r/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(metadata,indent=2)+'\n');print(metadata)
   digest=bytes.fromhex(metadata['sha256']);(r/'vmapple_reference_boot.h').write_text('/* Private locally generated reference capture metadata. */\n#define VMREF_RAM_BASE '+hex(phys)+'ULL\n#define VMREF_RAM_BYTES '+hex(mem)+'ULL\n#define VMREF_CNTFRQ '+str(frequency[0])+'U\n#define VMREF_BASE '+hex(physical)+'ULL\n#define VMREF_BYTES '+str(length)+'ULL\n#define VMREF_PC '+hex(pc)+'ULL\n#define VMREF_X0 '+hex(x0)+'ULL\nstatic const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in digest)+'};\n')
   call('quit')
  p.wait(timeout=5)
 finally:
  if p.poll() is None:p.terminate();p.wait(timeout=5)
