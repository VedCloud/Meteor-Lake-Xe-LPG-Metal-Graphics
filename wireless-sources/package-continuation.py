"""Stage new features separately; never replace the working EFI or replay old publishers."""
from pathlib import Path
import shutil,hashlib,json,subprocess,difflib
r=Path(__file__).resolve().parent.parent;usb=Path('/Volumes/SEQOUIA');u=usb/'iGPU-Port';out=r/'port-artifacts/continuation-20261021';stage=out/'iGPU-Port';stage.mkdir(exist_ok=True)
old=u/'Peripherals/Continuity/Candidate-0.1.0';c=stage/'Peripherals/Continuity/Candidate-0.1.1';assert old.is_dir() and not c.exists()
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def copy(a,b):b.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(a,b)
def tree(a,b):shutil.copytree(a,b,ignore=shutil.ignore_patterns('._*','__pycache__'))
def manifest(folder,name):
 p=folder/name;p.write_text(''.join(sha(f)+'  '+str(f.relative_to(folder))+'\n' for f in sorted(folder.rglob('*')) if f.is_file() and f!=p and not f.name.startswith('._')))
build=r/'desktop-build/airport-awdl-next-objects'
tree(build/'AirportItlwm.kext',c/'AirportItlwm.kext');tree(old/'KnownWorking',c/'KnownWorking');tree(old/'Source',c/'Source')
for name in ['BUILD.json','COMPILE.json']:copy(build/name,c/name)
for name in ['LatitudeAWDLTx.h','test-awdl-tx.cpp','build-private-radio.py','compile-private-path.py','collect-native-integration.sh']:
 copy(r/'wireless-sources'/name,c/'Source'/name)
copy(r/'wireless-sources/trial-awdl-0.1.1.sh',c/'trial-awdl.sh')
copy(old/'CANDIDATE-BUNDLE-SHA256SUMS.txt',c/'PREVIOUS-BUNDLE-SHA256SUMS.txt')
copy(old/'WORKING-BUNDLE-SHA256SUMS.txt',c/'WORKING-BUNDLE-SHA256SUMS.txt')
mods=['AirportItlwm/AirportItlwmV2.cpp','AirportItlwm/AirportItlwmV2.hpp','itlwm/hal_iwx/ItlIwx.cpp','itlwm/hal_iwx/ItlIwx.hpp','itlwm/hal_iwx/if_iwxvar.h','include/LatitudeAWDLTx.h']
patch=[]
for name in mods:
 oldp=r/'desktop-build/airport-awdl'/name;newp=r/'desktop-build/airport-awdl-next'/name
 copy(newp,c/'Source/Modified'/name)
 patch.extend(difflib.unified_diff(oldp.read_text().splitlines(True) if oldp.exists() else [],newp.read_text().splitlines(True),fromfile='a/'+name if oldp.exists() else '/dev/null',tofile='b/'+name))
(c/'Source/0.1.0-to-0.1.1.patch').write_text(''.join(patch))
(c/'README.md').write_text('''# AWDL candidate 0.1.1 — native lifecycle and transmit accounting

Full 74-unit Intel Sequoia compilation, linking and ad-hoc signature verified.
Preserves the exact 0.1.0 Meteor Lake firmware/PNVM and ordinary Wi-Fi fixes.

Adds per-frame cookies before the ring kick; firmware response status is counted
once, independently of queue acceptance and buffer reclamation. Reset aborts
release the raw packet's node reference. These counters do not prove remote
reception; a broadcast success is not a peer acknowledgement.

Virtual-interface overrides delegate to Sequoia's native factories, recording
create/attach/enable/disable/detach outcomes. No old Ventura object is allocated,
no fictitious AWDL capability is advertised and no link is forced active.

AirDrop is unfinished: peer TX/RX, native CHANNEL_IO queues, off-channel timing
and target firmware qualification remain. No automatic RF transmission occurs;
latitude-awdl-current-channel=1 remains absent from the USB configuration.
This candidate is staged, not selected in EFI. The original and Dell-tested
0.1.0 bundle are accepted predecessors; restore returns the original Wi-Fi.
No graphics test suite is requested.

Source/0.1.0-to-0.1.1.patch applies after the archived 0.1.0 patch stack.
''')
manifest(c/'AirportItlwm.kext','../CANDIDATE-BUNDLE-SHA256SUMS.txt')
# The bundle manifest belongs above the bundle; relative entries still name its contents.
manifest(c,'SHA256SUMS.txt')
text=stage/'Desktop/TextRendering'
for name in ['libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64','test-appkit-x86_64','NATIVE-VALIDATION.json']:
 copy(r/'desktop-build/latitude-lcd'/name,text/name)
