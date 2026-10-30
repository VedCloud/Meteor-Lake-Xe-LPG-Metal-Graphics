# SPDX-License-Identifier: MIT
"""Add a guest-only native virtual-display/presentation agent to an owned COW image.
Only added helper/plist/marker files differ. No host display or security changes.
Original recovery images and the running v12 capture are immutable inputs.
"""
from pathlib import Path
import datetime,hashlib,json,os,plistlib,struct,subprocess,mmap,sys
sys.path.insert(0,'/Volumes/Dell27Build/PresentationEfficiencyBuild')
from hfs_owned_image_metadata import OwnedHFS
R=Path(__file__).resolve().parent;P=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research');H=P/'HFSRecoveryExperiment'
src=H/'BaseSystem-26A428-HFS-SecurityLogging-v12.dmg';dst=H/'BaseSystem-26A428-HFS-DisplayBridge-v13.dmg'
expected='6c108ad9eae9cbe5e5b0256ffd25f9ba0322450538739ca6805f0693f5da4ce9'
def sha(p):
 with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def plist(args):
 b=subprocess.check_output(args);return plistlib.loads(b[b.index(b'<?xml'):b.index(b'</plist>')+8])
assert not dst.exists() and sha(src)==expected
binary=R/'NativeDisplayBridge/dell27-display-bridge';subprocess.run(['codesign','--verify','--strict',str(binary)],check=True)
subprocess.run(['/bin/cp','-c',str(src),str(dst)],check=True)
folders=['System/Library/CoreServices','System/Library/LaunchAgents'];saved={};h=OwnedHFS(dst)
try:
 for folder in folders:
  old=h.bsd_info(folder);assert struct.unpack_from('>I',old)[0]==0;saved[folder]=old
  stage=bytearray(old);struct.pack_into('>II',stage,0,os.getuid(),os.getgid());stage[8]&=~8;h.bsd_info(folder,stage)
finally:h.close()
a=plist(['hdiutil','attach','-readwrite','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)]);dev=a['system-entities'][0]['dev-entry'];mount=Path(next(e['mount-point'] for e in a['system-entities'] if 'mount-point' in e))
program='/System/Library/CoreServices/dell27-display-bridge'
agent={'Label':'org.dell27.firmware-display-bridge','ProgramArguments':[program,'--guest-firmware-bridge'],'RunAtLoad':True,'KeepAlive':False,'LimitLoadToSessionType':['LoginWindow','Aqua'],'ProcessType':'Interactive','StandardOutPath':'/dev/console','StandardErrorPath':'/dev/console','EnvironmentVariables':{'ASL_DISABLE':'1'}}
files={program.lstrip('/'):(binary.read_bytes(),0o755),'System/Library/CoreServices/dell27-firmware-bridge.enabled':(b'D27-GUEST-DISPLAY-BRIDGE-V1\n',0o644),'System/Library/LaunchAgents/org.dell27.firmware-display-bridge.plist':(plistlib.dumps(agent),0o644)}
try:
 for name,(data,mode) in files.items():
  p=mount/name;assert not p.exists() and not p.is_symlink();p.write_bytes(data);p.chmod(mode);assert p.read_bytes()==data
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
h=OwnedHFS(dst)
try:
 for name in files:h.owner(name,(0,0));assert h.owner(name)['previousUIDGID']==(0,0)
 for folder,old in saved.items():h.bsd_info(folder,old);assert h.bsd_info(folder)==old
finally:h.close()
a=plist(['hdiutil','attach','-readonly','-nomount','-nobrowse','-imagekey','diskimage-class=CRawDiskImage','-plist',str(dst)]);dev=a['system-entities'][0]['dev-entry']
try:
 check=subprocess.run(['diskutil','verifyVolume',dev],capture_output=True,text=True);assert check.returncode==0,check.stdout+check.stderr
finally:subprocess.run(['hdiutil','detach',dev],check=True,capture_output=True)
old=P/'BootContextExperiment-SecurityLogging-v12';profile=P/'BootContextExperiment-DisplayBridge-v13';assert not profile.exists();profile.mkdir();meta=json.loads((old/'VMAPPLE-REFERENCE-BOOT.json').read_text());capture=profile/'vmapple-reference-boot-state.bin'
assert sha(old/capture.name)==meta['sha256'];subprocess.run(['/bin/cp','-c',str(old/capture.name),str(capture)],check=True)
at=meta['recoveryImagePhysical']-meta['guestBase'];size=meta['recoveryImageBytes'];assert dst.stat().st_size==size and meta['recoveryImageSHA256']==expected
with capture.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);old_hash=hashlib.sha256()
 for off in range(0,size,1024*1024):old_hash.update(im[at+off:at+min(size,off+1024*1024)])
 assert old_hash.hexdigest()==expected
 with dst.open('rb') as inp:
  for off in range(0,size,1024*1024):
   b=inp.read(min(1024*1024,size-off))
   if im[at+off:at+off+len(b)]!=b:im[at+off:at+off+len(b)]=b
 im.flush();digest=hashlib.sha256(im).hexdigest();im.close()
meta.update(parentCaptureSHA256=meta['sha256'],sha256=digest,recoveryImageSHA256=sha(dst),inputRelativePath='EngineProbe/vmapple-'+digest+'.bin',guestDisplayBridge={'agent':agent,'addedFiles':{n:hashlib.sha256(b).hexdigest() for n,(b,m) in files.items()},'nativeRendererVerified':False},graphicalInstallerVerified=False)
head=(old/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0];head+='static const uint8_t vmref_sha256[32]={'+','.join(str(v) for v in bytes.fromhex(digest))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+meta['inputRelativePath'].replace('/','\\'))+'\n'
(profile/'vmapple_reference_boot.h').write_text(head);(profile/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(meta,indent=2)+'\n');(profile/'BOOT-CONTEXT-PREPARATION.json').write_bytes((old/'BOOT-CONTEXT-PREPARATION.json').read_bytes())
assert sha(src)==expected
report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'profile':str(profile),'image':str(dst),'imageSHA256':sha(dst),'inputSHA256':digest,'addedGuestFiles':meta['guestDisplayBridge']['addedFiles'],'originalImageUnchanged':True,'filesystemVerification':check.stdout+check.stderr,'AppleExecutableBytesChanged':False,'hostDisplayChanged':False,'hostSecurityChanged':False,'nativeFramesVerified':False,'graphicalInstallerVerified':False}
(R/'DISPLAY-BRIDGE-PREPARATION.json').write_text(json.dumps(report,indent=2)+'\n');(R/'BootContextExperiment').symlink_to(profile,target_is_directory=True);print(json.dumps(report,indent=2))
