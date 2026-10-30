# SPDX-License-Identifier: MIT
"""Exact EFI validation with isolated per-run firmware disks and evidence."""
from pathlib import Path
import subprocess,time,bz2,shutil,socket,json,hashlib,re,sys,fcntl,datetime,os,plistlib
r=Path(__file__).resolve().parent;w=r.parent
cases=['vm-tampered'] if '--tampered-only' in sys.argv else ['valid'] if '--exfat' in sys.argv else ['valid','vm-tampered']
locks=[]
for name in cases:
 lock=(r/('.firmware-test-'+name+'.lock')).open('a')
 try:fcntl.flock(lock.fileno(),fcntl.LOCK_EX|fcntl.LOCK_NB)
 except BlockingIOError:raise SystemExit('Another firmware test already owns '+name+' outputs')
 locks.append(lock)
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for data in iter(lambda:f.read(1024*1024),b''):h.update(data)
 return h.hexdigest()
build=json.loads((r/'BUILD.json').read_text());assert build['EFI_SHA256']==sha(r/'UnicornTest.efi')
for name,h in build['sourceSHA256'].items():assert sha(r/name)==h,'rebuild required: '+name
profile=r/'BootContextExperiment' if '--boot-context' in sys.argv else r/'HFSExperiment' if '--hfs' in sys.argv else r/'StorageExperiment' if '--storage' in sys.argv else r/'FramebufferExperiment' if '--framebuffer' in sys.argv else r/'InstallerExperiment' if '--installer' in sys.argv else r
assert ('framebuffer' in build['mode'])==('--framebuffer' in sys.argv),'EFI/framebuffer mismatch'
assert (('installer' in build['mode']) or ('framebuffer' in build['mode']))==('--installer' in sys.argv),'EFI/profile mismatch'
assert ('storage' in build['mode'])==('--storage' in sys.argv),'EFI/storage mismatch'
assert ('hfs' in build['mode'])==('--hfs' in sys.argv),'EFI/HFS mismatch'
assert ('boot-context' in build['mode'])==('--boot-context' in sys.argv),'EFI/boot-context mismatch'
meta=json.loads((profile/'VMAPPLE-REFERENCE-BOOT.json').read_text());input_relative=Path(meta['inputRelativePath'])
assert not input_relative.is_absolute() and '..' not in input_relative.parts
assert sha(profile/'vmapple-reference-boot-state.bin')==meta['sha256']
runid=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')+'-'+str(os.getpid())
run=r/'FirmwareRuns'/runid;run.mkdir(parents=True)
(run/'BUILD.json').write_text(json.dumps(build,indent=2)+'\n');(run/'INPUT.json').write_text(json.dumps(meta,indent=2)+'\n')
private_efi=run/'executed.efi';shutil.copy2(r/'UnicornTest.efi',private_efi);assert sha(private_efi)==build['EFI_SHA256']
results={};kernel={}
for case in cases:
 root=run/case;v=root/'VirtualUSB';(v/'EFI/BOOT').mkdir(parents=True);(v/'EngineProbe').mkdir();(v/'EFI/Drivers').mkdir(parents=True)
 shutil.copy2(private_efi,v/'EFI/BOOT/BOOTX64.EFI');shutil.copy2(r/'reference-boot-state.bin',v/'EngineProbe/reference-boot-state.bin');shutil.copy2(w/'InstallerInputs/exfat_x64.efi',v/'EFI/Drivers/exfat_x64.efi')
 use_exfat=('--exfat' in sys.argv or case=='vm-tampered' or meta['guestBytes']>500*1024**2)
 if not use_exfat:
  dst=v/input_relative;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(profile/'vmapple-reference-boot-state.bin',dst)
 if '--formatted-aux' in sys.argv:
  import runpy
  formatter=runpy.run_path(str(r/'prepare-empty-aux.py'))
  aux_report=formatter['create']((v/'EngineProbe/UtilityStorage-v1.bin').resolve())
  aux_report['formatterSHA256']=sha(r/'prepare-empty-aux.py')
  (root/'AUX-INITIALIZATION.json').write_text(json.dumps(aux_report,indent=2)+'\n')
 else:
  with (v/'EngineProbe/UtilityStorage-v1.bin').open('wb') as f:f.truncate(32*1024*1024)
 log=root/'serial.log';err=root/'qemu.log';monitor=root/'monitor.sock';variables=root/'vars.fd'
 variables.write_bytes(bz2.decompress((w/'darwin-vm/qemu-sptm/pc-bios/edk2-i386-vars.fd.bz2').read_bytes()))
 # macOS AF_UNIX path limit: keep monitor sockets short while all disks and
 # serial outputs remain in this run's private directory.
 monitor=Path('/tmp/d27-'+str(os.getpid())+'-'+case+'.sock');assert not monitor.exists()
 args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'),'-M','q35','-cpu','max','-accel','tcg','-m','12288M' if '--boot-context' in sys.argv else '8192M' if '--installer' in sys.argv else '2048M','-nodefaults','-device','virtio-rng-pci','-device','VGA','-display','none','-serial','file:'+str(log),'-monitor','unix:'+str(monitor)+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(w/'Standalone/OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(variables),'-drive','if=none,id=esp,format=raw,file=fat:rw:'+str(v),'-device','virtio-blk-pci,drive=esp']
 if use_exfat:
  private_input=(profile/'FirmwareInputs'/runid/case/'inputs.raw') if '--hfs' in sys.argv else root/'inputs.raw'
  private_input.parent.mkdir(parents=True,exist_ok=True)
  # APFS clones keep isolated writable negative fixtures without allocating
  # a second multi-gigabyte extent. Fall back to a full copy on other filesystems.
  copied=subprocess.run(['cp','-c',str(profile/'test-inputs.raw'),str(private_input)],capture_output=True)
  if copied.returncode:
   private_input.unlink(missing_ok=True);shutil.copy2(profile/'test-inputs.raw',private_input)
  if case=='vm-tampered':
   attached=plistlib.loads(subprocess.check_output(['hdiutil','attach','-plist','-nobrowse','-imagekey','diskimage-class=CRawDiskImage',str(private_input)]))
   point=Path(next(e['mount-point'] for e in attached['system-entities'] if 'mount-point' in e))
   try:
    target=point/input_relative;assert sha(target)==meta['sha256'], 'negative test requires the exact valid fixture'
    with target.open('r+b') as f:f.seek(-1,2);last=f.read(1);f.seek(-1,2);f.write(bytes([last[0]^1]))
   finally:subprocess.run(['hdiutil','detach',str(point)],check=True)

  args+=['-drive','if=none,id=inputdisk,format=raw,readonly=on,file='+str(private_input),'-device','virtio-blk-pci,drive=inputdisk']
 if '--outer-icount' in sys.argv:
  args+=['-icount','shift=0,align=off,sleep=off','-rtc','clock=vm']
 (root/'COMMAND.json').write_text(json.dumps(args,indent=2)+'\n')
 (root/'OUTER-CLOCK.json').write_text(json.dumps({'policy':'instruction-paced outer x86 clock' if '--outer-icount' in sys.argv else 'original outer wall clock','EFI_SHA256':build['EFI_SHA256'],'guestCounterFrequencyUnchanged':True,'DellPerformanceMeasurement':False,'instructionCountingIsCycleAccurate':False},indent=2)+'\n')
 (r/('ACTIVE-'+case+'.json')).write_text(json.dumps({'run':str(run),'log':str(log),'EFI_SHA256':build['EFI_SHA256']},indent=2)+'\n')
 with err.open('w') as stderr:
  p=subprocess.Popen(args,stdout=stderr,stderr=stderr,pass_fds=tuple(x.fileno() for x in locks))
  (root/'HOST-PROCESS.json').write_text(json.dumps({'PID':p.pid,'runnerPID':os.getpid(),'processGroup':os.getpgid(p.pid),'run':str(run),'case':case,'EFI_SHA256':build['EFI_SHA256']},indent=2)+'\n')
  awake=subprocess.Popen(['/usr/bin/caffeinate','-i','-w',str(p.pid)],stdin=subprocess.DEVNULL,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
  try:
   deadline=time.monotonic()+(86400 if '--boot-context' in sys.argv else 1800)
   while time.monotonic()<deadline:
    text=log.read_text(errors='replace') if log.exists() else ''
    if text.count('Dell27 Unicorn firmware engine integration test')>1:raise RuntimeError('Unexpected firmware reset: '+str(log))
    if case=='valid' and 'DONE engine integration tests' in text:break
    if case!='valid' and 'FAIL reference capture admission' in text:break
    if 'FATAL firmware bridge:' in text or '\nFAIL ' in text or '!!!! X64 Exception' in text:raise RuntimeError(text[-6000:])
    if p.poll() is not None:
     exit_report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'QEMUReturnCode':p.returncode,'case':case,'run':str(run),'EFI_SHA256':build['EFI_SHA256'],'QEMUErrorText':err.read_text(),'lastSerialTail':text[-3000:],'graphicalInstallerVerified':False}
     (root/'HOST-EXIT.json').write_text(json.dumps(exit_report,indent=2)+'\n')
     raise RuntimeError('QEMU exited with code '+str(p.returncode)+': '+err.read_text())
    time.sleep(.2)
   else:raise RuntimeError('Firmware test wall-time budget reached: '+str(log))
   for marker in ['PASS compositor compatibility guard:','Firmware watchdog disabled','PASS firmware services self-test','PASS full Unicorn scalar JIT execution','PASS full Unicorn selected EL2 JIT execution','PASS DIT:','PASS GICv3:','PASS guest timer MSR:','PASS future timer preemption:','PASS MSI:','PASS SGI:','PASS PCI:','PASS VirtIO empty kicks:','PASS VirtIO AUX:', 'PASS VirtIO root:','PASS GPIO:','PASS IOSFC:','PASS BDIF:','PASS AES:','PASS RTC:','PASS PL031:','PASS VMApple config:','PASS PSCI:','PASS QARMA:','PASS guest exceptions:','PASS sparse physical RAM: 16 GiB','PASS VMAPPLE PAC: 72 ','PASS authenticated loads:','PASS TLBI range:','PASS SPRR: 768 ','PASS system MMU:','PASS 16K COW:','PASS partial code hooks:']:
    assert marker in text,marker
   assert 'visited=25871; pc=0x8070af77c;' in text
   if case=='valid':
    if use_exfat:assert 'Verified EfiFs 1.13 exFAT driver started' in text
    assert 'Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay' in text
    count=int(re.search(r'VMAPPLE kernel replay stopped:.*?(?:visited|scheduled_slots)=(\d+);',text)[1])
    kernel={('scheduledInstructionSlotsUpperBound' if build['mode'].endswith('block-trace') else 'instructionsVisited'):count,'committedPhysicalBytes':int(re.search(r'VMAPPLE physical RAM committed=(\d+)',text)[1]),'initializedCaptureBytes':meta['guestBytes'],'logicalRAMBytes':meta['RAMBytes'],'fullKernelBoot':False,'darwinBannerObserved':'Darwin Kernel Version 27.0.0' in text,'pmapStartupCompleted':'pmap_startup() init/release time:' in text,'stop':re.search(r'VMAPPLE kernel replay stopped: (.*)',text)[1].strip(),'lastPhysicalFault':re.search(r'VMAPPLE RAM fault detail: (.*)',text)[1].strip(),'kernelPanicObserved':'VMAPPLE kernel panic format:' in text,'panicFormat':(re.search(r'VMAPPLE kernel panic format: (.*)',text)[1].strip() if 'VMAPPLE kernel panic format:' in text else None),'guestProcessFatalExceptions':re.findall(r'ERROR: \[[^\n]*FATAL exception:[^\n]*',text),'launchdObserved':'Darwin Bootstrapper Version' in text,'shellPromptObserved':'bash-3.2#' in text,'GIC':re.search(r'VMAPPLE GIC accesses=(.*)',text)[1].strip(),'PVCTRRProtection':False}
   else:assert 'beginning sparse-RAM VMAPPLE kernel replay' not in text and 'Darwin Kernel Version' not in text
   assert sha(v/'EFI/BOOT/BOOTX64.EFI')==build['EFI_SHA256']
   results[case]='PASS';print('PASS isolated EFI',case,runid,flush=True)
  finally:
   if p.poll() is None:
    try:
     with socket.socket(socket.AF_UNIX) as s:
      s.connect(str(monitor));s.recv(4096)
      if case=='valid' and '--framebuffer' in sys.argv:
       s.sendall(('screendump '+str(root/'display.ppm')+'\n').encode());s.settimeout(5)
       try:s.recv(8192)
       except socket.timeout:pass
      s.sendall(b'quit\n')
     p.wait(timeout=5)
    except (OSError,subprocess.TimeoutExpired):p.terminate();p.wait(timeout=5)
   monitor.unlink(missing_ok=True)
assert not subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--undefined-only',str(r/'UnicornTest.elf')],text=True).strip()
report={'EFI_SHA256':build['EFI_SHA256'],'OVMFCases':results,'unresolvedSymbols':[],'VMAPPLEReplay':kernel,'inputSHA256':meta['sha256'],'runDirectory':str(run),'privateExecutable':str(private_efi),'sourceSHA256':build['sourceSHA256'],'DellHardware':False,'kernelBoot':False,'firmwareWatchdogDisabled':True,'isolatedInputsAndOutputs':True}
name='ADMISSION-VALIDATION.json' if '--tampered-only' in sys.argv else 'EXFAT-FIRMWARE-VALIDATION.json' if '--exfat' in sys.argv else 'SPARSE-RAM-EXPLORATORY.json'
(run/'VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n');(r/name).write_text(json.dumps(report,indent=2)+'\n')
# Convenience logs are published only after completion. Canonical evidence and
# the executed EFI remain in the unique run directory.
for case in cases:shutil.copy2(run/case/'serial.log',r/('ovmf-sparse-'+case+'.log'))
print('PASS firmware checks; full macOS boot is not established',flush=True)