for f in (r/'subpixel-sources').iterdir():
 if f.is_file():copy(f,text/'Source'/f.name)
for name in ['INTEGRATION-STATUS.md','NATIVE-RUNTIME.md','validate-native-text.sh']:copy(r/'subpixel-sources'/name,text/name)
power=stage/'Peripherals/Power'
copy(r/'power-sources/collect-sleep.sh',power/'collect-sleep.sh')
copy(r/'power-sources/test-sleep-collector.py',power/'Source/test-sleep-collector.py')
copy(r/'power-sources/test-gpu-power-state.py',power/'Source/test-gpu-power-state.py')
copy(r/'NullMoth/gpu-port/xkpi/xnu/nmx_kext.cpp',power/'Source/nmx_kext-power-retry.cpp')
copy(out/'SLEEP-AUDIT.json',power/'SLEEP-AUDIT.json')
copy(r/'wireless-sources/collect-native-integration.sh',stage/'Peripherals/Continuity/collect-native-integration.sh')
copy(r/'desktop-sources/collect-desktop-failure.sh',stage/'collect-desktop-failure.sh')
(stage/'CONTINUATION-20261021.md').write_text('''# Continuation: AWDL, native text formats and sleep

AWDL 0.1.1 compiles/links/signs and is staged separately. AirDrop is not functional.
The active EFI remains the Dell-tested 0.1.0 bundle.

Text adds native BGRA channel-order handling and all RGBA/BGRA transparent-image
combinations. Host ASan/UBSan and release tests pass; Intel artifacts compile/sign.
This is process-local rendering support. System-wide Apple-app and GPU-backed
CoreAnimation integration remain unfinished. No global injection is installed.

Sleep symptom: stays awake. The active DSDT has Name(SS3,Zero) and guarded _S3;
no forced _S3 patch is applied. The GPU callback state-error fix compiles as an
Intel object and passes failure/retry tests, but is not installed. No sleep fix
is claimed. The new read-only before/after collector gathers the live evidence.
The desktop collector also now records sleep capabilities and scheduled wakes.

On the Dell, before trying Apple menu Sleep:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Power/collect-sleep.sh before
Then, if it remains awake or after waking:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Power/collect-sleep.sh after

Native integration evidence, without RF transmission or app injection:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/collect-native-integration.sh
''')
for k in [c/'AirportItlwm.kext',text/'libLatitudeLCDNative-x86_64.dylib']:
 subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
# Verify relative bundle hashes directly (manifest file is one directory above).
for line in (c/'CANDIDATE-BUNDLE-SHA256SUMS.txt').read_text().splitlines():
 h,name=line.split('  ',1);assert sha(c/'AirportItlwm.kext'/name)==h
(out/'STAGED.json').write_text(json.dumps({'activeEFIChanged':False,'AirDropFunctional':False,'FreeTypeSystemWide':False,'sleepFixed':False,'candidateSHA256':sha(c/'AirportItlwm.kext/Contents/MacOS/AirportItlwm'),'textSHA256':sha(text/'libLatitudeLCDNative-x86_64.dylib')},indent=2)+'\n')
print('Staged',stage)
