from pathlib import Path
import sys,hashlib,json,subprocess
root=Path(__file__).resolve().parent.parent;sys.path.insert(0,str(root/'touchpad-sources'));from aml_methods import device_range,package
out=root/'port-artifacts/tablet-mode-v2';out.mkdir(exist_ok=True)
def patch(data):
 lo,hi=device_range(data,b'HIDD');candidates=[]
 for at in range(lo,hi-8):
  if data[at]!=0x14:continue
  size,length=package(data,at+1);name=at+1+size;end=at+1+length
  if end<=hi and data[name:name+4]==b'_STA' and data[name+4]==8:candidates.append((name,end))
 assert len(candidates)==1,candidates
 at,end=candidates[0];assert b'OIDE' in data[at:end] and b'XSTA' not in data[lo:hi]
 result=bytearray(data);result[at:at+4]=b'XSTA';result[9]=0;result[9]=(-sum(result))&255
 assert sum(result)%256==0 and len(result)==len(data)
 assert all(i==9 or at<=i<at+4 for i,(a,b) in enumerate(zip(data,result)) if a!=b)
 return result,at
if __name__=='__main__':
 records=[]
 for profile in ['Base','Touchpad']:
  src=Path('/Volumes/SEQOUIA/iGPU-Port/Peripherals')/profile/'DSDT.aml';data=src.read_bytes();modified,at=patch(data);dest=out/profile/'DSDT.aml';dest.parent.mkdir(exist_ok=True);dest.write_bytes(modified)
  records.append({'profile':profile,'originalSHA256':hashlib.sha256(data).hexdigest(),'modifiedSHA256':hashlib.sha256(modified).hexdigest(),'HIDD_STANameOffset':at,'sameLength':True,'onlyChange':'HIDD _STA->XSTA plus ACPI checksum'})
 subprocess.run(['/opt/homebrew/bin/iasl','-p',str(out/'SSDT-LAT-TABLET'),str(root/'tablet-sources/SSDT-LAT-TABLET.dsl')],check=True,capture_output=True)
 (out/'ACPI-VALIDATION.json').write_text(json.dumps(records,indent=2)+'\n');print('PASS targeted HIDD _STA rename in both DSDTs, exact byte scope and checksums; new SSDT compiled.')
