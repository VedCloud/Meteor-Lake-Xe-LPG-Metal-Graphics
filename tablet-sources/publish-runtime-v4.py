"""One-shot publication: read-only ISH preflight and process-local text test."""
from pathlib import Path
import sys,shutil,hashlib,json,plistlib,subprocess,datetime
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port';per=port/'Peripherals';stage=root/'port-artifacts/unified-runtime-v4-20261021'
stage.mkdir(exist_ok=True);assert not (stage/'PUBLISHED.json').exists(),'Already published; do not replay'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def files(p):return [f for f in p.rglob('*') if f.is_file() and not f.name.startswith('._') and '__pycache__' not in f.parts]
def run(args):subprocess.run([str(x) for x in args],check=True,capture_output=True)
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in manifest.items():assert sha(port/n)==h,n
assert (usb/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes()
writes={}
def put(src,dst):assert src.is_file(),src;writes[dst]=src
def tree(src,dst):
 for f in files(src):put(f,dst/f.relative_to(src))
def generated(name,text,dst):
 p=stage/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text);put(p,dst)
old={name:sha(per/name/'config.plist') for name in ['Base','Touchpad']}
for profile in ['base','touchpad']:
 for mode in ['accelerated','recovery']:
  name=profile+'-'+mode+'.plist';d=plistlib.loads((port/'GraphicsModes'/name).read_bytes())
  assert not any(e['BundlePath']=='LatitudeISHTransport.kext' for e in d['Kernel']['Add'])
  d['Kernel']['Add'].append({'Arch':'x86_64','BundlePath':'LatitudeISHTransport.kext','Comment':'Latitude ISH 0.1.1 read-only preflight; IPC boot gate off','Enabled':mode=='accelerated','ExecutablePath':'Contents/MacOS/LatitudeISHTransport','MaxKernel':'','MinKernel':'24.0.0','PlistPath':'Contents/Info.plist'})
  args=d['NVRAM']['Add']['7C436110-AB2A-4BBB-A880-FE41995C9F82'].get('boot-args','');assert 'latitude-ish-pio' not in args
  p=stage/name;p.write_bytes(plistlib.dumps(d));put(p,port/'GraphicsModes'/name)
  if mode=='accelerated':put(p,per/profile.title()/'config.plist')
put(stage/'base-accelerated.plist',per/'config.plist');put(stage/'base-accelerated.plist',usb/'EFI/OC/config.plist')
sys.path.insert(0,str(root/'graphics-mode-sources'));from prepare import create_script
generated('switch-graphics-mode.sh',create_script(stage,['base','touchpad']),port/'switch-graphics-mode.sh')
trial=(per/'touchpad-trial.sh').read_text()
for profile in ['Base','Touchpad']:
 assert old[profile] in trial;trial=trial.replace(old[profile],sha(stage/(profile.lower()+'-accelerated.plist')))
generated('touchpad-trial.sh',trial,per/'touchpad-trial.sh')
tree(root/'desktop-build/ish-transport/LatitudeISHTransport.kext',usb/'EFI/OC/Kexts/LatitudeISHTransport.kext')
for f in (root/'tablet-sources').iterdir():
 if f.is_file() and not f.name.startswith(('publish-','._')):put(f,per/'Tablet'/f.name)
put(root/'desktop-build/latitude-tablet-state',per/'Tablet/latitude-tablet-state')
put(root/'desktop-build/ish-transport/BUILD.json',per/'Tablet/Candidate/ISH-BUILD.json')
for f in (root/'subpixel-sources').iterdir():
 if f.is_file():put(f,port/'Desktop/TextRendering/Source'/f.name)
