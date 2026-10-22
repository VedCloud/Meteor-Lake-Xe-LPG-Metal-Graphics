"""Assemble private radio code and tests; leaves the active USB driver alone."""
from pathlib import Path
import json,hashlib,shutil,subprocess,difflib
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA/iGPU-Port');out=r/'port-artifacts/awdl-radio-0.1.2-20261021';c=out/'iGPU-Port/Peripherals/Continuity/Candidate-0.1.2';assert not c.exists();c.mkdir(parents=True)
b=r/'desktop-build/airport-awdl-radio-objects';helper=r/'desktop-build/awdl-radio-helper';prev=u/'Peripherals/Continuity/Candidate-0.1.0';prior=u/'Peripherals/Continuity/Candidate-0.1.1'
def cp(a,b):b.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(a,b)
def tree(a,b):shutil.copytree(a,b,ignore=shutil.ignore_patterns('._*','__pycache__'))
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def sums(folder,p):p.write_text(''.join(sha(f)+'  '+str(f.relative_to(folder))+'\n' for f in sorted(folder.rglob('*')) if f.is_file() and f!=p and not f.name.startswith('._')))
tree(b/'AirportItlwm.kext',c/'AirportItlwm.kext');tree(prev/'KnownWorking',c/'KnownWorking');tree(prev/'AirportItlwm.kext',c/'DellTested-0.1.0/AirportItlwm.kext')
for name in ['BUILD.json']:cp(b/name,c/name)
cp(helper/'latitude-awdl-radio-x86_64',c/'latitude-awdl-radio-x86_64');cp(helper/'VALIDATION.json',c/'RADIO-VALIDATION.json')
cp(r/'wireless-sources/trial-awdl-0.1.2.sh',c/'trial-awdl.sh')
cp(prev/'CANDIDATE-BUNDLE-SHA256SUMS.txt',c/'PREVIOUS-BUNDLE-SHA256SUMS.txt');cp(prior/'CANDIDATE-BUNDLE-SHA256SUMS.txt',c/'PRIOR-0.1.1-BUNDLE-SHA256SUMS.txt');cp(prev/'WORKING-BUNDLE-SHA256SUMS.txt',c/'WORKING-BUNDLE-SHA256SUMS.txt')
source=c/'Source';tree(prior/'Source',source)
for p in (r/'wireless-sources').iterdir():
 if p.is_file() and p.suffix in ['.h','.inc','.cpp','.py','.sh','.md']:cp(p,source/p.name)
cp(r/'wireless-sources/trial-awdl-0.1.2.sh',source/'trial-awdl.sh')
engine=r/'desktop-build/awdl-radio-engine-source'
for name in ['src','radiotap']:tree(engine/name,source/'OWL'/name)
for name in ['COPYING','PROVENANCE.json','radio-wire-alignment.patch']:cp(engine/name,source/'OWL'/name)
mods=['AirportItlwm/AirportItlwmV2.cpp','AirportItlwm/AirportItlwmV2.hpp','AirportItlwm/LatitudeAWDLUserClient.inc','itlwm/hal_iwx/ItlIwx.cpp','itlwm/hal_iwx/ItlIwx.hpp','include/LatitudeAWDLRadio.h']
patch=[]
for name in mods:
 new=r/'desktop-build/airport-awdl-radio'/name;old=r/'desktop-build/airport-awdl-next'/name;cp(new,source/'Modified'/name)
 patch.extend(difflib.unified_diff(old.read_text().splitlines(True) if old.exists() else [],new.read_text().splitlines(True),fromfile='a/'+name if old.exists() else '/dev/null',tofile='b/'+name))
(source/'0.1.1-to-0.1.2.patch').write_text(''.join(patch))
(c/'README.md').write_text('''# AWDL current-channel radio bridge 0.1.2

Real driver connections implemented:
- Root-only IOKit user client: hardware/status query, bounded action-frame receive
  dequeue and gated action submission to ItlIwx::iwx_tx. Ordinary Apple Wi-Fi
  user-client types continue through the native superclass.
- Copy matching AWDL vendor action frames from the existing Intel RX path into
  a 16-frame FIFO, with payload validation, overflow accounting and reset cleanup.
  No firmware RX filter changes, channel changes or ordinary packet consumption.
- The userspace tool links the real OWL action encoder/receiver: PSF/MIF creation,
  bounded peer handling and current-channel state. TX uses the existing firmware
  status/cookie/reclaim/reset ledger, rather than treating enqueue as completion.
- Stop closes the API before draining the controller gate and detaching the HAL.

Full Intel kernel compilation/link/sign and Intel userspace compile/sign pass.
Strict ASan/UBSan tests cover the actual user-client body and protocol boundary.
OWL's unaligned wire accesses and overflow-prone offset checks were corrected in
an archived private overlay. Upstream checkout and license notices are retained.

Hardware transmission and passive RX delivery are not validated. The firmware
may still filter frames from the AWDL BSSID; this build does not pretend otherwise.
The userspace poller is a transport qualification tool, not a native Finder service.
AirDrop remains unfinished: off-channel scheduling, native Skywalk peer data,
IPv6/mDNS and a completed transfer with another Apple device remain required.
No awdl0/llw0 interface is manufactured or capability falsely advertised.

Usage after rebooting with this candidate:
sudo ./latitude-awdl-radio-x86_64 status
sudo ./latitude-awdl-radio-x86_64 listen 30

These two commands do not transmit. A one-frame send-psf/send-mif command requires
both --transmit-current-channel and the separate latitude-awdl-current-channel=1
boot gate. The package/installer do not enable this gate or automatically send.
RF results cannot be inferred from successful compilation or protocol unit tests.

trial-awdl.sh apply selects this USB Wi-Fi bundle; restore selects the original.
A copy of the Dell-tested 0.1.0 bundle is also retained. Graphics, keyboard,
OpenCore configuration and the validated 26,000-frame GPU suite are unchanged.
''')
sums(c/'AirportItlwm.kext',c/'CANDIDATE-BUNDLE-SHA256SUMS.txt');sums(c,c/'SHA256SUMS.txt')
cp(r/'wireless-sources/collect-continuity.sh',out/'iGPU-Port/Peripherals/Continuity/collect-continuity.sh')
cp(r/'wireless-sources/collect-native-integration.sh',out/'iGPU-Port/Peripherals/Continuity/collect-native-integration.sh')
for p in [c/'AirportItlwm.kext',c/'latitude-awdl-radio-x86_64']:subprocess.run(['codesign','--verify','--strict',str(p)],check=True)
subprocess.run(['/usr/bin/shasum','-a','256','-c','SHA256SUMS.txt'],cwd=c,stdout=subprocess.DEVNULL,check=True)
(out/'STAGED.json').write_text(json.dumps({'candidate':'0.1.2','driverUUID':json.loads((b/'BUILD.json').read_text())['uuid'],'AirDropFunctional':False,'radioAPIImplemented':True,'DellRadioValidated':False,'hostRadioOpened':False},indent=2)+'\n')
print('Staged',c)
