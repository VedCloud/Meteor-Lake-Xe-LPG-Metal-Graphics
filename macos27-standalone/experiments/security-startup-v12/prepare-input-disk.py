from pathlib import Path
import json,subprocess,hashlib,mmap,plistlib,datetime
r=Path(__file__).resolve().parent;base=Path('/Volumes/Dell27Build/PresentationEfficiencyBuild/BootContextExperiment').resolve();new=(r/'BootContextExperiment').resolve();src=base/'test-inputs.raw';dst=new/'test-inputs.raw'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def pl(args):
 b=subprocess.check_output(args);return plistlib.loads(b[b.index(b'<?xml'):b.index(b'</plist>')+8])
old=json.loads((base/'VMAPPLE-REFERENCE-BOOT.json').read_text());meta=json.loads((new/'VMAPPLE-REFERENCE-BOOT.json').read_text());assert not dst.exists();before=sha(src);subprocess.run(['/bin/cp','-c',str(src),str(dst)],check=True)
a=pl(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)]);dev=a['system-entities'][0]['dev-entry'];point=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point'in e));changed=0
try:
 target=point/old['inputRelativePath'];assert sha(target)==old['sha256'];dest=point/meta['inputRelativePath'];assert not dest.exists();target.rename(dest);assert dest.stat().st_size==(new/'vmapple-reference-boot-state.bin').stat().st_size
 with dest.open('r+b') as out,(new/'vmapple-reference-boot-state.bin').open('rb') as inp:
  mm=mmap.mmap(out.fileno(),0)
  for offset in range(0,len(mm),1024*1024):
   data=inp.read(min(1024*1024,len(mm)-offset))
   if mm[offset:offset+len(data)]!=data:mm[offset:offset+len(data)]=data;changed+=1
  mm.flush();mm.close()
 assert sha(dest)==meta['sha256']
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
assert sha(src)==before
record={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'image':str(dst),'imageSHA256':sha(dst),'inputSHA256':meta['sha256'],'sourceRawUnchanged':True,'changedMiBChunks':changed,'graphicalInstallerVerified':False};(new/'EXFAT-TEST-IMAGE.json').write_text(json.dumps(record,indent=2)+'\n');print(json.dumps(record))
