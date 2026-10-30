# SPDX-License-Identifier: MIT
"""Exercise the exact full-engine EFI in OVMF; reject modified replay inputs."""
from pathlib import Path
import subprocess,time,bz2,shutil,socket,json,hashlib,re
r=Path(__file__).resolve().parent
# Separate runners must never share serial logs, vars images or monitor paths.
import fcntl,sys
cases=(['vm-tampered'] if '--tampered-only' in sys.argv else ['valid'] if '--exfat' in sys.argv else ['valid','vm-tampered'])
test_locks=[]
for name in cases:
 lock=(r/('.firmware-test-'+name+'.lock')).open('a')
 try:fcntl.flock(lock.fileno(),fcntl.LOCK_EX|fcntl.LOCK_NB)
 except BlockingIOError:raise SystemExit('Another firmware test already owns '+name+' outputs')
 test_locks.append(lock)
w=r.parent;v=r/('VirtualUSBTampered' if '--tampered-only' in __import__('sys').argv else 'VirtualUSB');(v/'EFI/BOOT').mkdir(parents=True,exist_ok=True);(v/'EngineProbe').mkdir(exist_ok=True)
build=json.loads((r/'BUILD.json').read_text());assert build['EFI_SHA256']==hashlib.sha256((r/'UnicornTest.efi').read_bytes()).hexdigest()
for name,digest in build['sourceSHA256'].items():assert hashlib.sha256((r/name).read_bytes()).hexdigest()==digest, 'rebuild required: '+name
shutil.copy2(r/'UnicornTest.efi',v/'EFI/BOOT/BOOTX64.EFI');original=(r/'reference-boot-state.bin').read_bytes();results={};vm_replay={}
vm_original=(r/'vmapple-reference-boot-state.bin').read_bytes()
input_relative=Path(json.loads((r/'VMAPPLE-REFERENCE-BOOT.json').read_text()).get('inputRelativePath','EngineProbe/vmapple-reference-boot-state.bin'))
(v/input_relative).parent.mkdir(parents=True,exist_ok=True)
for case in cases:
 data=bytearray(original)
 if case=='tampered':data[-1]^=1
 (v/'EngineProbe/reference-boot-state.bin').write_bytes(data)
 vmdata=bytearray(vm_original)
 if case=='vm-tampered':vmdata[-1]^=1
 if '--exfat' in __import__('sys').argv:(v/input_relative).unlink(missing_ok=True)
 else:(v/input_relative).write_bytes(vmdata)
 log=r/('ovmf-sparse-'+case+'.log');err=r/('ovmf-sparse-'+case+'-qemu.log');monitor=r/('sparse-'+case+'.sock');vars=r/('sparse-'+case+'-vars.fd');log.unlink(missing_ok=True);monitor.unlink(missing_ok=True)
 vars.write_bytes(bz2.decompress((w/'darwin-vm/qemu-sptm/pc-bios/edk2-i386-vars.fd.bz2').read_bytes()))
 args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'),'-M','q35','-cpu','max','-accel','tcg','-m','2048M','-nodefaults','-device','virtio-rng-pci','-device','VGA','-display','none','-serial','file:'+str(log),'-monitor','unix:'+str(monitor)+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(w/'Standalone/OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(vars),'-drive','if=none,id=esp,format=raw,readonly=on,file=fat:ro:'+str(v),'-device','virtio-blk-pci,drive=esp']
 if '--exfat' in __import__('sys').argv:args+=['-drive','if=none,id=inputdisk,format=raw,readonly=on,file='+str(r/'test-inputs.raw'),'-device','virtio-blk-pci,drive=inputdisk']
 with err.open('w') as e:
  p=subprocess.Popen(args,stdout=e,stderr=e,pass_fds=tuple(lock.fileno() for lock in test_locks))
  try:
   deadline=time.monotonic()+1000
   while time.monotonic()<deadline:
    text=log.read_text(errors='replace') if log.exists() else ''
    if text.count('Dell27 Unicorn firmware engine integration test')>1:raise RuntimeError('Unexpected firmware reset; see '+str(log))
    if case=='valid' and 'DONE engine integration tests' in text:break
    if case!='valid' and 'FAIL reference capture admission' in text:break
    if 'FATAL' in text or '\nFAIL ' in text or '!!!! X64 Exception' in text:raise RuntimeError(text[-6000:])
    if p.poll() is not None:raise RuntimeError(err.read_text())
    time.sleep(.2)
   else:raise RuntimeError('Firmware test timeout\n'+text[-6000:])
   if '--exfat' in __import__('sys').argv:assert 'Verified EfiFs 1.13 exFAT driver started' in text
   assert 'Firmware watchdog disabled for long-running ARM64 runtime' in text
   assert 'BRIDGE MSABI format: ok 4294967298 beef -42' in text
   assert 'PASS firmware services self-test' in text
   assert 'PASS full Unicorn scalar JIT execution' in text and 'PASS full Unicorn selected EL2 JIT execution' in text
   assert 'PASS SPRR: 768 nonidentity guest MMU permission checks' in text
   assert 'PASS VMAPPLE PAC: 72 ' in text
   assert 'PASS system MMU: physical read/write contents, EL0 separation and unmapped backing rejection' in text
   assert 'PASS DIT:' in text and 'PASS GICv3:' in text and 'PASS guest exceptions:' in text and 'PASS sparse physical RAM: 16 GiB' in text and 'PASS TLBI range:' in text
   if case=='valid':
    assert 'visited=25871; pc=0x8070af77c;' in text and 'Stopped instruction: 0xd53efc80' in text
    assert 'Reference replay is diagnostic evidence, not kernel boot.' in text
    assert 'Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay' in text
    visits=int(re.search(r'VMAPPLE kernel replay stopped:.*?(?:visited|scheduled_slots)=(\d+);',text)[1])
    committed=int(re.search(r'VMAPPLE physical RAM committed=(\d+)',text)[1])
    vm_replay={('scheduledInstructionSlotsUpperBound' if build['mode']=='vmapple-block-trace' else 'instructionsVisited'):visits,'committedPhysicalBytes':committed,'initializedCaptureBytes':len(vm_original),'logicalRAMBytes':17179869184,'fullKernelBoot':False,
     'darwinBannerObserved':'Darwin Kernel Version 27.0.0' in text,
     'pmapStartupCompleted':'pmap_startup() init/release time:' in text,
     'stop':re.search(r'VMAPPLE kernel replay stopped: (.*)',text)[1].strip(),
     'lastPhysicalFault':re.search(r'VMAPPLE RAM fault detail: (.*)',text)[1].strip(),
     'kernelPanicObserved':'VMAPPLE kernel panic format:' in text,
     'panicFormat':(re.search(r'VMAPPLE kernel panic format: (.*)',text)[1].strip() if 'VMAPPLE kernel panic format:' in text else None),
     'launchdObserved':'Darwin Bootstrapper Version' in text,
     'shellPromptObserved':'bash-3.2#' in text,
     'GIC':re.search(r'VMAPPLE GIC accesses=(.*)',text)[1].strip(),
     'PVCTRRProtection':False}
   elif case=='vm-tampered':
    assert 'SPTM replay stopped' in text and 'beginning sparse-RAM VMAPPLE kernel replay' not in text and 'Darwin Kernel Version' not in text
   else:assert 'Verified paused-reference memory capture' not in text and 'SPTM replay stopped' not in text
   results[case]='PASS';print('PASS OVMF engine',case,flush=True)
  finally:
   if p.poll() is None:
    try:
     with socket.socket(socket.AF_UNIX) as s:s.connect(str(monitor));s.recv(4096);s.sendall(b'quit\n')
     p.wait(timeout=5)
    except (OSError,subprocess.TimeoutExpired):p.terminate();p.wait(timeout=5)
