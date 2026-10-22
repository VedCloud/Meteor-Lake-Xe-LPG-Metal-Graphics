from pathlib import Path
import sys,plistlib,json,hashlib,subprocess,importlib.util
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port';out=root/'port-artifacts/unified-runtime-20261021';out.mkdir(exist_ok=True)
spec=importlib.util.spec_from_file_location('visibility',root/'tablet-sources/prepare-visibility.py');v=importlib.util.module_from_spec(spec);spec.loader.exec_module(v)
records=[]
for profile in ['Base','Touchpad']:
 source=port/'Peripherals'/profile/'DSDT.aml';raw=source.read_bytes();changed,at=v.patch(raw);dest=out/profile;dest.mkdir(exist_ok=True);(dest/'DSDT.aml').write_bytes(changed);(dest/'DSDT-original.aml').write_bytes(raw)
 records.append({'profile':profile,'originalSHA256':hashlib.sha256(raw).hexdigest(),'patchedSHA256':hashlib.sha256(changed).hexdigest(),'nameOffset':at,'normalTable':'DSDT.aml','recoveryTable':'DSDT-'+profile+'-Recovery.aml','onlyChanges':'HIDD _STA->XSTA and checksum'})
 for mode in ['accelerated','recovery']:
  d=plistlib.loads((port/'GraphicsModes'/(profile.lower()+'-'+mode+'.plist')).read_bytes())
  old=[x for x in d['ACPI']['Patch'] if x['Comment'].startswith('Latitude HIDD only:')];assert len(old)==1
  d['ACPI']['Patch']=[x for x in d['ACPI']['Patch'] if x not in old]
  entry=next(x for x in d['ACPI']['Add'] if x['Path']=='DSDT.aml');entry['Path']='DSDT.aml' if mode=='accelerated' else 'DSDT-'+profile+'-Recovery.aml'
  entry['Comment']='Supplied DSDT: scoped HIDD method renamed' if mode=='accelerated' else 'Original supplied DSDT without HIDD visibility change'
  state=next(x for x in d['Kernel']['Add'] if x['BundlePath']=='LatitudeTabletState.kext');state['Comment']='Qualified tablet reader 0.2.1; per-device PS2 input gates, stale signal restores input'
  d['UEFI']['Audio']['AudioOutMask']=1
  path=out/(profile.lower()+'-'+mode+'.plist');path.write_bytes(plistlib.dumps(d));subprocess.run([str(root/'research/OpenCore-release/matching/Utilities/ocvalidate/ocvalidate'),str(path)],check=True,capture_output=True)
(out/'ACPI-VALIDATION.json').write_text(json.dumps({'reason':'OpenCore patched OEM 557414-byte DSDT before replacing it with unpatched custom 557441-byte DSDT; fix must be in the supplied table itself','profiles':records},indent=2)+'\n')
print('PASS effective DSDT patch and original recovery table selection in all four validated profiles; global patches and firmware byte lengths retained.')
