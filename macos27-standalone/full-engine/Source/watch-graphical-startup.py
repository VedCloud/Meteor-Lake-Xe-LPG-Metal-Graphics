"""Bounded, read-only observation of this one owned boot; never infers UI success."""
from pathlib import Path
import json,subprocess,sys,time,datetime
r=Path(__file__).resolve().parent;a=json.loads((r/'ACTIVE-valid.json').read_text());run=Path(a['run']);seen=set();deadline=time.monotonic()+28800
print('Observing this boot only: '+str(run),flush=True)
while time.monotonic()<deadline:
 if json.loads((r/'ACTIVE-valid.json').read_text())!=a:raise SystemExit('Active test changed; observer stopped')
 text=Path(a['log']).read_text(errors='replace').replace('\r','');runtime=text.split('Verified complete initialized memory capture',1)[-1]
 if any(s in runtime for s in ['VMAPPLE kernel panic format:','FATAL exception:','Launch Constraint Violation']):print('ATTENTION: native fault in current boot; inspect serial.log',flush=True);break
 p=subprocess.run([sys.executable,str(r/'inspect-live-windowserver-threads.py')],capture_output=True,text=True)
 report={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(run),'EFI_SHA256':a['EFI_SHA256'],'graphicalInstallerVerified':False}
 if p.returncode==0:
  d=json.loads(p.stdout);names={p['name']for p in d['processes']};notable=names&{'WindowServer','launchservicesd','lsd','coreservicesd','opendirectoryd','Installer Progre'}
  for name in sorted(notable-seen):print('NATIVE PROCESS OBSERVED: '+name+'; this does not establish visible installer UI',flush=True)
  seen|=notable;report.update(notableNativeProcesses=sorted(notable),WindowServerThreads=d.get('WindowServerThreads',[]),WindowServerState=d.get('WindowServerState'),nativeIOFramebufferRegistry=d.get('nativeIOFramebufferRegistry'))
  if 'WindowServer'in names:
   q=subprocess.run([sys.executable,str(r/'capture-live-display.py')],capture_output=True,text=True)
   if q.returncode==0:
    display=json.loads(q.stdout);report['display']=display
    if display['nonblackPixels']:
     (run/'GRAPHICAL-STARTUP-OBSERVATION.json').write_text(json.dumps(report,indent=2)+'\n');print('ATTENTION: nonblack real display captured; inspect image before calling it an installer: '+display['png'],flush=True);break
 else:report['probeIncomplete']=p.stderr[-600:]
 tmp=run/'GRAPHICAL-STARTUP-OBSERVATION.tmp';tmp.write_text(json.dumps(report,indent=2)+'\n');tmp.replace(run/'GRAPHICAL-STARTUP-OBSERVATION.json')
 if 'DONE engine integration tests'in runtime:print('Runtime ended; no installer success inferred',flush=True);break
 for _ in range(2):time.sleep(30)
print('Graphical startup observer ended; source test remains authoritative',flush=True)
