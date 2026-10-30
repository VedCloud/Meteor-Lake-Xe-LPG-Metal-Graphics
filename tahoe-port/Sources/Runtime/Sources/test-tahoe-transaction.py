from pathlib import Path
import subprocess,tempfile,os,json,plistlib,shutil,hashlib,re
b=Path(__file__).resolve().parent;pkg=b/'Tahoe-Graphics-Runtime';physical=b/'TransactionTests';physical.mkdir(exist_ok=True)
tests=Path(tempfile.mkdtemp(prefix='dell-tahoe-tests-',dir='/private/tmp'));tests.rmdir();tests.symlink_to(physical,target_is_directory=True)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def manifest(p):
 (p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in sorted(p.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.txt' and 'reports' not in f.relative_to(p).parts))
manifest(pkg)
source=(pkg/'install-windowserver.sh').read_text();results=[]
dispatch='''#!/usr/bin/python3
from pathlib import Path
import os,sys,plistlib,shutil,subprocess
name=Path(sys.argv[0]).name;a=sys.argv[1:];r=Path(os.environ['TEST_ROOT']);p=Path(os.environ['TEST_PACKAGE']);mode=os.environ['TEST_MODE']
if name=='id':print(0);sys.exit()
if name=='uname':print('x86_64');sys.exit()
if name=='sw_vers':print('25G999' if mode=='unknown-build' and a==['-buildVersion'] else '25G241' if a==['-buildVersion'] else '26.7.1');sys.exit()
if name in ['chown']:sys.exit()
if name=='ioreg':sys.exit()
if name=='diskutil':
 if '-plist' in a:sys.stdout.buffer.write(plistlib.dumps({'ParentWholeDisk':'disk99','DeviceIdentifier':'disk99s1','VolumeUUID':(p/'USB-EFI-UUID.txt').read_text().strip(),'MountPoint':str(r/'Volumes/EFI')}))
 sys.exit()
if name=='nvram':
 if '=' not in ' '.join(a):sys.exit(1)
 sys.exit(9 if mode=='nvram-failure' else 0)
if name=='kcditto':
 for rel in ['Group/Library/KernelCollections/AuxiliaryKernelExtensions.kc','NewGroup/Library/KernelCollections/AuxiliaryKernelExtensions.kc']:
  q=r/'System/Volumes/Preboot'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b'new-preboot')
 sys.exit(7 if mode=='cache-copy-failure' else 0)
if name=='kmutil':
 if a[0]=='showloaded':print('com.dhinakg.AMFIPass');sys.exit()
 path=Path(a[a.index('-A')+1])
 if a[0]=='create':
  records=[]
  for i,v in enumerate(a):
   if v!='-p':continue
   k=Path(a[i+1]);n=k.stem
   if n not in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']:continue
   if mode=='missing-preflight' and path.name=='preflight.kc' and n=='NMXAccel':continue
   uu=(p/(n+'-UUID.txt')).read_text().strip()
   if mode=='wrong-final-uuid' and path.name=='final.kc' and n=='NMXAccel':uu='00000000-0000-0000-0000-000000000000'
   records.append({'CFBundleIdentifier':(p/(n+'-ID.txt')).read_text().strip(),'CFBundleVersion':(p/(n+'-VERSION.txt')).read_text().strip(),'OSBundleUUID':bytes.fromhex(uu.replace('-','')),'_PrelinkBundlePath':str(k),'_PrelinkExecutableRelativePath':'Contents/MacOS/'+n,'_PrelinkExecutableSize':1})
  path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(plistlib.dumps({'_PrelinkInfoDictionary':records}));sys.exit()
 if a[0]=='inspect':sys.stdout.buffer.write(path.read_bytes());sys.exit()
sys.exit(2)
'''
for mode in ['success','missing-preflight','wrong-final-uuid','cache-copy-failure','nvram-failure','unknown-build','config-changed']:
 root=tests/mode
 if root.exists():shutil.rmtree(root)
 root.mkdir();p=root/'Package';shutil.copytree(pkg,p,copy_function=os.link,ignore=shutil.ignore_patterns('reports'))
 (root/'bin').mkdir();(root/'private/var/run').mkdir(parents=True);(root/'private/var/tmp').mkdir(parents=True)
 for name in ['id','uname','sw_vers','chown','ioreg','diskutil','nvram','kcditto','kmutil']:
  q=root/'bin'/name;q.write_text(dispatch);q.chmod(0o755)
 helper=p/'tahoe-runtime-check';helper.unlink();helper.write_text('#!/usr/bin/python3\nimport os,sys\na=sys.argv[1:]\nif a==["--hardware"]:sys.exit(0)\nif a[0]=="--families":a[-1]=os.environ["TEST_ROOT"]\nos.execv('+repr(str(pkg/'tahoe-runtime-check'))+','+[repr(str(pkg/'tahoe-runtime-check'))].__repr__()+'+a)\n');helper.chmod(0o755)
 # Real native family UUID check runs against actual Apple KDK binaries in a disposable filesystem root.
 families=plistlib.loads((pkg/'ABI-qualifications.plist').read_bytes())['25G241'];kdk=b/'KDK25G241-expanded/KDK.pkg/Payload'
 for family in families:
  src=next(kdk/x for x in family['paths'] if (kdk/x).is_file());dst=root/family['paths'][0];dst.parent.mkdir(parents=True,exist_ok=True);os.link(src,dst)
 for f in ['System/Library/KernelCollections/BootKernelExtensions.kc','System/Library/KernelCollections/SystemKernelExtensions.kc','Library/KernelCollections/AuxiliaryKernelExtensions.kc','System/Volumes/Preboot/Group/Library/KernelCollections/AuxiliaryKernelExtensions.kc']:
  dest=root/f;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(b'old-'+f.encode())
 for n in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']:
  q=root/'Library/Extensions'/(n+'.kext')/'Contents/MacOS'/n;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(('old-'+n).encode())
 cfg=root/'Volumes/EFI/EFI/OC/config.plist';cfg.parent.mkdir(parents=True)
 original=(b/'Candidate/EFI/OC/config.plist').read_bytes()
 if mode=='config-changed':
  d=plistlib.loads(original);d['Misc']['Boot']['Timeout']=17;original=plistlib.dumps(d)
 cfg.write_bytes(original)
 shutil.copytree(pkg/'EFI-Kexts/LatitudePanelClaim.kext',cfg.parent/'Kexts/LatitudePanelClaim.kext')
 feature=root/'Library/Preferences/FeatureFlags/Domain/CoreMedia.plist';feature.parent.mkdir(parents=True,exist_ok=True);feature.write_bytes(plistlib.dumps({'OtherFeature':{'Enabled':True}}))
 protected=[root/'Library/Extensions'/(n+'.kext')/'Contents/MacOS'/n for n in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']]+[cfg,feature,root/'Library/KernelCollections/AuxiliaryKernelExtensions.kc',root/'System/Volumes/Preboot/Group/Library/KernelCollections/AuxiliaryKernelExtensions.kc']
 before={str(x):x.read_bytes() for x in protected}
 s=source
 s=re.sub(r'(?<!\$HERE)/(?:Library|System|private/var|Volumes)/',lambda m:str(root)+m.group(0),s)
 s=s.replace('src="$HERE$path"','src="$HERE${path#"$TEST_ROOT"}"')
 for name in ['kmutil','diskutil','nvram','kcditto']:
  s=s.replace('/usr/bin/'+name,str(root/'bin'/name)).replace('/usr/sbin/'+name,str(root/'bin'/name))
 # Verify fixture mapping removed every literal live-host destination before executing.
 assert not re.search(r'(?<!\$HERE)(?<![A-Za-z0-9_])(?:"| )/(?:Library|System|private/var|Volumes)/',s)
 script=p/'fixture-install.sh';script.write_text(s)
 # The fixture script and mocked dispatch are part of the fixture checksum set, not production tests passing as hardware proof.
 sums=p/'SHA256SUMS.txt';sums.unlink();manifest(p)
 env=dict(os.environ,PATH=str(root/'bin')+':/usr/bin:/bin:/usr/sbin:/sbin',TEST_ROOT=str(root),TEST_PACKAGE=str(p),TEST_MODE=mode)
 log=root/'test.log'
 with log.open('w') as f:run=subprocess.run(['/bin/bash',str(script)],env=env,stdout=f,stderr=subprocess.STDOUT,timeout=90)
 if mode=='success':
  assert run.returncode==0,log.read_text()[-4000:]
  assert plistlib.loads(cfg.read_bytes())==plistlib.loads((pkg/'Profiles/accelerated.plist').read_bytes())
  flags=plistlib.loads(feature.read_bytes());assert flags['OtherFeature']['Enabled'] and not flags['AllowOnePassMetalScaling']['Enabled']
  for n in ['NMXGPU','NMXAccel','LatitudeMTLFB','NMXAGDC']:assert (root/'Library/Extensions'/(n+'.kext')/'Contents/MacOS'/n).read_bytes()==(pkg/'Library/Extensions'/(n+'.kext')/'Contents/MacOS'/n).read_bytes()
 else:
  assert run.returncode!=0,(mode,log.read_text()[-3000:])
  for x,content in before.items():assert Path(x).read_bytes()==content,(mode,x,log.read_text()[-3000:])
  assert not(root/'System/Volumes/Preboot/NewGroup/Library/KernelCollections/AuxiliaryKernelExtensions.kc').exists(),mode
 assert not(root/'private/var/run/latitude-tahoe-graphics.lock').exists(),mode
 results.append({'case':mode,'passed':True,'exit':run.returncode});print('PASS',mode,run.returncode,flush=True)
(b/'TRANSACTION-TESTS.json').write_text(json.dumps({'tests':results,'scope':'Actual shell transaction with real signature/UUID/config/prelink validators, mocked hardware and kmutil/cache/NVRAM endpoints in disposable roots; not a target kernel collection or GPU test','hostInstallationPerformed':False},indent=2)+'\n')
