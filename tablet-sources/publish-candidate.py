"""One-shot private candidate publisher; not an installer to run on the Dell."""
from pathlib import Path
import sys,hashlib,json,plistlib,shutil,subprocess
root=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');port=usb/'iGPU-Port';per=port/'Peripherals'
sys.path.insert(0,str(root/'graphics-mode-sources'));from prepare import configure,create_script
stage=root/'port-artifacts/tablet-input-candidate-20261021';backup=root/'backups/tablet-input-candidate-20261021'
assert not stage.exists() and not backup.exists(),'One-shot publisher already used'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inv(p):return {str(f.relative_to(p)):sha(f) for f in p.rglob('*') if f.is_file() and not f.name.startswith('._')}
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for name,value in manifest.items():assert sha(port/name)==value,name
assert (usb/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes(),'Active EFI is not the restored base'
previous=inv(usb/'EFI');modes=stage/'GraphicsModes';modes.mkdir(parents=True)
old={};new={}
for profile,src in [('base',per/'Base/config.plist'),('touchpad',per/'Touchpad/config.plist')]:
 old[profile]=sha(src);d=plistlib.loads(src.read_bytes());assert not any(e['BundlePath']=='LatitudeTabletObserver.kext' for e in d['Kernel']['Add'])
 entry=dict(next(e for e in d['Kernel']['Add'] if e['BundlePath']=='LatitudeHotkeys.kext'));entry.update(BundlePath='LatitudeTabletObserver.kext',ExecutablePath='Contents/MacOS/LatitudeTabletObserver',Comment='Passive tablet CC/CD observer 0.1.0; no input or display control')
 d['Kernel']['Add'].append(entry)
 for mode,on in [('accelerated',True),('recovery',False)]:
  doc=configure(d,on);doc['Kernel']['Add'][-1]['Enabled']=on
  dest=modes/(profile+'-'+mode+'.plist');dest.write_bytes(plistlib.dumps(doc));subprocess.run([str(root/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(dest)],capture_output=True,check=True)
 new[profile]=sha(modes/(profile+'-accelerated.plist'))
script=(per/'touchpad-trial.sh').read_text()
for profile in old:assert old[profile] in script;script=script.replace(old[profile],new[profile])
(stage/'touchpad-trial.sh').write_text(script);(stage/'switch-graphics-mode.sh').write_text(create_script(modes,['base','touchpad']))
backup.mkdir();shutil.copytree(usb/'EFI',backup/'EFI');shutil.copytree(per,backup/'Peripherals')
for name in ['SHA256SUMS.json','collect-desktop-failure.sh','switch-graphics-mode.sh']:shutil.copy2(port/name,backup/name)
shutil.copytree(port/'GraphicsModes',backup/'GraphicsModes')
changed=[];newfiles=[]
def put(src,dest):
 if not dest.exists():newfiles.append(dest)
 dest.parent.mkdir(parents=True,exist_ok=True);tmp=dest.with_name(dest.name+'.candidate-tmp');shutil.copyfile(src,tmp);tmp.replace(dest);changed.append(dest)
def tree(src,dest):
 for p in src.rglob('*'):
  if p.is_file() and not p.name.startswith('._'):put(p,dest/p.relative_to(src))
try:
 for src in modes.iterdir():put(src,port/'GraphicsModes'/src.name)
 put(stage/'switch-graphics-mode.sh',port/'switch-graphics-mode.sh');put(stage/'touchpad-trial.sh',per/'touchpad-trial.sh')
 for profile,dest in [('base',per/'Base/config.plist'),('base',per/'config.plist'),('touchpad',per/'Touchpad/config.plist')]:put(modes/(profile+'-accelerated.plist'),dest)
 tree(root/'desktop-build/gpio-mtl/LatitudeGPIO.kext',per/'Touchpad/LatitudeGPIO.kext')
 # GPIO diagnostics are in the optional trial only; normal base keeps it disabled.
 tree(root/'desktop-build/tablet-observer/LatitudeTabletObserver.kext',usb/'EFI/OC/Kexts/LatitudeTabletObserver.kext')
 for p in (root/'tablet-sources').iterdir():
  if p.is_file() and p.name!='publish-candidate.py':put(p,per/'Tablet'/p.name)
 put(root/'desktop-build/latitude-tablet-state',per/'Tablet/latitude-tablet-state')
 tree(root/'desktop-build/Latitude Tablet Keyboard.app',per/'Tablet/Latitude Tablet Keyboard.app')
 tree(root/'port-artifacts/tablet-policy',per/'Tablet/Candidate/Policy')
 tree(root/'desktop-build/tablet-observer/LatitudeTabletObserver.kext',per/'Tablet/Candidate/LatitudeTabletObserver.kext')
 put(root/'desktop-build/tablet-observer/BUILD.json',per/'Tablet/Candidate/OBSERVER-BUILD.json')
 for name in ['LatitudeGPIO.cpp','latitude_gpio_map.h','build.py','test-driver.py','test-map.py']:put(root/'gpio-sources'/name,per/'Tablet/Candidate/GPIO-Source'/name)
 put(root/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
 lines=[]
 for line in (per/'TRIAL-SHA256SUMS.txt').read_text().splitlines():
  _,name=line.split('  ',1);lines.append(sha(per/name)+'  '+name)
 temp=stage/'TRIAL-SHA256SUMS.txt';temp.write_text('\n'.join(lines)+'\n');put(temp,per/'TRIAL-SHA256SUMS.txt')
 put(modes/'base-accelerated.plist',usb/'EFI/OC/config.plist')
 # Assert exact EFI scope, including the confirmed keyboard and stable GPU.
 after=inv(usb/'EFI');changedEFI=[n for n in previous if previous[n]!=after[n]];addedEFI=sorted(set(after)-set(previous))
 assert changedEFI==['OC/config.plist'],changedEFI
 assert all(n.startswith('OC/Kexts/LatitudeTabletObserver.kext/') for n in addedEFI)
 subprocess.run([sys.executable,str(root/'copilot-sources/retain.py'),str(usb/'EFI'),'--check'],check=True,capture_output=True)
 for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():h,n=line.split(None,1);assert sha(port/n.strip())==h
 status={'candidate':'tablet-input-observation-20261021','normalGraphics':'accelerated-every-boot','copilotMappingPreserved':True,'workingGraphicsBinariesChanged':False,'passiveObserverEnabledOnNormalBoot':True,'observerHardwareBootValidated':False,'modeActuationEnabled':False,'automaticRotationEnabled':False,'automaticKeyboardPopupEnabled':False,'keyboardManualPanelCompiled':True,'onScreenKeyboardIntentImplemented':True,'ISHTransportStatus':'Bounded packet framing/XYZ decoding implemented offline; PCI firmware/IPC/HBM/interrupt/DMA runtime not implemented','touchTransformStatus':'Normalized affine transform tested; existing Wacom rotation path identified; coordinated framebuffer transaction not implemented','capturedTabletSamplesReplayed':50,'capturedTouchpadFrames':4007,'touchpadTrialEnabledInNormalProfile':False,'optionalGPIOVersion':'0.1.1 diagnostic instrumentation; same pin and register policy','backup':str(backup),'modifiedEFI':changedEFI,'addedEFI':addedEFI}
 f=stage/'STATUS.json';f.write_text(json.dumps(status,indent=2)+'\n');put(f,per/'Tablet/Candidate/STATUS.json')
 for p in changed:
  if p.is_relative_to(port):manifest[str(p.relative_to(port))]=sha(p)
 (port/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n')
 for n,h in manifest.items():assert sha(port/n)==h,n
 for target in [usb/'EFI/OC/config.plist',*[p for p in (port/'GraphicsModes').glob('*.plist') if not p.name.startswith('._')]]:subprocess.run([str(root/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(target)],capture_output=True,check=True)
 for target in [usb/'EFI/OC/Kexts/LatitudeTabletObserver.kext',per/'Tablet/Latitude Tablet Keyboard.app',per/'Tablet/latitude-tablet-state',per/'Touchpad/LatitudeGPIO.kext']:subprocess.run(['codesign','--verify','--strict',str(target)],check=True)
except BaseException:
 shutil.copy2(backup/'EFI/OC/config.plist',usb/'EFI/OC/config.plist')
 if (usb/'EFI/OC/Kexts/LatitudeTabletObserver.kext').exists():shutil.rmtree(usb/'EFI/OC/Kexts/LatitudeTabletObserver.kext',onerror=lambda function,path,error: None if isinstance(error[1],FileNotFoundError) else (_ for _ in ()).throw(error[1]))
 for p in reversed(changed):
  if not p.is_relative_to(port):continue
  src=backup/p.relative_to(port)
  if src.is_file():shutil.copyfile(src,p)
  elif p in newfiles:p.unlink(missing_ok=True)
 shutil.copyfile(backup/'SHA256SUMS.json',port/'SHA256SUMS.json');raise
print('Published candidate. Stable graphics/Copilot retained; normal boot observes tablet events only. Backup:',backup)
