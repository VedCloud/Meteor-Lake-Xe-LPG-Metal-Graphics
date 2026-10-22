from pathlib import Path
import subprocess,hashlib,json
r=Path(__file__).resolve().parent.parent;v=r/'port-artifacts/touchpad-mtl-1.0.0';out=r/'touchpad-candidate/ACPI';out.mkdir(exist_ok=True)
source=r/'checkpoints/usable-desktop-real-memory-0.2.5-20261019-232025/EFI/OC/ACPI/DSDT.aml';data=bytearray(source.read_bytes())
def pkg(at):
 first=data[at];more=first>>6
 length=(first&0x3f) if not more else first&0xf
 for j in range(more):length|=data[at+j+1]<<(4+8*j)
 return more+1,length
found=[]
for i in range(36,len(data)-8):
 if data[i:i+2]==b'\x5b\x82':
  size,length=pkg(i+2);start=i+2+size
  if data[start:start+4]==b'TPD0':found.append((start+4,i+2+length))
assert len(found)==1,found
start,end=found[0];assert start<end<=len(data)
changes=[]
for old,new,nargs in [(b'_DSM',b'XDSM',4),(b'_CRS',b'XCRS',0)]:
 hits=[]
 for i in range(start,end-8):
  if data[i]==0x14:
   size,length=pkg(i+1);name=i+1+size
   if data[name:name+4]==old and name+4<end and data[name+4]&7==nargs and i+1+length<=end:hits.append(name)
 assert len(hits)==1,(old,hits)
 at=hits[0];changes.append({'offset':at,'old':old.decode(),'new':new.decode()});data[at:at+4]=new
# ACPI table checksum is the only additional byte changed.
data[9]=0;data[9]=(-sum(data))&255;assert sum(data)%256==0
(out/'DSDT.aml').write_bytes(data)
subprocess.run(['/opt/homebrew/bin/iasl','-p',str(out/'SSDT-LAT-TPD0'),str(r/'touchpad-sources/SSDT-LAT-TPD0.dsl')],check=True,capture_output=True,text=True)
subprocess.run(['/opt/homebrew/bin/iasl','-d','-p',str(v/'acpi/DSDT-patched'),str(out/'DSDT.aml')],check=True,capture_output=True,text=True)
original=source.read_bytes();changed=[i for i,(a,b) in enumerate(zip(original,data)) if a!=b]
allowed={9}
for x in changes:allowed.update(range(x['offset'],x['offset']+4))
assert set(changed)<=allowed
validation={'source_sha256':hashlib.sha256(original).hexdigest(),'patched_sha256':hashlib.sha256(data).hexdigest(),'table_length_unchanged':len(original)==len(data),'checksum_zero':True,'scoped_device':'_SB.PC00.I2C3.TPD0','method_renames':changes,'changed_offsets':changed,'new_methods':'_DSM returns firmware TPDH directly for HID descriptor; _CRS concatenates firmware SBFB and SBFI, polling controller avoids GPIO routing; _STA and _INI untouched','hardware_verified':False}
(v/'ACPI-VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n');print(json.dumps(validation,indent=2))
