from pathlib import Path
import sys,plistlib,shutil,json,hashlib,subprocess
r=Path(__file__).resolve().parent.parent;sys.path.insert(0,str(r/'graphics-mode-sources'));from prepare import configure,create_script
sys.path.insert(0,str(r/'copilot-sources'));from retain import retain,REL
u=Path('/Volumes/SEQOUIA');port=u/'iGPU-Port';per=port/'Peripherals';stage=r/'port-artifacts/accelerated-default-tablet-20261021';b=r/'backups/accelerated-default-tablet-20261021'
assert not b.exists() and not stage.exists(),'One-shot publisher: already used'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def inventory(root):return {str(p.relative_to(root)):sha(p) for p in root.rglob('*') if p.is_file() and not p.name.startswith('._')}
manifest=json.loads((port/'SHA256SUMS.json').read_text())
for n,h in manifest.items():assert sha(port/n)==h,n
retain(plistlib.loads((u/'EFI'/REL).read_bytes()),True)
assert (u/'EFI/OC/config.plist').read_bytes()==(per/'Base/config.plist').read_bytes(),'Active config differs from restored base'
previous=inventory(u/'EFI');modes=stage/'GraphicsModes';modes.mkdir(parents=True)
old_hash={};new_hash={}
for profile,source in [('base',per/'Base/config.plist'),('touchpad',per/'Touchpad/config.plist')]:
 d=plistlib.loads(source.read_bytes());old_hash[profile]=sha(source)
 for mode,on in [('accelerated',True),('recovery',False)]:
  p=modes/(profile+'-'+mode+'.plist');p.write_bytes(plistlib.dumps(configure(d,on)))
  subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(p)],check=True,capture_output=True)
 new_hash[profile]=sha(modes/(profile+'-accelerated.plist'))
(stage/'switch-graphics-mode.sh').write_text(create_script(modes,['base','touchpad']))
script=(per/'touchpad-trial.sh').read_text()
for profile in old_hash:assert old_hash[profile] in script;script=script.replace(old_hash[profile],new_hash[profile])
(stage/'touchpad-trial.sh').write_text(script)
b.mkdir(parents=True);shutil.copytree(u/'EFI',b/'EFI');shutil.copytree(per,b/'Peripherals')
for name in ['SHA256SUMS.json','collect-desktop-failure.sh','install-windowserver.sh']:shutil.copy2(port/name,b/name)
ub=port/'backups'/b.name;shutil.copytree(b,ub)
changed=[];absent=[]
def put(source,dest):
 if not dest.exists():absent.append(str(dest.relative_to(port)))
 dest.parent.mkdir(parents=True,exist_ok=True);temp=dest.with_name(dest.name+'.publish-tmp');shutil.copy2(source,temp);temp.replace(dest);changed.append(dest)
try:
 for p in modes.iterdir():put(p,port/'GraphicsModes'/p.name)
 put(stage/'switch-graphics-mode.sh',port/'switch-graphics-mode.sh')
 for p in (r/'graphics-mode-sources').iterdir():
  if p.is_file() and p.name!='publish.py':put(p,port/'GraphicsModes/Source'/p.name)
 put(r/'graphics-mode-sources/README.md',port/'GraphicsModes/README.md')
 for dest,profile in [(per/'Base/config.plist','base'),(per/'Touchpad/config.plist','touchpad'),(per/'config.plist','base')]:put(modes/(profile+'-accelerated.plist'),dest)
 put(stage/'touchpad-trial.sh',per/'touchpad-trial.sh')
 lines=[]
 for line in (per/'TRIAL-SHA256SUMS.txt').read_text().splitlines():
  _,n=line.split('  ',1);lines.append(sha(per/n)+'  '+n)
 temp=stage/'TRIAL-SHA256SUMS.txt';temp.write_text('\n'.join(lines)+'\n');put(temp,per/'TRIAL-SHA256SUMS.txt')
 for p in (r/'tablet-sources').iterdir():
  if p.is_file() and p.suffix in ['.sh','.py','.m','.md']:put(p,per/'Tablet'/p.name)
 put(r/'desktop-build/latitude-tablet-state',per/'Tablet/latitude-tablet-state')
 put(r/'desktop-sources/collect-desktop-failure.sh',port/'collect-desktop-failure.sh')
 put(r/'graphics-mode-sources/install-windowserver-wrapper.sh',port/'install-windowserver.sh')
 for n in ['prepare.py','retain.py','test-macros.py']:put(r/'copilot-sources'/n,per/'CopilotCommand'/n)
 put(r/'port-artifacts/copilot-command/CONFIRMED.json',per/'CopilotCommand/CONFIRMED.json')
 doc=stage/'COPILOT.md';doc.write_text('''# Confirmed Copilot → Command
User confirmed working on the Dell on 21 October 2026. Active USB driver translation is retained; no userspace remapper. Preserve this Info.plist Macro Inversion in all future EFI builds. Canonical local retention tool: copilot-sources/retain.py; source copies supplied here for review. Historical backups remain historical. No reinstall needed.
''');put(doc,per/'CopilotCommand/README.md')
 put(modes/'base-accelerated.plist',u/'EFI/OC/config.plist') if False else None
 # Commit active USB boot policy last; package hash guard is already prepared.
 target=u/'EFI/OC/config.plist';tmp=target.with_name('config.plist.graphics-tmp');shutil.copy2(modes/'base-accelerated.plist',tmp);tmp.replace(target)
 for p in changed:manifest[str(p.relative_to(port))]=sha(p)
 (port/'SHA256SUMS.json').write_text(json.dumps(manifest,indent=2)+'\n')
 after=inventory(u/'EFI');assert set(previous)==set(after);assert [n for n in after if previous[n]!=after[n]]==['OC/config.plist']
 retain(plistlib.loads((u/'EFI'/REL).read_bytes()),True)
 for line in (port/'Desktop/WINDOWSERVER-BASELINE-SHA256SUMS.txt').read_text().splitlines():h,n=line.split(None,1);assert sha(port/n.strip())==h
 for n,h in manifest.items():assert sha(port/n)==h,n
 subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(target)],check=True)
 subprocess.run(['codesign','--verify','--strict',str(per/'Tablet/latitude-tablet-state')],check=True)
 for p in [port/'switch-graphics-mode.sh',port/'install-windowserver.sh',port/'collect-desktop-failure.sh',per/'touchpad-trial.sh',per/'Tablet/collect-tablet-macos.sh']:subprocess.run(['/bin/bash','-n',str(p)],check=True)
except BaseException:
 shutil.copy2(b/'EFI/OC/config.plist',u/'EFI/OC/config.plist')
 # All overwritten peripheral files have been preserved in the full peripheral backup.
 for p in changed:
  rel=p.relative_to(port);src=b/rel
  if src.is_file():shutil.copy2(src,p)
  elif str(rel) in absent:p.unlink(missing_ok=True)
 shutil.copy2(b/'SHA256SUMS.json',port/'SHA256SUMS.json')
 raise
validation={'normal_usb_mode':'accelerated-every-boot','efi_only_changed':'OC/config.plist','graphics_binaries_changed':False,'copilot_user_confirmed':True,'copilot_retained':True,'tablet_auto_features_enabled':False,'tablet_blocker':'No verified macOS ISH transport, sensor orientation samples or mode signal in supplied reports','updated_files':len(changed),'boot_validation_pending':True}
(stage/'VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n')
print('Published persistent accelerated USB mode, explicit recovery switch, confirmed Copilot preservation, corrected tablet diagnostics. Package hashes and frozen graphics baseline verified. Full EFI backup:',ub)
