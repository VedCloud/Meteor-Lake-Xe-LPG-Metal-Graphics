from owned_monitor import owned_monitor
# SPDX-License-Identifier: MIT
"""Read-only selected process metadata in the owned test's kernel memory.
Offsets come from disassembly of the exact supplied 26A428 kernel. Does not
execute guest code, modify RAM, read PAC keys, or export unrelated user data. Selected code and frame pointers
may be read to identify the currently executing stack.
"""
from pathlib import Path
import socket,re,json,subprocess,struct,datetime
import sys
target_process='WindowServer';target_pid=None
if len(sys.argv)>1:
 assert len(sys.argv) in (3,5) and sys.argv[1]=='--process'
 target_process=sys.argv[2];assert target_process in ('runningboardd','cfprefsd','syslogd','notifyd','opendirectoryd','dirhelper','lsd','launchservicesd','coreservicesd','securityd','trustd','xpcproxy','recoveryosd','applekeystored','Installer Progre','ditto','bash','launchd','pmset')
 if len(sys.argv)==5:
  assert sys.argv[3]=='--pid';target_pid=int(sys.argv[4]);assert 0<target_pid<2**31
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
  def translate(va, user_root=None):
   upper=bool(va>>63);n_bits=64-((tcr>>16)&63 if upper else tcr&63);granule=(tcr>>30)&3 if upper else (tcr>>14)&3;assert granule==(1 if upper else 2)
   level=4-((n_bits-14+10)//11);table=(ttbr1 if upper else ttbr0 if user_root is None else user_root)&0x0000ffffffffc000
   while level<=3:
    shift=14+11*(3-level);pte=struct.unpack('<Q',armread(table+8*((va>>shift)&0x7ff),8))[0];assert pte&1,('unmapped kernel VA',hex(va),level)
    if level==3 or not pte&2:
     m=(1<<shift)-1;return (pte&0x0000ffffffffc000&~m)|(va&m)
    table=pte&0x0000ffffffffc000;level+=1
   raise ValueError('no leaf')
  def virtual(va,n,user_root=None):
   out=b''
   while n:
    part=min(n,0x4000-(va&0x3fff));out+=armread(translate(va,user_root),part);va+=part;n-=part
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

  # Exact-kernel task_suspend iterates task+0x50 via thread+0x3e0.
  # _get_task_pmap reads task.map+0x28, vm_map.pmap+0x58. Validate
  # the pmap's first virtual/physical table pair before any user stack read.
  ws_threads=[];ws_state=None
  for wp in [p for p in processes if p['name']==target_process and (target_pid is None or p['pid']==target_pid)]:
   try:
    task=int(wp['address'],16)+0x7c0;vmmap=canonical(v64(task+0x28));pmap=canonical(v64(vmmap+0x58));root_va=canonical(v64(pmap));root_pa=v64(pmap+8)
    assert root_pa and not root_pa&0x3fff and translate(root_va)==root_pa,'pmap root pair mismatch'
    slide=live_slide;umask=(1<<(64-(tcr&63)))-1
    if slide is not None and target_process=='WindowServer':
     try:
      import hashlib
      signature=virtual(0x18708b508+slide,0x120,root_pa)
      assert hashlib.sha256(signature).hexdigest()=='0559ac041a11c161d2e181f41ba4459f471cec87c606816176344e8ce18b5b8d','WS address-space signature mismatch'
      ws_state={'pid':wp['pid'],'scope':'Selected WindowServer BSS read through its own verified virtual/physical page-table root; no guest modifications','slide':hex(slide),'metalCapability':virtual(0x1c39d44a8+slide,1,root_pa)[0],'capabilityOnce':hex(int.from_bytes(virtual(0x1c39d44b0+slide,8,root_pa),'little')),'activeCompositor':int.from_bytes(virtual(0x1c39d3df4+slide,4,root_pa),'little'),'defaultToMetalFlag':virtual(0x1c39d3e40+slide,1,root_pa)[0],'virtualConsoleOnly':virtual(0x1c39d33c3+slide,1,root_pa)[0],'HIDEventDriverPortPresent':int.from_bytes(virtual(0x1c39d42e8+slide,4,root_pa),'little')!=0}
      # CoreDisplay can still be unmapped during SkyLight/IOSurface startup.
      # Retain the separately signature-verified SkyLight state in that case.
      try:
       core_sig=virtual(0x1834fe648+slide,32,root_pa)
       assert hashlib.sha256(core_sig).hexdigest()=='5fe8dad42e24d07a476c5212d060a077a87916bb38aebbade73dc5ee76a7e7d8','CoreDisplay preference signature mismatch'
       ws_state['useGPUWrangler']=virtual(0x1c390ce88+slide,1,root_pa)[0]
      except (ValueError,AssertionError) as e:
       ws_state['CoreDisplayPreferenceStateIncomplete']=str(e)
     except (ValueError,AssertionError) as e:ws_state={'pid':wp['pid'],'incomplete':str(e),'scope':'Required code/state mapping not ready; no compositor result inferred'}
    if slide is not None:
     try:
      import hashlib
      assert hashlib.sha256(virtual(0x186e27148+slide,96,root_pa)).hexdigest()=='f17498657f30c48dc770166cb947b0ddad019c8da26f6a0e4db8afb7e7185efd'
      result['selectedServiceLegacyASL']={'scope':'Exact96B native ASL initializer signature verified; selected-service logger port presence only','serverPortPresent':int.from_bytes(virtual(0x1c44704a0+slide,4,root_pa),'little')!=0}
     except (ValueError,AssertionError):pass
    head=task+0x50;th=canonical(v64(head));seen_th=set()
    for _ in range(32):
     if th==head or not th or th in seen_th:break
     seen_th.add(th);ro=canonical(v64(th+0x3f0));assert canonical(v64(ro))==th and canonical(v64(ro+0x28))==task,'Selected service thread ownership mismatch'
     nameptr=canonical(v64(th+0x830));name=virtual(nameptr,64).split(b'\0',1)[0].decode(errors='replace') if nameptr else ''
     pcb=canonical(v64(th+0x100));flavor,count=struct.unpack('<II',virtual(pcb,8));assert flavor==21 and 68<=count<=80
     sfp,slr,ssp,spc,scpsr=struct.unpack('<QQQQI',virtual(pcb+0xf0,0x24));assert not scpsr&15
     sandbox_progress=None;frames=[];fp=sfp&umask;seen_fp=set()
     for _ in range(24):
      if not fp or fp&15 or fp in seen_fp:break
      seen_fp.add(fp)
      try:next_fp,lr=struct.unpack('<QQ',virtual(fp,16,root_pa))
      except (ValueError,AssertionError):break
      if target_process=='WindowServer' and slide is not None and (lr&umask)==0x184e6ce64+slide and sandbox_progress is None:
       try:
        import hashlib
        proof=json.loads((r/'SANDBOX-COMPILER-LAYOUT.json').read_text())
        for function in proof['functions'].values():
         assert hashlib.sha256(virtual(function['address']+slide,function['bytes'],root_pa)).hexdigest()==function['sha256'],'sandbox/compiler ABI signature mismatch'
        stream,scheme=struct.unpack('<QQ',virtual(fp-16,16,root_pa));stream&=umask;scheme&=umask
        assert stream>=0x1000 and not stream&7 and scheme>=0x1000 and not scheme&7
        assert int.from_bytes(virtual(scheme+0x2d8,8,root_pa),'little')&umask==stream,'saved FILE and active Scheme port disagree'
        meta=virtual(stream,0x98,root_pa);remaining=struct.unpack_from('<i',meta,8)[0];flags,fd=struct.unpack_from('<Hh',meta,0x10);offset=struct.unpack_from('<q',meta,0x90)[0]
        assert flags&4 and -1<=fd<4096 and -1<=remaining<=16*1024*1024
        known=bool(flags&0x1000);assert not known or 0<=offset<=1024**3
        profile_label=None;profile_bytes=None
        try:
         profile_root=Path('/Volumes/Dell27Recovery/System/Library/Sandbox/Profiles');public={}
         for pf in profile_root.glob('*.sb'):
          for label in [pf.name,pf.stem,'/System/Library/Sandbox/Profiles/'+pf.name]:public[label]=pf
         label_pointer=int.from_bytes(virtual(scheme+0x2f0,8,root_pa),'little')&umask
         label=virtual(label_pointer,min(256,0x4000-(label_pointer&0x3fff)),root_pa).split(b'\0',1)[0].decode('utf-8')
         if label in public:profile_label=public[label].name;profile_bytes=public[label].stat().st_size
        except (ValueError,AssertionError,UnicodeError):pass
        sandbox_progress={'scope':'Signature-verified compiler FILE metadata only; no file/policy contents or keys read. Offset requires SOFF, and one sample does not establish progress.','knownPublicProfile':profile_label,'sourceProfileBytes':profile_bytes,'streamIdentity':hex(stream),'descriptor':fd,'readBufferRemainingBytes':remaining,'fileOffsetValid':known,'fileReadOffsetBytes':offset if known else None}
       except (ValueError,AssertionError) as e:sandbox_progress={'incomplete':str(e),'scope':'No compiler progress inference'}
      frames.append(hex(lr&umask));next_fp&=umask
      if next_fp<=fp or next_fp-fp>0x40000:break
      fp=next_fp
     chain=None
     # Exact supplied libsandbox _find_slot_in_env traverses x9->next(+16)
     # until x0 (Scheme NIL). Read addresses/counts only, no symbol strings.
     resume_pc=(u64(env+320) if current==th and is_user else spc&umask)
     if slide is not None and 0x184e697e8+slide<=resume_pc<=0x184e69800+slide:
      def selected_reg(n):return u64(env+64+8*n) if current==th and is_user else v64(pcb+8+8*n)
      nil=selected_reg(0)&umask;cursor=selected_reg(9)&umask;visited=set();reason='bounded at256 entries'
      for _ in range(256):
       if cursor==nil:reason='terminated at actual Scheme NIL';break
       if cursor in visited:reason='cycle detected';break
       visited.add(cursor)
       try:cursor=int.from_bytes(virtual(cursor+16,8,root_pa),'little')&umask
       except (ValueError,AssertionError):reason='unmapped cursor; no inference';break
      chain={'scope':'Only addresses/counts of current exact-function environment chain, not policy contents','resumePC':hex(resume_pc),'nodes':len(visited),'result':reason}
     kernel_saved=None
     # Exact-build Switch_context saves x29/x30/SP at kstackptr+0x50/+0x58/+0x60;
     # thread.kstackptr is +0x148. Only inspect selected WAIT threads, never data.
     if int.from_bytes(virtual(th+0x1c8,4),'little')&1 and th!=current:
      try:
       ks=canonical(v64(th+0x148));kf,kl,ksp=struct.unpack('<QQQ',virtual(ks+0x50,24));kf=canonical(kf);ksp=canonical(ksp)
       assert ksp>=0xfffffe0000000000 and ksp<=kf<ksp+0x10000 and not kf&15
       kframes=[hex(canonical(kl))];kfseen=set();gate_wait=None
       for _ in range(20):
        if kf in kfseen or not ksp<=kf<ksp+0x10000:break
        kfseen.add(kf);nf,nl=struct.unpack('<QQ',virtual(kf,16));kframes.append(hex(canonical(nl)));nf=canonical(nf)
        if canonical(nl)==0xfffffe000a6adfe0:
         # Exact tail-call mutex prologue saves caller x20 (recursive lock)
         # at FP-16. IORecursiveLockLock stores owner at lock+0x18.
         lock=canonical(v64(kf-16));owner=canonical(v64(lock+0x18));orop=canonical(v64(owner+0x3f0));assert canonical(v64(orop))==owner
         onp=canonical(v64(owner+0x830));oname=virtual(onp,64).split(b'\0',1)[0].decode(errors='replace') if onp else ''
         ost=int.from_bytes(virtual(owner+0x1c8,4),'little');oks=canonical(v64(owner+0x148));of,ol,osp=struct.unpack('<QQQ',virtual(oks+0x50,24));of=canonical(of);osp=canonical(osp);ofr=[hex(canonical(ol))];oseen=set()
         for _ in range(20):
          if of in oseen or of&15 or not osp<=of<osp+0x10000:break
          oseen.add(of);onf,onl=struct.unpack('<QQ',virtual(of,16));ofr.append(hex(canonical(onl)));onf=canonical(onf)
          if onf<=of:break
          of=onf
         gate_wait={'lock':hex(lock),'ownerThread':hex(owner),'ownerName':oname,'ownerSchedulerState':hex(ost),'ownerSavedKernelReturnPCs':ofr,'scope':'Read-only exact recursive-lock metadata and owner saved return addresses; owner context may be stale if RUN'}
        if nf<=kf or nf&15:break
        kf=nf
       kernel_saved={'scope':'Selected noncurrent WAIT-thread saved kernel return addresses; exact-build Switch_context ABI; no stack payload exported','returnPCs':kframes,'recursiveGateWait':gate_wait}
      except (ValueError,AssertionError):pass
     scheduler_state=int.from_bytes(virtual(th+0x1c8,4),'little');wait_result=int.from_bytes(virtual(th+0xc8,4),'little',signed=True)
     ws_threads.append({'pid':wp['pid'],'thread':hex(th),'schedulerState':hex(scheduler_state),'schedulerFlags':[label for bit,label in [(1,'WAIT'),(2,'SUSP'),(4,'RUN'),(8,'UNINT'),(16,'TERMINATE'),(32,'TERMINATE2'),(64,'WAIT_REPORT'),(128,'IDLE'),(256,'WAKING')] if scheduler_state&bit],'waitResult':wait_result,'savedKernelReturnChain':kernel_saved,'name':name,'SandboxEnvironmentChain':chain,'SandboxCompilerInputMetadata':sandbox_progress,'savedPC':hex(spc&umask),'savedLR':hex(slr&umask),'savedReturnChain':frames,'scope':'Selected saved return addresses only, from verified selected-service-owned thread and page tables; may be stale for currently running EL0 thread'})
     th=canonical(v64(th+0x3e0))
   except (ValueError,AssertionError) as e:ws_threads.append({'pid':wp['pid'],'incomplete':str(e)})
  try:
   cv=virtual(0xfffffe00079b8f38,128)
   vb,vr,vw,vh,vd=struct.unpack_from('<QQQQQ',cv)
   result['kernelConsoleVideo']={'scope':'Read-only PE_current_console public geometry, exact26A428 address','baseAddress':hex(vb),'rowBytes':vr,'width':vw,'height':vh,'depth':vd,'rawLayoutValidated':False}
  except (ValueError,AssertionError):pass
  try:
   fb_arrays={}
   for label,ga in [('allOpened',0xfffffe000ad17d68),('started',0xfffffe000ad17d70)]:
    array=canonical(v64(ga));count=int.from_bytes(virtual(array+0x14,4),'little') if array else None
    if count is not None:assert count<1024
    fb_arrays[label]={'array':hex(array),'count':count}
    if count and label=='started':
     storage=canonical(v64(array+0x20));members=[]
     for i in range(min(count,16)):
      obj=canonical(v64(storage+8*i));st=int.from_bytes(virtual(obj+0x48,4),'little')
      members.append({'object':hex(obj),'IOServiceState':hex(st),'registered':bool(st&2),'inactive':bool(st&1),'userInvisible':bool(st&0x80000000)})
     fb_arrays[label]['publicServiceStates']=members
   result['nativeIOFramebufferRegistry']={'scope':'Read-only exact-build framebuffer globals, OSArray metadata and selected IOService state bits; no rendering buffers or process payloads read','arrays':fb_arrays,'consoleFramebuffer':hex(canonical(v64(0xfffffe000ad17d88))),'displayCount':int.from_bytes(virtual(0xfffffe000ad17d80,4),'little'),'serverInitialized':virtual(0xfffffe000ad17d9c,1)[0]}
  except (ValueError,AssertionError):pass
  result['selectedService']=target_process;result['WindowServerThreads' if target_process=='WindowServer' else 'ServiceThreads']=ws_threads;result['WindowServerState' if target_process=='WindowServer' else 'ServiceState']=ws_state
 finally:call('cont')
path=d/('LIVE-SANDBOX-'+target_process.upper()+'-THREADS-'+datetime.datetime.now(datetime.timezone.utc).strftime('%H%M%S')+'.json');path.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
