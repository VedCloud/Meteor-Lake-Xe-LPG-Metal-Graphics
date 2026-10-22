from pathlib import Path
import tempfile,shutil,subprocess
r=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');script=usb/'iGPU-Port/restore-touchpad.sh';old=usb/'iGPU-Port/backups/backend4g-touchpad-before-20261020/EFI/OC'
with tempfile.TemporaryDirectory() as d:
 for mode in ['success','changed-config','changed-dsdt']:
  p=Path(d)/mode;efi=p/'EFI/OC';port=p/'iGPU-Port';efi.mkdir(parents=True);port.mkdir();save=port/'backups/backend4g-touchpad-before-20261020/EFI/OC';save.mkdir(parents=True)
  for name in ['config.plist','ACPI/DSDT.aml']:
   f=efi/name;f.parent.mkdir(exist_ok=True);shutil.copy2(usb/'EFI/OC'/name,f)
   f=save/name;f.parent.mkdir(exist_ok=True);shutil.copy2(old/name,f)
  (efi/'Kexts/LatitudeI2CMeteorLake.kext').mkdir(parents=True);(efi/'ACPI/SSDT-LAT-TPD0.aml').write_text('candidate')
  shutil.copy2(script,port/script.name)
  if mode=='changed-config':(efi/'config.plist').write_bytes((efi/'config.plist').read_bytes()+b'\n')
  if mode=='changed-dsdt':(efi/'ACPI/DSDT.aml').write_bytes((efi/'ACPI/DSDT.aml').read_bytes()+b'\x00')
  before={n:(efi/n).read_bytes() for n in ['config.plist','ACPI/DSDT.aml']}
  c=subprocess.run(['/bin/bash',str(port/script.name)],capture_output=True,text=True)
  if mode=='success':
   assert c.returncode==0,c.stdout+c.stderr
   for n in before:assert (efi/n).read_bytes()==(old/n).read_bytes()
   assert not (efi/'Kexts/LatitudeI2CMeteorLake.kext').exists() and not (efi/'ACPI/SSDT-LAT-TPD0.aml').exists()
  else:
   assert c.returncode==2,c.stdout+c.stderr
   for n,b in before.items():assert (efi/n).read_bytes()==b
   assert (efi/'Kexts/LatitudeI2CMeteorLake.kext').exists()
 print('PASS actual touchpad rollback in isolated fixtures: restores exact saved config/DSDT, removes only candidate bundle/SSDT; later config/DSDT edits refused without mutation. Active USB unchanged by test.')
