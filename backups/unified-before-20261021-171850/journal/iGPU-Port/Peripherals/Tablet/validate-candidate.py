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
 assert (fixture/'EFI/OC/config.plist').read_bytes()==original
 keyboard=fixture/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist';before=sha(keyboard)
 for action in ['apply','restore']:subprocess.run(['/bin/bash',str(p/'Peripherals/touchpad-trial.sh'),action],check=True,capture_output=True)
 assert (fixture/'EFI/OC/config.plist').read_bytes()==original and sha(keyboard)==before
 assert (fixture/'EFI/OC/Kexts/LatitudeTabletObserver.kext/Contents/MacOS/LatitudeTabletObserver').is_file()
for name,value in json.loads((port/'SHA256SUMS.json').read_text()).items():assert sha(port/name)==value,name
for path in [p for p in (port/'Peripherals/Tablet').glob('*.sh') if not p.name.startswith('._')]+[port/'collect-desktop-failure.sh',port/'Peripherals/touchpad-trial.sh']:subprocess.run(['/bin/bash','-n',str(path)],check=True)
print('PASS actual candidate apply/restore and accelerated/recovery scripts in isolated EFI fixture; Copilot, persistent graphics and passive observer retained; package hashes and shell syntax valid. Active USB not switched by test.')
