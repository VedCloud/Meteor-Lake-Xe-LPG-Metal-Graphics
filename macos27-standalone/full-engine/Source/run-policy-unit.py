from pathlib import Path
import subprocess,time,socket,json,datetime,hashlib,bz2,shutil,os
r=Path(__file__).resolve().parent;w=r.parent;run=r/'UnitRuns'/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')+'-'+str(os.getpid()));v=run/'VirtualUSB';(v/'EFI/BOOT').mkdir(parents=True);shutil.copy2(r/'UnicornTest.efi',v/'EFI/BOOT/BOOTX64.EFI');shutil.copy2(r/'BUILD.json',run/'BUILD.json')
vars=run/'vars.fd';vars.write_bytes(bz2.decompress((w/'darwin-vm/qemu-sptm/pc-bios/edk2-i386-vars.fd.bz2').read_bytes()));log=run/'serial.log';mon='/tmp/d27-policy-'+str(os.getpid())+'.sock'
args=[str(w/'darwin-vm/qemu-sptm/build-dell-host/qemu-system-x86_64'),'-M','q35','-cpu','max','-accel','tcg','-m','2048M','-nodefaults','-device','VGA','-display','none','-serial','file:'+str(log),'-monitor','unix:'+mon+',server=on,wait=off','-drive','if=pflash,format=raw,readonly=on,file='+str(w/'Standalone/OVMF_CODE.fd'),'-drive','if=pflash,format=raw,file='+str(vars),'-drive','if=none,id=esp,format=raw,file=fat:rw:'+str(v),'-device','virtio-blk-pci,drive=esp'];(run/'COMMAND.json').write_text(json.dumps(args,indent=2)+'\n')
with (run/'qemu.log').open('w') as err:
 p=subprocess.Popen(args,stdout=err,stderr=err)
 try:
  deadline=time.monotonic()+60
  while time.monotonic()<deadline:
   text=log.read_text(errors='replace') if log.exists() else ''
   if '\nFAIL ' in text or '!!!! X64 Exception' in text:raise RuntimeError(text[-3500:])
   if 'PASS service observer fixture:' in text:break
   if p.poll() is not None:raise RuntimeError('QEMU exit '+str(p.returncode))
   time.sleep(.2)
  else:raise RuntimeError('unit timeout '+text[-1500:])
  report={'result':'PASS','run':str(run),'EFI_SHA256':hashlib.sha256((v/'EFI/BOOT/BOOTX64.EFI').read_bytes()).hexdigest(),'scope':'61 translated read-only service observer cases plus earlier compositor guard; no macOS or renderer executed','graphicalInstaller':False};(run/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
 finally:
  if p.poll() is None:
   try:
    with socket.socket(socket.AF_UNIX) as s:s.connect(mon);s.sendall(b'quit\n')
   except OSError:pass
   p.wait(timeout=10)
