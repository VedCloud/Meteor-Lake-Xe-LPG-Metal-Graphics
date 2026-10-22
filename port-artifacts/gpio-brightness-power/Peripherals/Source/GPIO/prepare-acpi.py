from pathlib import Path
import sys,subprocess,plistlib,json,hashlib
r=Path(__file__).resolve().parent.parent;sys.path.insert(0,str(r/'touchpad-sources'));from aml_methods import methods,device_range,replace_bodies
v=r/'port-artifacts/gpio-brightness-power';v.mkdir(exist_ok=True)
s=(r/'touchpad-sources/SSDT-LAT-TPD0.dsl').read_text().replace('External (\\_SB.PC00.I2C3.TPD0.SBFI, BuffObj)','External (\\_SB.PC00.I2C3.TPD0.SBFG, BuffObj)').replace('ConcatenateResTemplate (SBFB, SBFI)','ConcatenateResTemplate (SBFB, SBFG)')
(v/'SSDT-LAT-TPD0.dsl').write_text(s);subprocess.run(['/opt/homebrew/bin/iasl','-p',str(v/'SSDT-LAT-TPD0'),str(v/'SSDT-LAT-TPD0.dsl')],check=True,capture_output=True)
ssdt=(v/'SSDT-LAT-TPD0.aml').read_bytes();bodies={n:ssdt[a:b] for n,(a,b) in methods(ssdt).items()};base=Path('/Volumes/SEQOUIA/EFI/OC/ACPI/DSDT.aml').read_bytes();a,b=device_range(base,b'TPD0');patched,changes=replace_bodies(base,bodies,a,b);(v/'DSDT.aml').write_bytes(patched)
allowed={9}
for x in changes:allowed.update(range(x['flags_offset'],x['end']))
assert all(i in allowed for i,(a,b) in enumerate(zip(base,patched)) if a!=b)
subprocess.run(['/opt/homebrew/bin/iasl','-d','-p',str(v/'DSDT'),str(v/'DSDT.aml')],check=True,capture_output=True)
c=plistlib.loads(Path('/Volumes/SEQOUIA/iGPU-Port/Peripherals/Touchpad/config.plist').read_bytes());props=c['DeviceProperties']['Add'].get('PciRoot(0x0)/Pci(0x15,0x3)',{});props.pop('force-polling',None)
e=dict(Arch='x86_64',BundlePath='LatitudeGPIO.kext',Comment='Meteor Lake INTC1083, ELAN GPIO387 only',Enabled=True,ExecutablePath='Contents/MacOS/LatitudeGPIO',PlistPath='Contents/Info.plist',MinKernel='24.0.0',MaxKernel='24.99.99');idx=next(i for i,e in enumerate(c['Kernel']['Add']) if e['BundlePath'].endswith('/VoodooGPIO.kext'))+1;c['Kernel']['Add'].insert(idx,e)
assert '-vi2c-force-polling' not in c['NVRAM']['Add']['7C436110-AB2A-4BBB-A880-FE41995C9F82']['boot-args']
(v/'config.plist').write_bytes(plistlib.dumps(c));(v/'ACPI-VALIDATION.json').write_text(json.dumps({'scoped':'TPD0 _DSM/_CRS only','changes':changes,'checksum':sum(patched)%256,'linux_gpio_id':'INTC1083','gpio':387,'physical_pad':232,'group':'GPP_D_3','gpio_resource':'Dell runtime SBFG, no guessed pin injection','hardware_verified':False},indent=2)+'\n')
subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(v/'config.plist')],check=True)
# Execute exact new method bodies in the prior firmware fixture, now using SBFG.
s=(r/'touchpad-sources/test-inline-acpi.py').read_text();s=s.replace('r=Path(__file__).resolve().parent.parent','r=Path(__file__).resolve().parents[2]')
s=s.replace("target=(r/'port-artifacts/touchpad-inline-1.0.1/DSDT.aml').read_bytes()","target=(r/'port-artifacts/gpio-brightness-power/DSDT.aml').read_bytes()")
s=s.replace("fixture='\\n'.join", "fixture=fixture.replace('SBFI','SBFG')\nfixture='\\n'.join",1)
s=s.replace("print('PASS exact patched DSDT", "print('PASS GPIO resource patched DSDT")
(v/'test-acpi.py').write_text(s);subprocess.run([sys.executable,str(v/'test-acpi.py')],env={**__import__('os').environ,'PYTHONPATH':str(r/'touchpad-sources')},check=True)
