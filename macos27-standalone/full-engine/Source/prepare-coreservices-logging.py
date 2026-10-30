"""Isolated native ASL environment test; no executable patches or error bypass.
CoreServices joins the previously configured LaunchServices/OpenDirectory legacy logger experiment. The
existing firmware service-error observers remain. Original images untouched.
"""
from pathlib import Path
import datetime,hashlib,json,os,plistlib,struct,subprocess,mmap
from hfs_owned_image_metadata import OwnedHFS
R=Path(__file__).resolve().parent
H=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/HFSRecoveryExperiment')
src=H/'BaseSystem-26A428-HFS-ServiceLogging-v10.dmg';dst=H/'BaseSystem-26A428-HFS-ServiceLogging-v11.dmg'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
expected='2d15421d386bf24359a249474f6df3c0706eb5d273f3e9540012acf0b3845e12'
assert not dst.exists() and sha(src)==expected
subprocess.run(['/bin/cp','-c',str(src),str(dst)],check=True)
folder='System/Library/LaunchDaemons';names=['com.apple.coreservicesd.plist']
h=OwnedHFS(dst);saved=h.bsd_info(folder);assert struct.unpack_from('>II',saved)==(0,0)
stage=bytearray(saved);struct.pack_into('>II',stage,0,os.getuid(),os.getgid());stage[8]&=~8;h.bsd_info(folder,stage);h.close()
def plist(args):
 data=subprocess.check_output(args);return plistlib.loads(data[data.index(b'<?xml'):data.index(b'</plist>')+8])
a=plist(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)])
dev=a['system-entities'][0]['dev-entry'];mount=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point' in e));changes=[]
try:
 for name in names:
  p=mount/folder/name;old=p.read_bytes();before=plistlib.loads(old);after={**before,'EnvironmentVariables':{**before.get('EnvironmentVariables',{}),'ASL_DISABLE':'1'}}
  data=plistlib.dumps(after);p.unlink();p.write_bytes(data);p.chmod(0o644)
  assert plistlib.loads(p.read_bytes())==after
  assert {k:v for k,v in after.items() if k!='EnvironmentVariables'}=={k:v for k,v in before.items() if k!='EnvironmentVariables'}
  changes.append({'path':folder+'/'+name,'beforeSHA256':hashlib.sha256(old).hexdigest(),'afterSHA256':hashlib.sha256(data).hexdigest(),'change':{'ASL_DISABLE':'1'},'programUnchanged':all(after.get(k)==before.get(k) for k in ['Program','ProgramArguments','MachServices','Label'])})
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
h=OwnedHFS(dst)
try:
 for name in names:h.owner(folder+'/'+name,(0,0))
 h.bsd_info(folder,saved);assert h.bsd_info(folder)==saved
finally:h.close()
a=plist(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)]);dev=a['system-entities'][0]['dev-entry']
try:
 check=subprocess.run(['diskutil','verifyVolume',dev],capture_output=True,text=True);assert check.returncode==0,check.stdout+check.stderr
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
old=(R/'BootContextExperiment').resolve();profile=old.parent/'BootContextExperiment-ServiceLogging-v11';assert not profile.exists();profile.mkdir()
meta=json.loads((old/'VMAPPLE-REFERENCE-BOOT.json').read_text());capture=profile/'vmapple-reference-boot-state.bin'
assert sha(old/capture.name)==meta['sha256'];subprocess.run(['/bin/cp','-c',str(old/capture.name),str(capture)],check=True)
at=meta['recoveryImagePhysical']-meta['guestBase'];size=meta['recoveryImageBytes'];assert meta['recoveryImageSHA256']==expected
with capture.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);checkhash=hashlib.sha256()
 for off in range(0,size,1024*1024):checkhash.update(im[at+off:at+min(size,off+1024*1024)])
 assert checkhash.hexdigest()==expected
 with dst.open('rb') as inp:
  for off in range(0,size,1024*1024):
   b=inp.read(min(1024*1024,size-off))
   if im[at+off:at+off+len(b)]!=b:im[at+off:at+off+len(b)]=b
 im.flush();digest=hashlib.sha256(im).hexdigest();im.close()
meta.update(parentCaptureSHA256=meta['sha256'],sha256=digest,recoveryImageSHA256=sha(dst),inputRelativePath='EngineProbe/vmapple-'+digest+'.bin',serviceLoggingExperiment=changes,graphicalInstallerVerified=False)
head=(old/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]
head+='static const uint8_t vmref_sha256[32]={'+','.join(str(v) for v in bytes.fromhex(digest))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+meta['inputRelativePath'].replace('/','\\'))+'\n'
(profile/'vmapple_reference_boot.h').write_text(head);(profile/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(meta,indent=2)+'\n');(profile/'BOOT-CONTEXT-PREPARATION.json').write_bytes((old/'BOOT-CONTEXT-PREPARATION.json').read_bytes())
assert sha(src)==expected
report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'profile':str(profile),'image':str(dst),'imageSHA256':sha(dst),'inputSHA256':digest,'originalUnchanged':True,'changes':changes,'filesystemVerification':check.stdout+check.stderr,'AppleExecutableBytesChanged':False,'hostLoggingChanged':False,'cryptoAndDisplayResultsOverridden':False,'runtimeEffectVerified':False,'graphicalInstallerVerified':False}
(R/'CORESERVICES-LOGGING-PREPARATION.json').write_text(json.dumps(report,indent=2)+'\n');(R/'BootContextExperiment').unlink();(R/'BootContextExperiment').symlink_to(profile,target_is_directory=True);print(json.dumps(report,indent=2))