native=port/'Desktop/TextRendering'
for name in ['libLatitudeLCD-x86_64.a','libLatitudeLCDCoreText-x86_64.dylib','VALIDATION.json','libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64','NATIVE-VALIDATION.json']:put(root/'desktop-build/latitude-lcd'/name,native/name)
put(root/'subpixel-sources/validate-native-text.sh',native/'validate-native-text.sh');put(root/'subpixel-sources/NATIVE-RUNTIME.md',native/'NATIVE-RUNTIME.md')
generated('NATIVE-SHA256SUMS.txt',''.join(sha(root/'desktop-build/latitude-lcd'/name)+'  '+name+'\n' for name in ['libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64']),native/'NATIVE-SHA256SUMS.txt')
put(root/'wireless-sources/AWDL-AUDIT.md',per/'Continuity/AWDL-AUDIT.md');put(root/'UNIFIED-CANDIDATE-WORK.md',port/'UNIFIED-CANDIDATE-WORK.md')
status=json.loads((port/'UNIFIED-STATUS.json').read_text());status.update({'candidate':'unified-runtime-20261021-v4','ISHPCITransportImplemented':True,'ISHDefaultMode':'read-only preflight','ISHHardwareValidated':False,'ISHInterruptToGravityMockTest':'passed ASan/UBSan on actual driver source','NativeCTLineDrawConnection':'process-local opaque CPU bitmap route tested; Intel execution pending','FreeTypeSystemAppsIntegrated':False,'AWDLRadioPathConnected':False,'automaticRotationEnabled':False,'graphicsBinariesChanged':False})
generated('STATUS.json',json.dumps(status,indent=2)+'\n',port/'UNIFIED-STATUS.json');put(stage/'STATUS.json',per/'Tablet/Candidate/STATUS.json')
validate=root/'research/OpenCore-release/matching/Utilities/ocvalidate/ocvalidate'
for p in stage.glob('*.plist'):run([validate,p])
backup=root/'backups'/('runtime-v4-before-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
shutil.copytree(usb/'EFI',backup/'EFI',ignore=shutil.ignore_patterns('._*'))
journal={}
for dst in [*writes,per/'TRIAL-SHA256SUMS.txt',port/'SHA256SUMS.json']:
 save=backup/'journal'/dst.relative_to(usb)
 if dst.exists():save.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(dst,save);journal[dst]=save
 else:journal[dst]=None
def atomic(src,dst):
 dst.parent.mkdir(parents=True,exist_ok=True);tmp=dst.with_name(dst.name+'.runtime-tmp');shutil.copyfile(src,tmp);tmp.replace(dst)
active=usb/'EFI/OC/config.plist'
try:
 for dst,src in writes.items():
  if dst!=active:atomic(src,dst)
 lines=[]
 for line in (per/'TRIAL-SHA256SUMS.txt').read_text().splitlines():_,n=line.split('  ',1);lines.append(sha(per/n)+'  '+n)
 generated('TRIAL-SHA256SUMS.txt','\n'.join(lines)+'\n',per/'TRIAL-SHA256SUMS.txt');atomic(stage/'TRIAL-SHA256SUMS.txt',per/'TRIAL-SHA256SUMS.txt')
 for dst in [*writes]:
  if dst.is_relative_to(port):manifest[str(dst.relative_to(port))]=sha(dst)
 (stage/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n');atomic(stage/'SHA256SUMS.json',port/'SHA256SUMS.json')
 for n,h in manifest.items():assert sha(port/n)==h,n
 run(['codesign','--verify','--strict',usb/'EFI/OC/Kexts/LatitudeISHTransport.kext'])
 run([sys.executable,root/'copilot-sources/retain.py',usb/'EFI','--check'])
 for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():h,n=line.split(None,1);assert sha(port/n.strip())==h,n
 atomic(writes[active],active);run([validate,active]);run([sys.executable,root/'tablet-sources/validate-candidate.py'])
 # Assert every pre-existing EFI payload outside config is byte-identical.
 for f in files(backup/'EFI'):
  n=f.relative_to(backup/'EFI')
  if str(n)!='OC/config.plist':assert sha(f)==sha(usb/'EFI'/n),n
 (stage/'PUBLISHED.json').write_text(json.dumps({'backup':str(backup),'filesPackaged':len(writes),'DellRuntimeValidated':False,'normalBoot':'accelerated with read-only ISH preflight','validation':'matching ocvalidate, actual switch/trial fixtures, manifest, signatures, Copilot and frozen graphics passed'},indent=2)+'\n')
 print('Published v4; backup:',backup)
except BaseException:
 for dst,saved in reversed(list(journal.items())):
  if saved:atomic(saved,dst)
  else:dst.unlink(missing_ok=True)
 print('Publication rolled back.',file=sys.stderr);raise
