"""Stage a guest-only native CoreDisplay preference in a fresh HFS clone.
Original recovery images, Apple binaries and host preferences stay unchanged.
"""
from pathlib import Path
import datetime, hashlib, json, os, plistlib, struct, subprocess, mmap
from hfs_owned_image_metadata import OwnedHFS
R=Path(__file__).resolve().parent
H=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment')
src=H/'BaseSystem-26A428-HFS-Diagnostics-v4.dmg'
dst=H/'BaseSystem-26A428-HFS-LegacyDisplay-v6.dmg'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
expected='432254146761371876b4127829a31c34bdf9ab4f637ae0f713a7ce435c5d36ba'
assert not dst.exists() and sha(src)==expected
subprocess.run(['/bin/cp','-c',str(src),str(dst)],check=True)
folder='System/Library/Templates/Data/Library/Preferences';rel=folder+'/com.apple.CoreDisplay.plist'
h=OwnedHFS(dst);saved=h.bsd_info(folder)
assert struct.unpack_from('>II',saved)==(0,0)
assert (h.lookup(folder)[1])==1
stage=bytearray(saved);struct.pack_into('>II',stage,0,os.getuid(),os.getgid());stage[8]&=~8
h.bsd_info(folder,stage);h.close()
def plist_command(args):
 # hdiutil can append warnings after its XML; parse precisely one document.
 data=subprocess.check_output(args);end=data.index(b'</plist>')+len(b'</plist>')
 return plistlib.loads(data[:end])
attached=plist_command(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)])
dev=attached['system-entities'][0]['dev-entry']
mount=Path(next(x['mount-point'] for x in attached['system-entities'] if 'mount-point' in x))
payload=plistlib.dumps({'useGPUWrangler':False})
try:
 p=mount/rel;assert not p.exists() and p.parent.stat().st_uid==os.getuid()
 with p.open('xb') as f:f.write(payload)
 p.chmod(0o644)
 assert plistlib.loads(p.read_bytes())=={'useGPUWrangler':False}
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
h=OwnedHFS(dst)
try:
 h.owner(rel,(0,0));h.bsd_info(folder,saved)
 assert h.bsd_info(folder)==saved
 at,kind=h.lookup(rel);assert kind==2
 assert struct.unpack_from('>II',h.bsd_info(rel))==(0,0)
finally:h.close()
attached=plist_command(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)])
dev=attached['system-entities'][0]['dev-entry']
try:
 check=subprocess.run(['diskutil','verifyVolume',dev],capture_output=True,text=True)
 assert check.returncode==0,check.stdout+check.stderr
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
old=(R/'BootContextExperiment').resolve()
profile=old.parent/'BootContextExperiment-LegacyDisplay-v6';assert not profile.exists();profile.mkdir()
meta=json.loads((old/'VMAPPLE-REFERENCE-BOOT.json').read_text())
capture=profile/'vmapple-reference-boot-state.bin'
assert sha(old/capture.name)==meta['sha256']
subprocess.run(['/bin/cp','-c',str(old/capture.name),str(capture)],check=True)
start=meta['recoveryImagePhysical']-meta['guestBase'];size=meta['recoveryImageBytes'];changed=0
with capture.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);before=hashlib.sha256(im[start:start+size]).hexdigest()
 assert before==meta['recoveryImageSHA256']==expected
 with dst.open('rb') as inp:
  for off in range(0,size,1024*1024):
   data=inp.read(min(1024*1024,size-off))
   if im[start+off:start+off+len(data)]!=data:
    im[start+off:start+off+len(data)]=data;changed+=len(data)
 im.flush();digest=hashlib.sha256(im).hexdigest();im.close()
meta.update(parentCaptureSHA256=meta['sha256'],sha256=digest,recoveryImageSHA256=sha(dst),inputRelativePath='EngineProbe/vmapple-'+digest+'.bin',displayDiscoveryPreference={'domain':'com.apple.CoreDisplay','useGPUWrangler':False},graphicalInstallerVerified=False)
head=(old/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]
head+='static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(digest))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+meta['inputRelativePath'].replace('/','\\'))+'\n'
(profile/'vmapple_reference_boot.h').write_text(head)
(profile/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(meta,indent=2)+'\n')
(profile/'BOOT-CONTEXT-PREPARATION.json').write_bytes((old/'BOOT-CONTEXT-PREPARATION.json').read_bytes())
assert sha(src)==expected
report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'profile':str(profile),'image':str(dst),'imageSHA256':sha(dst),'inputSHA256':digest,'sourceUnchanged':True,'guestPreference':rel,'preference':{'useGPUWrangler':False},'filesystemVerification':check.stdout+check.stderr,'changedCaptureBytes':changed,'AppleExecutableBytesChanged':False,'hostPreferencesChanged':False,'runtimeVerified':False}
(R/'LEGACY-DISPLAY-PREPARATION.json').write_text(json.dumps(report,indent=2)+'\n')
# Repoint only this isolated candidate after all checks succeeded.
(R/'BootContextExperiment').unlink();(R/'BootContextExperiment').symlink_to(profile,target_is_directory=True)
print(json.dumps(report,indent=2))
