from pathlib import Path
import tempfile,shutil,subprocess,hashlib
r=Path(__file__).resolve().parent.parent;source=r/'port-artifacts/continuation-20261021/iGPU-Port/Peripherals/Continuity/Candidate-0.1.1'
old=Path('/Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/Candidate-0.1.0/AirportItlwm.kext')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
with tempfile.TemporaryDirectory() as tmp:
 u=Path(tmp);c=u/'iGPU-Port/Peripherals/Continuity/Candidate-0.1.1';shutil.copytree(source,c)
 (u/'EFI/OC').mkdir(parents=True);(u/'EFI/OC/config.plist').write_text('fixture');target=u/'EFI/OC/Kexts/AirportItlwm.kext';target.parent.mkdir();shutil.copytree(old,target)
 script=c/'trial-awdl.sh'
 for action in ['apply','apply','restore','restore','apply','restore']:
  subprocess.run(['/bin/bash',str(script),action],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
  expected=c/('AirportItlwm.kext' if action=='apply' else 'KnownWorking/AirportItlwm.kext')/'Contents/MacOS/AirportItlwm'
  assert sha(target/'Contents/MacOS/AirportItlwm')==sha(expected)
 # Unknown active driver is never replaced.
 b=target/'Contents/MacOS/AirportItlwm';b.write_bytes(b'unknown');before=sha(b)
 assert subprocess.run(['/bin/bash',str(script),'apply'],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode==2
 assert sha(b)==before and not Path(str(target)+'.awdl-staged').exists()
 assert (u/'EFI/OC/config.plist').read_text()=='fixture'
print('PASS actual trial: Dell-tested 0.1.0 → 0.1.1, idempotence, original restore, unknown-driver refusal, EFI config preservation. Temporary fixture only.')
