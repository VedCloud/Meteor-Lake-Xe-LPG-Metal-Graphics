from pathlib import Path
import tempfile,subprocess,os,time
r=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory(prefix='desktop-crash-collector-') as tmp:
 base=Path(tmp)
 for mode in ['pass','no-crash','history-fails','no-weather']:
  p=base/mode;p.mkdir();(p/'bin').mkdir();system=p/'system reports';user=p/'user home';(user/'Library/Logs/DiagnosticReports').mkdir(parents=True);system.mkdir()
  if mode!='no-crash':
   for folder in [system,user/'Library/Logs/DiagnosticReports']:
    (folder/'WindowServer-2026 sample.ips').write_text('full crash stack '+str(folder))
    f=folder/'WindowServer-old.crash';f.write_text('old');t=time.time()-3*86400;os.utime(f,(t,t))
    (folder/'OtherApp.ips').write_text('unrelated')
  panics=p/'panic reports';(panics/'Retired').mkdir(parents=True)
  if mode!='no-crash':
   (panics/'panic-full sample.ips').write_text('full panic evidence')
   (panics/'Retired/Kernel-2026.panic').write_text('retired panic evidence')
   old=panics/'old.panic';old.write_text('old panic');t=time.time()-3*86400;os.utime(old,(t,t))
   (panics/'OtherApp.ips').write_text('unrelated')
  retained=p/'retained-source';retained.mkdir();(retained/'LatitudeDesktopLive.log').write_text('failed boot live evidence');(retained/'LatitudeDesktopRegistry.log.previous').write_text('previous registry evidence')
  for name in ['LatitudeDesktopAttempt.trace','LatitudeDesktopAttempt.trace.previous','LatitudeWindowServerSample.txt','LatitudeWindowServerSample.stderr.txt']:(retained/name).write_text('captured '+name)
  script=(r/'desktop-sources/collect-desktop-failure.sh').read_text().replace('crash_root=/Library/Logs/DiagnosticReports','crash_root="'+str(system)+'"',1).replace('user_home=${HOME:-/var/root}','user_home="'+str(user)+'"').replace('/usr/bin/log show','log show').replace('/usr/sbin/system_profiler','system_profiler').replace('/usr/sbin/ioreg','ioreg').replace('/usr/bin/pgrep','pgrep').replace('/usr/bin/sample','sample').replace('/Library/Logs/$saved',str(retained)+'/$saved').replace('/Library/GPUBundles',str(p/'missing-runtime')).replace('/bin/launchctl','launchctl').replace('/usr/bin/id','id').replace('      sleep 8\n','      : # fixture observation window\n')
  script=script.replace('for panic_root in /Library/Logs/DiagnosticReports /Library/Logs/PanicReporter; do','for panic_root in "'+str(panics)+'"; do').replace('/Library/Logs/panic.log',str(p/'missing-legacy-panic.log'))
  (p/'collect-desktop-failure.sh').write_text(script)
  for name,body in {'pgrep':'if [ "$2" = Weather ]; then [ "$MODE" != no-weather ] || exit 1; echo 1008; else echo 163; fi','sample':'echo current-stack > "${@: -1}"; touch "$MOCK_SAMPLE_DONE"','ioreg':'echo fixture-input-chain','system_profiler':'echo \'{"SPDisplaysDataType":[]}\'', 'uname':'echo x86_64','sysctl':'echo boot-fixture','sw_vers':'echo version-fixture','log':'printf \'%s\\n\' "$*" > "$MOCK_LOG_ARGS"; echo "fixture history gamma AppleALC"; exit "${MOCK_LOG_RESULT:-0}"','sleep':'exec /bin/sleep 3'}.items():
   f=p/'bin'/name;f.write_text('#!/bin/bash\n'+body+'\n');f.chmod(0o755)
  (p/'Desktop/reports').mkdir(parents=True)
  (p/'Desktop/AboutThisMac').mkdir(parents=True)
  helper=p/'Desktop/AboutThisMac/latitude-about-this-mac'
  helper.write_text('#!/bin/bash\n[ "$1" = diagnose ] || exit 9\nprintf \'<plist version="1.0"><dict><key>fixture</key><true/></dict></plist>\\n\'\n[ "$MODE" != no-crash ]\n')
  helper.chmod(0o755)
  key=p/'Desktop/latitude-key-state';key.write_text('#!/bin/bash\necho \'{"requestResult":0,"counters":{"Query66Entries":2}}\'\n');key.chmod(0o755)
  frame=p/'Desktop/latitude-frame-pacing';frame.write_text('#!/bin/bash\necho \'{"status":"fixture-captured","total_render_latency_ms":null}\'\n');frame.chmod(0o755)
  for name,body in {'id':'echo 501','launchctl':'exec "${@: -1}"'}.items():
   f=p/'bin'/name;f.write_text('#!/bin/bash\n'+body+'\n');f.chmod(0o755)


  date_stub=p/'bin/date';date_stub.write_text('#!/bin/bash\nif [ "$*" = "+%Y-%m-%d %H:%M:%S" ]; then if [ -e "$MOCK_SAMPLE_DONE" ]; then echo "2026-10-20 15:00:59"; else echo "2026-10-20 15:00:00"; fi; else exec /bin/date "$@"; fi\n');date_stub.chmod(0o755)
  env=os.environ.copy();env.update(MOCK_SAMPLE_DONE=str(p/'sample-done'),MOCK_LOG_ARGS=str(p/'log-args'),PATH=str(p/'bin')+':/usr/bin:/bin',SUDO_USER='fixture-user',MODE=mode,MOCK_LOG_RESULT='1' if mode=='history-fails' else '0')
  args=['2026-10-20 14:00:00','2026-10-20 15:00:03'] if mode=='no-crash' else []
  c=subprocess.run(['/bin/bash',str(p/'collect-desktop-failure.sh'),*args],env=env,capture_output=True,text=True,timeout=3)
  assert c.returncode==(1 if mode=='history-fails' else 0),c.stdout+c.stderr
  out=next((p/'Desktop/reports').glob('failure-detail-*'));copied=list((out/'WindowServer-crashes').rglob('*.ips'))
  assert ('end=2026-10-20 15:00:03' if mode=='no-crash' else 'end=2026-10-20 15:00:59') in (out/'log-window.txt').read_text()
  assert 'fixture' in (out/'about-this-mac.plist').read_text()
  assert 'fixture-captured' in (out/'frame-pacing.json').read_text()
  assert 'exit=0' in (out/'frame-pacing.status.txt').read_text()
  assert (out/'front-copy-before.txt').exists() and (out/'front-copy-after.txt').exists()
  assert 'Query66Entries' in (out/'input/firmware-key-counters.json').read_text()
  assert 'exit=0' in (out/'input/firmware-key-counters.status.txt').read_text()
  assert (out/'input/ec-acpi.txt').exists()
  assert (out/'input/graphics-acpi.txt').exists()
  assert 'nmxfb_gamma_ns' in (r/'desktop-sources/collect-desktop-failure.sh').read_text()
  assert (out/'display-path-counters.txt').exists()
  assert 'fixture-input-chain' in (out/'gamma-registry.txt').read_text()
  for name in ['AppleHDAController','IOHDACodecDevice','AppleHDACodecGeneric','IOAudioDevice']:
   assert 'fixture-input-chain' in (out/'audio'/f'{name}.txt').read_text()
  assert (out/'audio/devices.json').exists()
  assert (out/'input/BrightnessKeys.txt').exists() and (out/'input/ApplePS2Keyboard.txt').exists()
  assert 'AppleALC' in (out/'new-feature-log-summary.txt').read_text()
  for token in ['coreaudiod','corebrightnessd','displaypolicyd','AppleALC','AppleHDA']:
   assert token in (p/'log-args').read_text()

  assert ('exit=1' if mode=='no-crash' else 'exit=0') in (out/'about-this-mac.status.txt').read_text()
  assert len(copied)==(0 if mode=='no-crash' else 2),list(out.rglob('*'))
  assert not list((out/'WindowServer-crashes').rglob('*.crash'))
  panic_files=list((out/'Kernel-panics').rglob('*.ips'))+list((out/'Kernel-panics').rglob('*.panic'))
  assert len(panic_files)==(0 if mode=='no-crash' else 2),panic_files
  assert all('panic evidence' in f.read_text() for f in panic_files)
  assert ('copied=0' if mode=='no-crash' else 'copied=2 failed=0') in (out/'panic-status.txt').read_text()

  assert (out/'retained-boot-files/LatitudeDesktopLive.log').read_text()=='failed boot live evidence'
  assert (out/'retained-boot-files/LatitudeDesktopRegistry.log.previous').read_text()=='previous registry evidence'
  for name in ['LatitudeDesktopAttempt.trace','LatitudeDesktopAttempt.trace.previous','LatitudeWindowServerSample.txt','LatitudeWindowServerSample.stderr.txt']:assert (out/'retained-boot-files'/name).read_text()=='captured '+name
  assert ('skipped' if mode=='no-weather' else 'PID=1008 exit=0') in (out/'weather-sample.status.txt').read_text()
  if mode!='no-weather':assert (out/'weather-sample.txt').read_text().strip()=='current-stack'
  assert 'MISSING LatitudePanelLoad.log'  in (out/'retained-files-status.txt').read_text()
  if copied:
   assert all(f.read_text().startswith('full crash stack ') for f in copied)
   assert 'copied=2 failed=0' in (out/'crash-status.txt').read_text()
  else:assert 'No recent WindowServer crash report found' in (out/'crash-status.txt').read_text()
 print('PASS actual desktop collector: full system/user WindowServer crash files with spaces, old/unrelated reports excluded, distinct sources preserved, absent report reported, crashes retained when historical search fails; no GPU tests')
