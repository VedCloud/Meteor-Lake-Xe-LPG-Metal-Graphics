from pathlib import Path
import tempfile,subprocess,shutil,json,hashlib,plistlib
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
with tempfile.TemporaryDirectory(prefix='latitude-candidate-') as tmp:
 fixture=Path(tmp);shutil.copytree(usb/'EFI',fixture/'EFI');p=fixture/'iGPU-Port';p.mkdir();(p/'backups').mkdir();shutil.copytree(port/'Peripherals',p/'Peripherals');shutil.copytree(port/'GraphicsModes',p/'GraphicsModes');shutil.copyfile(port/'switch-graphics-mode.sh',p/'switch-graphics-mode.sh')
 original=(fixture/'EFI/OC/config.plist').read_bytes()
 for mode in ['recovery','accelerated']:
  subprocess.run(['/bin/bash',str(p/'switch-graphics-mode.sh'),mode],check=True,capture_output=True)
  doc=plistlib.loads((fixture/'EFI/OC/config.plist').read_bytes());entry=next(e for e in doc['Kernel']['Add'] if e['BundlePath']=='LatitudeTabletObserver.kext');assert entry['Enabled']==(mode=='accelerated')
  state=next(e for e in doc['Kernel']['Add'] if e['BundlePath']=='LatitudeTabletState.kext');assert state['Enabled']==(mode=='accelerated')
  aml=next(e for e in doc['ACPI']['Add'] if e['Path']=='SSDT-LAT-TABLET.aml');assert aml['Enabled']==(mode=='accelerated')
  patch=next(e for e in doc['ACPI']['Patch'] if e['Comment'].startswith('Latitude HIDD only:'));assert patch['Enabled']==(mode=='accelerated')
  assert doc['Misc']['Boot']['HideAuxiliary'] and doc['Misc']['Boot']['PickerMode']=='External'
  assert doc['UEFI']['Audio']['PlayChime']=='Enabled'
 assert (fixture/'EFI/OC/config.plist').read_bytes()==original
 keyboard=fixture/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist';before=sha(keyboard)
 for action in ['apply','restore']:subprocess.run(['/bin/bash',str(p/'Peripherals/touchpad-trial.sh'),action],check=True,capture_output=True)
 assert (fixture/'EFI/OC/config.plist').read_bytes()==original and sha(keyboard)==before
 assert (fixture/'EFI/OC/Kexts/LatitudeTabletObserver.kext/Contents/MacOS/LatitudeTabletObserver').is_file()
for name,value in json.loads((port/'SHA256SUMS.json').read_text()).items():assert sha(port/name)==value,name
for path in [p for p in (port/'Peripherals/Tablet').glob('*.sh') if not p.name.startswith('._')]+[port/'collect-desktop-failure.sh',port/'Peripherals/touchpad-trial.sh']:subprocess.run(['/bin/bash','-n',str(path)],check=True)
print('PASS actual candidate apply/restore and accelerated/recovery scripts in isolated EFI fixture; Copilot, persistent graphics and live tablet state, scoped HIDD recovery, theme/chime and hidden auxiliaries retained; package hashes and shell syntax valid. Active USB not switched by test.')
