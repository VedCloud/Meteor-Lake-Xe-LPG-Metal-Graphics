from owned_monitor import owned_monitor
# SPDX-License-Identifier: MIT
"""Read-only selected process metadata in the owned test's kernel memory.
Offsets come from disassembly of the exact supplied 26A428 kernel. Does not
execute guest code, modify RAM, read PAC keys, or export unrelated user data. Selected code and frame pointers
may be read to identify the currently executing stack.
"""
from pathlib import Path
import socket,re,json,subprocess,struct,datetime
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);args=json.loads((d/'valid/COMMAND.json').read_text());monitor=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0]
slide_path=d/'DYLD-SLIDE.json';live_slide=json.loads(slide_path.read_text())['slide'] if slide_path.exists() else None
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
 receive();call('stop')
 try:
  uc=u64(base+syms['guest_ram']);cpu=u64(uc+400);env=cpu+38800
  ttbr0=u64(env+0x308);ttbr1=u64(env+0x328);tcr=u64(env+0x358);tsz=(tcr>>16)&63;assert (tcr>>30)&3==1
  bits=64-tsz;mask=(1<<bits)-1
  def canonical(p):return (p&mask)|(((1<<64)-1)^mask) if p else 0
  # Read only region descriptors; never export raw region contents.
  mapped=u64(uc+0x760);count=struct.unpack('<I',read(uc+0x768,4))[0];assert count<1024
  regions=[]
  for i in range(count):
   mr=u64(mapped+8*i);desc=read(mr,0x48)
   if desc[0]!=1:continue
   rb=struct.unpack_from('<Q',desc,8)[0];size,addr=struct.unpack_from('<QQ',desc,0x30)[0],struct.unpack_from('<Q',desc,0x40)[0];host=u64(rb+8)
   regions.append((addr,size,host))
  def armread(pa,n):
   for addr,size,host in regions:
    if addr<=pa and pa+n<=addr+size:return read(host+pa-addr,n)
   raise ValueError('unbacked ARM physical address '+hex(pa))
  def translate(va):
   upper=bool(va>>63);n_bits=64-((tcr>>16)&63 if upper else tcr&63);granule=(tcr>>30)&3 if upper else (tcr>>14)&3;assert granule==(1 if upper else 2)
   level=4-((n_bits-14+10)//11);table=(ttbr1 if upper else ttbr0)&0x0000ffffffffc000
   while level<=3:
    shift=14+11*(3-level);pte=struct.unpack('<Q',armread(table+8*((va>>shift)&0x7ff),8))[0];assert pte&1,('unmapped kernel VA',hex(va),level)
    if level==3 or not pte&2:
     m=(1<<shift)-1;return (pte&0x0000ffffffffc000&~m)|(va&m)
    table=pte&0x0000ffffffffc000;level+=1
   raise ValueError('no leaf')
  def virtual(va,n):
   out=b''
   while n:
    part=min(n,0x4000-(va&0x3fff));out+=armread(translate(va),part);va+=part;n-=part
   return out
  def v64(va):return struct.unpack('<Q',virtual(va,8))[0]
  kp=canonical(v64(0xfffffe00079b9b88));processes=[];seen=set();p=kp
  for i in range(256):
   if not p or p in seen:break
   seen.add(p);pid=struct.unpack('<I',virtual(p+0x60,4))[0];name=virtual(p+0x5bc,17).split(b'\0',1)[0].decode(errors='replace')
   processes.append({'address':hex(p),'pid':pid,'name':name});p=canonical(v64(p+8))
   # le_prev points at the predecessor's next field (offset zero), or the
   # static list head. Stop before interpreting a static head as a proc.
   if p and p<0xfffffe1000000000:break
  current=canonical(u64(env+1304));current_name_ptr=canonical(v64(current+0x830)) if current else 0
  current_name=virtual(current_name_ptr,64).split(b'\0',1)[0].decode(errors='replace') if current_name_ptr else ''
  # Exact 26A428 _current_proc/_current_task: thread+0x3f0 is its
  # read-only metadata; +0 verifies the owner, +0x18 proc, +0x28 task.
  current_process=None
  try:
   ro=canonical(v64(current+0x3f0))
   if canonical(v64(ro))==current:
    cp=canonical(v64(ro+0x18));ct=canonical(v64(ro+0x28))
    if not cp:cp=kp
    if ct==cp+0x7c0:
     current_process={'address':hex(cp),'task':hex(ct),'pid':int.from_bytes(virtual(cp+0x60,4),'little'),'name':virtual(cp+0x5bc,17).split(b'\0',1)[0].decode(errors='replace'),'basis':'Exact-build thread read-only owner and embedded proc/task relationship verified'}
  except (ValueError,AssertionError):pass
  is_user=u64(env+320)<0x8000000000000000
  return_mask=(1<<(64-(tcr&63)))-1 if is_user else mask
  def code_pointer(p):return p&return_mask if is_user else canonical(p)
  fp=code_pointer(u64(env+296));frames=[];frame_seen=set()
  for i in range(24):
   if not fp or fp in frame_seen:break
   frame_seen.add(fp)
   try:
    next_fp=code_pointer(v64(fp));lr=code_pointer(v64(fp+8));frames.append({'frame':hex(fp),'returnPC':hex(lr)})
   except (ValueError,AssertionError):break
   if next_fp<=fp or next_fp-fp>0x40000:break
   fp=next_fp
  # The exact supplied exception-entry code loads machine.upcb at
  # thread+0x100 (diversifier 0x1e91). Only selected public saved-state
  # metadata/return addresses are exported; general registers are excluded.
  saved_user_state=None
  if current_process and current_process['pid']>0:
   try:
    pcb=canonical(v64(current+0x100));flavor,count=struct.unpack('<II',virtual(pcb,8))
    tail=virtual(pcb+0xf0,0x24);saved_fp,saved_lr,saved_sp,saved_pc,saved_cpsr=struct.unpack('<QQQQI',tail)
    umask=(1<<(64-(tcr&63)))-1
    if flavor==21 and 68<=count<=80 and not saved_cpsr&15 and 0<(saved_pc&umask)<0x8000000000:
     saved_user_state={'scope':'Last kernel-saved user context of verified current thread; may be stale while EL0 executes. No general registers or user stack payload read.','PC':hex(saved_pc&umask),'LR':hex(saved_lr&umask),'FP':hex(saved_fp&umask),'SP':hex(saved_sp&umask),'CPSR':hex(saved_cpsr),'flavor':flavor,'count':count}
     try:saved_user_state['instructionBytes']=virtual(saved_pc&umask,64).hex()
     except (ValueError,AssertionError):pass
   except (ValueError,AssertionError):pass
  sampled_pc=u64(env+320);instruction_hex=None;disassembly=[]
  try:
   raw_code=virtual(sampled_pc,64);instruction_hex=raw_code.hex()
   from capstone import Cs,CS_ARCH_ARM64,CS_MODE_ARM
   disassembly=[hex(i.address)+' '+i.mnemonic+' '+i.op_str for i in Cs(CS_ARCH_ARM64,CS_MODE_ARM).disasm(raw_code,sampled_pc)]
  except (ValueError,AssertionError,ImportError):pass
  compositor_state=None
  if is_user and live_slide is not None and current_process and current_process['name']=='WindowServer':
   try:
    slide=live_slide
    compositor_state={'scope':'Current process verified as WindowServer from exact-build thread metadata; current user task BSS only','slide':hex(slide),'metalCapability':virtual(0x1c39d44a8+slide,1)[0],'capabilityOnce':hex(v64(0x1c39d44b0+slide)),'activeCompositor':int.from_bytes(virtual(0x1c39d3df4+slide,4),'little'),'defaultToMetalFlag':virtual(0x1c39d3e40+slide,1)[0],'timeoutBranchWord':virtual(0x18708b618+slide,4).hex()}
   except (ValueError,AssertionError):pass
  result={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(d),'EFI_SHA256':a['EFI_SHA256'],'scope':'Read-only selected proc list/name/pid fields from exact-build disassembly; list may be partial if kernproc is not linked. Selected executing code, return addresses and WindowServer state only; no PAC keys or unrelated process payload exported.','compositorState':compositor_state,'PC':hex(sampled_pc),'instructionBytes':instruction_hex,'disassembly':disassembly,'backtraceScope':'user return-address chain' if is_user else 'kernel return-address chain','kernelBacktrace':frames,'savedUserState':saved_user_state,'currentProcess':current_process,'currentThread':hex(current),'currentThreadName':current_name,'kernelProc':hex(kp),'kernelProcPrevLink':hex(v64(kp+8)),'processes':processes,'endedAt':hex(p),'regionCount':len(regions)}
 finally:call('cont')
path=d/('LIVE-PROCESSES-'+datetime.datetime.now(datetime.timezone.utc).strftime('%H%M%S')+'.json');path.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
