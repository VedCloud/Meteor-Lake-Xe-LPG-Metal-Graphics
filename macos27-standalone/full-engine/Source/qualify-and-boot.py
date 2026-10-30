"""One-off owned comparison run; no recurring job or host setting changes."""
from pathlib import Path
import subprocess,os,time,json,datetime,hashlib
r=Path(__file__).resolve().parent;python='/Volumes/Dell27Build/deps/bin/python3'
state={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'PID':os.getpid(),'processGroup':os.getpgid(0),'EFI_SHA256':json.loads((r/'BUILD.json').read_text())['EFI_SHA256'],'graphicalInstallerVerified':False}
def save(phase,**fields):
 state.update(phase=phase,updatedUTC=datetime.datetime.now(datetime.timezone.utc).isoformat(),**fields)
 tmp=r/'RUNNER-STATE.tmp';tmp.write_text(json.dumps(state,indent=2)+'\n');tmp.replace(r/'RUNNER-STATE.json');print(phase,flush=True)
def launch(args,log):
 with (r/log).open('wb') as out:return subprocess.Popen(args,stdin=subprocess.DEVNULL,stdout=out,stderr=subprocess.STDOUT,env={**os.environ,'DELL27_DYLD_CACHE_DIR':'/Volumes/Dell27Recovery/System/Library/dyld'})
flags=['--exfat','--installer','--framebuffer','--storage','--hfs','--boot-context','--formatted-aux','--outer-icount']
children=[]
try:
 save('checking-modified-capture-rejection')
 n=launch([python,str(r/'test-sparse-firmware.py'),*flags,'--tampered-only'],'ADMISSION-RUNNER.log');save('admission-test-running',admissionRunnerPID=n.pid)
 assert n.wait()==0,'Admission test failed; inspect ADMISSION-RUNNER.log'
 proof=json.loads((r/'ADMISSION-VALIDATION.json').read_text());assert proof['EFI_SHA256']==state['EFI_SHA256'] and proof['OVMFCases']=={'vm-tampered':'PASS'}
 save('admission-passed-starting-positive-boot',admissionRun=proof['runDirectory'])
 p=launch([python,str(r/'test-sparse-firmware.py'),*flags],'POSITIVE-RUNNER.log');save('positive-boot-running',positiveRunnerPID=p.pid)
 seen_run=None;slide_attempt=0;resolver=None
 while p.poll() is None:
  a_path=r/'ACTIVE-valid.json'
  if a_path.exists():
   a=json.loads(a_path.read_text());assert a['EFI_SHA256']==state['EFI_SHA256']
   run=Path(a['run']);text=Path(a['log']).read_text(errors='replace') if Path(a['log']).exists() else ''
   if seen_run is None:
    seen_run=str(run);save('positive-boot-running',positiveRun=seen_run)
   assert seen_run==str(run),'Owned positive run changed unexpectedly'
   if 'Verified complete initialized memory capture; beginning sparse-RAM VMAPPLE kernel replay' in text and not children:
    for name in ['watch-service-events.py','watch-graphical-startup.py']:children.append(launch([python,str(r/name)],name+'.log'))
    save('admitted-kernel-replay-running',observerPIDs=[x.pid for x in children])
   if 'Darwin Bootstrapper Version' in text and not (run/'DYLD-SLIDE.json').exists() and time.monotonic()>=slide_attempt and (resolver is None or resolver.poll() is not None):
    resolver=launch([python,str(r/'determine-live-dyld-slide.py')],'DYLD-RESOLVER.log');slide_attempt=time.monotonic()+300
   if (run/'DYLD-SLIDE.json').exists() and not state.get('dyldSlideVerified'):save('native-boot-observation-running',dyldSlideVerified=True)
  time.sleep(10)
 save('positive-test-ended',positiveRunnerReturnCode=p.returncode)
finally:
 for c in [*children,resolver] if 'resolver' in locals() else children:
  if c is not None and c.poll() is None:
   c.terminate()
   try:c.wait(timeout=5)
   except subprocess.TimeoutExpired:c.kill();c.wait()
