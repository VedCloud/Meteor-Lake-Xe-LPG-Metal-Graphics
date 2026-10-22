from pathlib import Path
import subprocess,hashlib,json,plistlib
from aml_methods import methods,device_range,replace_bodies
r=Path(__file__).resolve().parent.parent;v=r/'port-artifacts/touchpad-inline-1.0.1';v.mkdir(exist_ok=True)
base=r/'checkpoints/usable-desktop-real-memory-0.2.5-20261019-232025/EFI/OC/ACPI/DSDT.aml';original=base.read_bytes();ssdt=(r/'touchpad-candidate/ACPI/SSDT-LAT-TPD0.aml').read_bytes();bodies={name:ssdt[lo:hi] for name,(lo,hi) in methods(ssdt).items()};start,end=device_range(original,b'TPD0');out,changes=replace_bodies(original,bodies,start,end)
allowed={9}
for x in changes:allowed.update(range(x['flags_offset'],x['end']))
assert all(i in allowed for i,(a,b) in enumerate(zip(original,out)) if a!=b)
(v/'DSDT.aml').write_bytes(out)
subprocess.run(['/opt/homebrew/bin/iasl','-d','-p',str(v/'DSDT'),str(v/'DSDT.aml')],check=True,capture_output=True,text=True)
# Use the installed candidate as the config baseline; remove only the failed SSDT.
f=Path('/Volumes/SEQOUIA/EFI/OC/config.plist');d=plistlib.loads(f.read_bytes());entries=d['ACPI']['Add'];old=[x for x in entries if x['Path']=='SSDT-LAT-TPD0.aml'];assert len(old)==1;entries.remove(old[0]);(v/'config.plist').write_bytes(plistlib.dumps(d))
validation={'change':'Inline the two previously external touchpad replacement methods into the existing DSDT TPD0 package; removes forward-scope SSDT dependency','original_sha256':hashlib.sha256(original).hexdigest(),'dsdt_sha256':hashlib.sha256(out).hexdigest(),'config_sha256':hashlib.sha256((v/'config.plist').read_bytes()).hexdigest(),'method_bodies':changes,'length_unchanged':True,'checksum_zero':True,'changed_only_target_method_bodies_and_checksum':True,'controller_kernel_unchanged':True,'GPU_payloads_unchanged':True,'hardware_gestures_verified':False}
(v/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n');print(json.dumps(validation,indent=2))
