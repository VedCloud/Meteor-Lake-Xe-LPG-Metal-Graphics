"""Prepare a Dell-specific candidate; never edit the mounted EFI here."""
from pathlib import Path
import sys, shutil, plistlib, hashlib, json, subprocess
r=Path(__file__).resolve().parent.parent
sys.path.insert(0,str(r/'touchpad-sources'))
from aml_methods import methods,device_range,replace_bodies
out=r/'port-artifacts/peripherals-20261021';out.mkdir(exist_ok=True)
def cp(a,b):
 b.parent.mkdir(parents=True,exist_ok=True)
 if a.is_dir():shutil.copytree(a,b,dirs_exist_ok=True)
 else:shutil.copy2(a,b)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
usb=Path('/Volumes/SEQOUIA');cfg=usb/'EFI/OC/config.plist'
d=plistlib.loads(cfg.read_bytes());original=plistlib.loads(cfg.read_bytes())
def entry(name):return dict(Arch='x86_64',BundlePath=name+'.kext',Comment='Latitude peripheral candidate; Sequoia only',Enabled=True,ExecutablePath='Contents/MacOS/'+name,PlistPath='Contents/Info.plist',MinKernel='24.0.0',MaxKernel='24.99.99')
sources={'IntelBluetoothFirmware':r/'research/bluetooth-i2c-a16/IntelBluetoothFirmware/IntelBluetoothFirmware.kext','IntelBTPatcher':r/'research/bluetooth-i2c-a16/IntelBluetoothFirmware/IntelBTPatcher.kext','BlueToolFixup':r/'research/bluetooth-i2c-a16/BrcmPatchRAM/BlueToolFixup.kext','LatitudeHotkeys':r/'hotkey-build/LatitudeHotkeys.kext'}
for name,source in sources.items():
 assert not any(e['BundlePath']==name+'.kext' for e in d['Kernel']['Add']),name
 cp(source,out/'Kexts'/source.name);d['Kernel']['Add'].append(entry(name))
 # Release archives may have no signature. Preserve original binaries; inspect architecture.
 subprocess.run(['lipo','-verify_arch','x86_64',str(source/'Contents/MacOS'/name)],check=True)
guid='7C436110-AB2A-4BBB-A880-FE41995C9F82'
for key,value in [('bluetoothExternalDongleFailed',bytes(1)),('bluetoothInternalControllerInfo',bytes(14))]:
 d['NVRAM']['Add'].setdefault(guid,{})[key]=value
 a=d['NVRAM']['Delete'].setdefault(guid,[])
 if key not in a:a.append(key)
(out/'config.plist').write_bytes(plistlib.dumps(d))
cp(r/'desktop-build/hotkey-counters.aml',out/'SSDT-LAT-BRT.aml')
assert int.from_bytes((out/'SSDT-LAT-BRT.aml').read_bytes()[24:28],'little')==5
# Optional I2C trial patches only the current DSDT's two TPD0 methods.
trial=plistlib.loads(plistlib.dumps(d));base=(usb/'EFI/OC/ACPI/DSDT.aml').read_bytes()
ssdt=(r/'touchpad-candidate/ACPI/SSDT-LAT-TPD0.aml').read_bytes()
bodies={n:ssdt[lo:hi] for n,(lo,hi) in methods(ssdt).items()}
start,end=device_range(base,b'TPD0');patched,changes=replace_bodies(base,bodies,start,end)
allowed={9}
for change in changes:allowed.update(range(change['flags_offset'],change['end']))
assert len(base)==len(patched) and sum(patched)%256==0
assert all(i in allowed for i,(a,b) in enumerate(zip(base,patched)) if a!=b)
(out/'Touchpad').mkdir(exist_ok=True);(out/'Touchpad/DSDT.aml').write_bytes(patched)
subprocess.run(['/opt/homebrew/bin/iasl','-d','-p',str(out/'Touchpad/DSDT'),str(out/'Touchpad/DSDT.aml')],check=True,capture_output=True)
cp(r/'touchpad-candidate/LatitudeI2CMeteorLake.kext',out/'Touchpad/LatitudeI2CMeteorLake.kext')
e=entry('LatitudeI2CMeteorLake');idx=next(i for i,e in enumerate(trial['Kernel']['Add']) if e['BundlePath']=='VoodooI2C.kext')+1;trial['Kernel']['Add'].insert(idx,e)
disabled=[]
for e in trial['Kernel']['Add']:
 if e['BundlePath'] in ['VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Mouse.kext','VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Trackpad.kext']:
  e['Enabled']=False;disabled.append(e['BundlePath'])
assert len(disabled)==2
trial['DeviceProperties']['Add'].setdefault('PciRoot(0x0)/Pci(0x15,0x3)',{})['force-polling']=bytes([1])
assert sum(e['Enabled'] for e in trial['Kernel']['Add'] if e['BundlePath'].endswith('/VoodooInput.kext'))==1
assert next(e for e in trial['Kernel']['Add'] if e['BundlePath'].endswith('/VoodooPS2Keyboard.kext'))['Enabled']
(out/'Touchpad/config.plist').write_bytes(plistlib.dumps(trial))
# Every base setting outside the specific additions is preserved.
check=plistlib.loads(plistlib.dumps(d));check['Kernel']['Add']=check['Kernel']['Add'][:-len(sources)]
check['NVRAM']=original['NVRAM'];assert check==original
validation={'status':'host-validated candidate; Dell boot required','bluetooth_usb_id':'8087:0033','intel_version':'2.5.1 fork','bluetool_version':'2.7.2 official release (reference advertises 2.7.3, not an available release)','Lilu':'existing 1.7.3 retained','touchpad':'ELAN04F3:328E; 8086:7e7b, Dell 1028:0cb6; opt-in trial','touchpad_method_changes':changes,'touchpad_disabled_ps2_pointer_plugins':disabled,'base_config_sha256':sha(cfg),'base_dsdt_sha256':sha(usb/'EFI/OC/ACPI/DSDT.aml'),'default_config_sha256':sha(out/'config.plist'),'trial_config_sha256':sha(out/'Touchpad/config.plist'),'trial_dsdt_sha256':sha(out/'Touchpad/DSDT.aml'),'hardware_verified':False,'full_gpu_tests_rerun':False}
(out/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n')
for f in [out/'config.plist',out/'Touchpad/config.plist']:
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(f)],check=True)
print('Prepared Bluetooth/WMI candidate and separate touchpad trial:',out)
