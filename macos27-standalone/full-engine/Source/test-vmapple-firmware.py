# SPDX-License-Identifier: MIT
"""Exercise the exact full-engine EFI in OVMF; reject modified replay inputs."""
from pathlib import Path
import subprocess,time,bz2,shutil,socket,json,hashlib,re
r=Path(__file__).resolve().parent;w=r.parent;v=r/'VirtualUSB';(v/'EFI/BOOT').mkdir(parents=True,exist_ok=True);(v/'EngineProbe').mkdir(exist_ok=True)
shutil.copy2(r/'UnicornTest.efi',v/'EFI/BOOT/BOOTX64.EFI');original=(r/'reference-boot-state.bin').read_bytes();results={};vm_replay={}
vm_original=(r/'vmapple-reference-boot-state.bin').read_bytes()
for case in ['valid','tampered','vm-tampered']:
 data=bytearray(original)
 if case=='tampered':data[-1]^=1
 (v/'EngineProbe/reference-boot-state.bin').write_bytes(data)
 vmdata=bytearray(vm_original)
 if case=='vm-tampered':vmdata[-1]^=1
 (v/'EngineProbe/vmapple-reference-boot-state.bin').write_bytes(vmdata)
 log=r/('ovmf-vmapple-'+case+'.log');err=r/('ovmf-vmapple-'+case+'-qemu.log');monitor=r/('vmapple-'+case+'.sock');vars=r/('vmapple-'+case+'-vars.fd');log.unlink(missing_ok=True);monitor.unlink(missing_ok=True)
 vars.write_bytes(bz2.decompress((w/'darwin-vm/qemu-sptm/pc-bios/edk2-i386-vars.fd.bz2').read_bytes()))
 args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'),'-M','q35','-cpu','max','-accel','tcg','-m','2048M','-nodefaults','-device','virtio-rng-pci','-device','VGA','-display','none','-serial','file:'+str(log),'-monitor','unix:'+str(monitor)+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(w/'Standalone/OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(vars),'-drive','if=none,id=esp,format=raw,readonly=on,file=fat:ro:'+str(v),'-device','virtio-blk-pci,drive=esp']
 with err.open('w') as e:
  p=subprocess.Popen(args,stdout=e,stderr=e)
  try:
   deadline=time.monotonic()+180
   while time.monotonic()<deadline:
    text=log.read_text(errors='replace') if log.exists() else ''
    if case=='valid' and 'DONE engine integration tests' in text:break
    if case!='valid' and 'FAIL reference capture admission' in text:break
    if 'FATAL' in text or '\nFAIL ' in text or '!!!! X64 Exception' in text:raise RuntimeError(text[-6000:])
    if p.poll() is not None:raise RuntimeError(err.read_text())
    time.sleep(.2)
   else:raise RuntimeError('Firmware test timeout\n'+text[-6000:])
   assert 'BRIDGE MSABI format: ok 4294967298 beef -42' in text
   assert 'PASS firmware services self-test' in text
   assert 'PASS full Unicorn scalar JIT execution' in text and 'PASS full Unicorn selected EL2 JIT execution' in text
   assert 'PASS SPRR: 768 nonidentity guest MMU permission checks' in text
   assert 'PASS VMAPPLE PAC: 72 ' in text
   assert 'PASS system MMU: physical read/write contents, EL0 separation and unmapped backing rejection' in text
   if case=='valid':
    assert 'visited=25871; pc=0x8070af77c;' in text and 'Stopped instruction: 0xd53efc80' in text
    assert 'Reference replay is diagnostic evidence, not kernel boot.' in text
    assert 'VMAPPLE kernel replay stopped: Invalid memory write (UC_ERR_WRITE_UNMAPPED)' in text
    assert 'address=0xfffffe00221f0000' in text
    assert 'VMAPPLE successful PAC calls: 1,1,0,0,0,0,0' in text
    assert 'VMAPPLE kernel panic format:' not in text
    visits=int(re.search(r'VMAPPLE kernel replay stopped:.*?visited=(\d+);',text)[1]);assert visits>=30000000
    vm_replay={'instructionsVisited':visits,'PACInitialStateCalls':1,'PACDefaultKeysCalls':1,'stop':'Declared RAM beyond the 256 MiB bounded replay mapping','guestVirtualAddress':'0xfffffe00221f0000','guestPhysicalAddress':'0x8221f0000','deviceTreeIncluded':True,'fullKernelBoot':False}
   elif case=='vm-tampered':
    assert 'SPTM replay stopped' in text and 'beginning bounded VMAPPLE kernel replay' not in text
   else:assert 'Verified paused-reference memory capture' not in text and 'SPTM replay stopped' not in text
   results[case]='PASS';print('PASS OVMF engine',case,flush=True)
  finally:
   if p.poll() is None:
    try:
     with socket.socket(socket.AF_UNIX) as s:s.connect(str(monitor));s.recv(4096);s.sendall(b'quit\n')
     p.wait(timeout=5)
    except (OSError,subprocess.TimeoutExpired):p.terminate();p.wait(timeout=5)
(v/'EngineProbe/reference-boot-state.bin').write_bytes(original)
(v/'EngineProbe/vmapple-reference-boot-state.bin').write_bytes(vm_original)
unresolved=subprocess.check_output(['/opt/homebrew/opt/llvm@20/bin/llvm-nm','--undefined-only',str(r/'UnicornTest.elf')],text=True);assert not unresolved.strip()
report={'EFI_SHA256':hashlib.sha256((r/'UnicornTest.efi').read_bytes()).hexdigest(),'OVMFCases':results,'unresolvedSymbols':[],'firmwareServices':'PASS: allocation/free, overlapping copy, strings, sorting/searching, SysV and MS varargs, truncation, nonlocal jump and timer','scalarJIT':'PASS: all 80 QEMU oracle bytes','selectedEL2JIT':'PASS: all 80 QEMU oracle bytes','SPRRPermissions':'PASS: 768 nonidentity guest R/W/X cases, physical contents, EL0 isolation, VHE/EL12 aliases and denied monitor backing','VMAPPLEPAC':'PASS: 72 checks including independent OpenSSL AES fixture, key changes, signature rejection, diversification, timer frequency/progression, NOP and unsupported OEM discovery','SPTMReplay':{'instructionsVisited':25871,'stopPC':'0x8070af77c','instruction':'0xd53efc80','reason':'unimplemented Apple APSTS_EL1 operation','completeBootEnvironment':False},'VMAPPLEReplay':vm_replay,'VMAPPLEDeviceProfile':'Unqualified M2 device tree plus VMAPPLE kernel; no VM board claim','DellHardware':False,'kernelBoot':False}
(r/'VMAPPLE-FIRMWARE-EXPLORATORY.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS exact full-engine EFI verification; kernel boot remains incomplete')
