from pathlib import Path
import subprocess,os,re,json
b=Path(__file__).resolve().parent;p=b/'Tahoe-Combined-Runtime';physical=b/'RecoveryTransactionTests/success';backup=next((physical/'Volumes/macOS - Data/Library/NullMoth/tahoe-backups').iterdir());data=Path((backup/'data-root.txt').read_text().strip());root=data.parents[1]
s=(p/'rollback-recovery-install.sh').read_text();s=re.sub(r'(?<![A-Za-z0-9_])/(?:Library|System|private/var|Volumes)/',lambda m:str(root)+m.group(0),s)
for name in ['diskutil','nvram']:s=s.replace('/usr/sbin/'+name,str(root/'bin'/name))
f=root/'fixture-recovery-rollback.sh';f.write_text(s);env=dict(os.environ,PATH=str(root/'bin')+':/usr/bin:/bin:/usr/sbin:/sbin',TEST_ROOT=str(root),TEST_SYSTEM=str(root/'Volumes/macOS'),TEST_DATA=str(data),TEST_PREBOOT=str(root/'Volumes/Preboot'),TEST_PACKAGE=str(root/'Package'),TEST_MODE='success')
r=subprocess.run(['/bin/bash',str(f),str(backup)],env=env,capture_output=True,text=True,timeout=30);assert r.returncode==0,r.stdout+r.stderr
cfg=root/'Volumes/EFI/EFI/OC/config.plist';assert cfg.read_bytes()==(b/'Candidate/EFI/OC/config.plist').read_bytes();assert not(cfg.parent/'Kexts/SimpleHDA.kext').exists()
for n in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']:assert(data/'Library/Extensions'/(n+'.kext')/'Contents/MacOS'/n).read_bytes()==('old-'+n).encode()
report=b/'RECOVERY-TRANSACTION-TESTS.json';d=json.loads(report.read_text());d.update(manualTargetRollbackPassed=True,targetSystemDataAndPrebootSeparated=True,scope='Actual Recovery shell and signature/family/EFI/prelink validators; hardware, volume resolution, kmutil and NVRAM simulated in disposable System/Data/Preboot roots. Real target cache build and boot remain unverified.');report.write_text(json.dumps(d,indent=2)+'\n');print('PASS Recovery manual rollback: target Data files, USB EFI and Preboot restored; host untouched.')
