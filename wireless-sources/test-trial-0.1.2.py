from pathlib import Path
import tempfile,shutil,subprocess,hashlib
r=Path(__file__).resolve().parent.parent;csrc=r/'port-artifacts/awdl-radio-0.1.2-20261021/iGPU-Port/Peripherals/Continuity/Candidate-0.1.2';u=Path('/Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity')
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);c=root/'iGPU-Port/Peripherals/Continuity/Candidate-0.1.2';shutil.copytree(csrc,c)
 target=root/'EFI/OC/Kexts/AirportItlwm.kext';target.parent.mkdir(parents=True);config=root/'EFI/OC/config.plist';config.write_text('unchanged-fixture')
 for predecessor in [c/'KnownWorking/AirportItlwm.kext',u/'Candidate-0.1.0/AirportItlwm.kext',u/'Candidate-0.1.1/AirportItlwm.kext']:
  if target.exists():shutil.rmtree(target)
  shutil.copytree(predecessor,target)
  for action in ['apply','apply','restore','restore']:
   subprocess.run(['/bin/bash',str(c/'trial-awdl.sh'),action],check=True,capture_output=True)
   expected=c/('AirportItlwm.kext' if action=='apply' else 'KnownWorking/AirportItlwm.kext')/'Contents/MacOS/AirportItlwm'
   assert sha(target/'Contents/MacOS/AirportItlwm')==sha(expected)
 # Simulate a failing signature preflight: staging must be cleaned, active retained.
 script=c/'trial-awdl.sh';stub=root/'codesign-fail';stub.write_text('#!/bin/sh\nexit 7\n');stub.chmod(0o755)
 script.write_text(script.read_text().replace('/usr/bin/codesign',str(stub)))
 sums=c/'SHA256SUMS.txt';lines=sums.read_text().splitlines();sums.write_text('\n'.join((sha(script)+'  trial-awdl.sh') if line.endswith('  trial-awdl.sh') else line for line in lines)+'\n')
 before=sha(target/'Contents/MacOS/AirportItlwm');result=subprocess.run(['/bin/bash',str(script),'apply'],capture_output=True);assert result.returncode==7,result.stderr
 assert before==sha(target/'Contents/MacOS/AirportItlwm') and not Path(str(target)+'.awdl-staged').exists()
 # Unknown installed driver never overwritten.
 (target/'Contents/MacOS/AirportItlwm').write_bytes(b'unknown')
 assert subprocess.run(['/bin/bash',str(script),'apply'],capture_output=True).returncode==2
 assert (target/'Contents/MacOS/AirportItlwm').read_bytes()==b'unknown' and config.read_text()=='unchanged-fixture'
print('PASS real 0.1.2 trial: all three predecessors, idempotence, original restore, signature-failure cleanup and unknown-driver refusal; fixture EFI configuration unchanged.')
