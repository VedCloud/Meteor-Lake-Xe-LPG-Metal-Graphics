# SPDX-License-Identifier: MIT
"""Deliberate fatal assertion must still halt inside actual x86 UEFI."""
from pathlib import Path
import subprocess,bz2,shutil,hashlib,json,os,time,socket,datetime,re
r=Path(__file__).resolve().parent;w=r.parent;build=json.loads((r/'BUILD.json').read_text());assert build['mode']=='assert-failure-probe'
digest=hashlib.sha256((r/'UnicornTest.efi').read_bytes()).hexdigest();assert digest==build['EFI_SHA256']
run=r/'FirmwareRuns'/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')+'-'+str(os.getpid())+'-assert');v=run/'VirtualUSB';(v/'EFI/BOOT').mkdir(parents=True)
shutil.copy2(r/'UnicornTest.efi',v/'EFI/BOOT/BOOTX64.EFI');(run/'vars.fd').write_bytes(bz2.decompress((w/'darwin-vm/qemu-sptm/pc-bios/edk2-i386-vars.fd.bz2').read_bytes()))
log=run/'serial.log';monitor=Path('/tmp/d27-assert-'+str(os.getpid())+'.sock')
args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'),'-M','q35','-cpu','max','-accel','tcg','-m','512M','-nodefaults','-device','VGA','-display','none','-serial','file:'+str(log),'-monitor','unix:'+str(monitor)+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(w/'Standalone/OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(run/'vars.fd'),'-drive','if=none,id=esp,format=raw,readonly=on,file=fat:ro:'+str(v),'-device','virtio-blk-pci,drive=esp']
with (run/'qemu.log').open('w') as out:
 p=subprocess.Popen(args,stdout=out,stderr=out)
 try:
  end=time.monotonic()+45
  while time.monotonic()<end:
   text=log.read_text(errors='replace') if log.exists() else ''
   if 'FATAL firmware bridge:' in text:break
   assert p.poll() is None,'QEMU exited';time.sleep(.2)
  assert re.search(r'FATAL firmware bridge: .*harness.c:\d+ guard',text),text[-1500:]
  assert 'PASS firmware services self-test' in text and 'FAIL assertion did not halt' not in text
  result={'result':'PASS: deliberate assertion remains fatal','EFI_SHA256':digest,'runDirectory':str(run),'alwaysOnAssertions':True,'assertionsDisabled':False,'kernelBoot':False}
  (r/'ASSERT-GUARD-VALIDATION.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
 finally:
  if p.poll() is None:
   with socket.socket(socket.AF_UNIX) as s:s.connect(str(monitor));s.sendall(b'quit\n')
   p.wait(timeout=5)
  monitor.unlink(missing_ok=True)
