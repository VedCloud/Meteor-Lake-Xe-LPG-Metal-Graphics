# SPDX-License-Identifier: MIT
"""Run the exact packaged EFI in virtual firmware, including corrupted input refusal."""
from pathlib import Path
import subprocess,time,socket,bz2,shutil,json,hashlib
r=Path(__file__).resolve().parent;root=r.parent;q=root/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'
bios=root/'darwin-vm/qemu-sptm/pc-bios';virtual=r/'VirtualUSB'
shutil.copy2(r/'Dell27Preflight.efi',virtual/'EFI/BOOT/BOOTX64.EFI')
for name in ['bootkc','dtree','sptm','txm']:shutil.copy2(root/'darwin-vm/firmware'/name,virtual/'macOS27'/name)
results={}
for case in ['valid','tampered']:
 console=r/('ovmf-'+case+'.log');stderr=r/('ovmf-'+case+'-qemu.log');monitor=r/('ovmf-'+case+'.sock');vars=r/('ovmf-'+case+'-vars.fd')
 vars.write_bytes(bz2.decompress((bios/'edk2-i386-vars.fd.bz2').read_bytes()))
 original=(virtual/'macOS27/dtree').read_bytes()
 if case=='tampered':
  data=bytearray(original);data[-1]^=1;(virtual/'macOS27/dtree').write_bytes(data)
 args=[str(q),'-M','q35','-accel','tcg','-m','512M','-nodefaults','-device','VGA','-display','none','-serial','file:'+str(console),'-monitor','unix:'+str(monitor)+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(r/'OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(vars),'-drive','if=none,id=esp,format=raw,readonly=on,file=fat:ro:'+str(virtual),'-device','virtio-blk-pci,drive=esp']
 try:
  with stderr.open('w') as err:
   p=subprocess.Popen(args,stdout=err,stderr=err)
   try:
    deadline=time.monotonic()+40
    while time.monotonic()<deadline:
     text=console.read_text(errors='replace') if console.exists() else ''
     wanted='Press a key to return to firmware.' if case=='valid' else 'Rejected boot inputs. No guest was executed.'
     if wanted in text:break
     if p.poll() is not None:raise RuntimeError(stderr.read_text())
     time.sleep(.2)
    else:raise RuntimeError('UEFI test timeout: '+case)
    if case=='valid':assert text.count('SHA-256 verified')==4 and 'UNIMPLEMENTED' in text and 'PASS: standalone firmware' in text
    else:assert 'dtree ... FAIL (status 26)' in text and 'PASS: standalone firmware' not in text
    results[case]='PASS'
   finally:
    if p.poll() is None:
     try:
      with socket.socket(socket.AF_UNIX) as s:s.connect(str(monitor));s.recv(4096);s.sendall(b'quit\n')
      p.wait(timeout=5)
     except (OSError,subprocess.TimeoutExpired):p.terminate();p.wait(timeout=5)
 finally:(virtual/'macOS27/dtree').write_bytes(original)
 print('PASS OVMF',case,flush=True)
report=json.loads((r/'VALIDATION.json').read_text());report['exactEFISHA256']=hashlib.sha256((r/'Dell27Preflight.efi').read_bytes()).hexdigest();report['OVMFCases']=results;(r/'VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n')