(v/'EngineProbe/reference-boot-state.bin').write_bytes(original)
(v/input_relative).write_bytes(vm_original)
unresolved=subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--undefined-only',str(r/'UnicornTest.elf')],text=True);assert not unresolved.strip()
report={'EFI_SHA256':hashlib.sha256((r/'UnicornTest.efi').read_bytes()).hexdigest(),'OVMFCases':results,'unresolvedSymbols':[],'firmwareServices':'PASS: allocation/free, overlapping copy, strings, sorting/searching, SysV and MS varargs, truncation, nonlocal jump and timer','scalarJIT':'PASS: all 80 QEMU oracle bytes','selectedEL2JIT':'PASS: all 80 QEMU oracle bytes','SPRRPermissions':'PASS: 768 nonidentity guest R/W/X cases, physical contents, EL0 isolation, VHE/EL12 aliases and denied monitor backing','VMAPPLEPAC':'PASS: 72 checks including independent OpenSSL AES fixture, key changes, signature rejection, diversification, timer frequency/progression, NOP and unsupported OEM discovery','SPTMReplay':{'instructionsVisited':25871,'stopPC':'0x8070af77c','instruction':'0xd53efc80','reason':'unimplemented Apple APSTS_EL1 operation','completeBootEnvironment':False},'VMAPPLEReplay':vm_replay,'VMAPPLEDeviceProfile':'Supplied vma2 template with captured initialized images, one vCPU, GICv3, PL011 and 16 GiB logical RAM','DellHardware':False,'kernelBoot':False}
(r/('ADMISSION-VALIDATION.json' if '--tampered-only' in __import__('sys').argv else 'EXFAT-FIRMWARE-VALIDATION.json' if '--exfat' in __import__('sys').argv else 'SPARSE-RAM-EXPLORATORY.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS exact full-engine EFI verification; kernel boot remains incomplete')
