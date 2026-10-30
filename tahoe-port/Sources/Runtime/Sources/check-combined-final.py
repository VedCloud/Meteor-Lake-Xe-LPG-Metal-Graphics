from pathlib import Path
import subprocess,os,re,json,plistlib,copy,hashlib
b=Path(__file__).resolve().parent;p=b/'Tahoe-Combined-Runtime';physical=b/'CombinedTransactionTests';root=next(Path('/private/tmp').glob('dell-tahoe-combined-tests-*'))/'success'
# Follow the actual fixture root referred to by the most recent completed successful transaction.
for candidate in Path('/private/tmp').glob('dell-tahoe-combined-tests-*'):
 if (candidate/'success/Library/NullMoth/tahoe-backups').exists():root=candidate/'success'
backup=next((root/'Library/NullMoth/tahoe-backups').iterdir());source=(p/'rollback-windowserver.sh').read_text();s=re.sub(r'(?<!\$HERE)/(?:Library|System|private/var|Volumes)/',lambda m:str(root)+m.group(0),source)
for name in ['diskutil','nvram']:s=s.replace('/usr/sbin/'+name,str(root/'bin'/name))
assert not re.search(r'(?:"| )/(?:Library|System|private/var|Volumes)/',s)
f=root/'fixture-rollback.sh';f.write_text(s);env=dict(os.environ,PATH=str(root/'bin')+':/usr/bin:/bin:/usr/sbin:/sbin',TEST_ROOT=str(root),TEST_PACKAGE=str(root/'Package'),TEST_MODE='success')
run=subprocess.run(['/bin/bash',str(f),str(backup)],env=env,capture_output=True,text=True,timeout=30);assert run.returncode==0,run.stdout+run.stderr
cfg=root/'Volumes/EFI/EFI/OC/config.plist';assert cfg.read_bytes()==(b/'Candidate/EFI/OC/config.plist').read_bytes();assert not(cfg.parent/'Kexts/SimpleHDA.kext').exists()
# The combined profiles must differ from the old profiles only in the explicit audio changes.
guid='7C436110-AB2A-4BBB-A880-FE41995C9F82'
for mode in ['accelerated','recovery']:
 old=plistlib.loads((b/'Tahoe-Graphics-Runtime/Profiles'/(mode+'.plist')).read_bytes());new=plistlib.loads((p/'Profiles'/(mode+'.plist')).read_bytes());clean=copy.deepcopy(new)
 clean['Kernel']['Add']=[k for k in clean['Kernel']['Add'] if k['BundlePath']!='SimpleHDA.kext']
 for k in clean['Kernel']['Add']:
  if k['BundlePath']=='AppleALC.kext':k.update(next(x for x in old['Kernel']['Add'] if x['BundlePath']=='AppleALC.kext'))
 clean['NVRAM']['Add'][guid]['boot-args']=old['NVRAM']['Add'][guid]['boot-args'];assert clean==old,mode
 args=new['NVRAM']['Add'][guid]['boot-args'].split();assert all(x in args for x in old['NVRAM']['Add'][guid]['boot-args'].split())
 subprocess.run([str(p/'ocvalidate'),str(p/'Profiles'/(mode+'.plist'))],stdout=subprocess.DEVNULL,check=True)
status=json.loads((b/'COMBINED-TRANSACTION-TESTS.json').read_text());status['manualRollbackPassed']=True;status['unrelatedEFISettingsPreserved']=True;status['AMFIBypassPreserved']=True;(b/'COMBINED-TRANSACTION-TESTS.json').write_text(json.dumps(status,indent=2)+'\n')
print('PASS manual rollback; both profiles preserve every non-audio EFI setting, including AMFI, graphics, Wi-Fi and input.')
