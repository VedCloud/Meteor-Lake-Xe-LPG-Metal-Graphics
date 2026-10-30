from pathlib import Path
import shutil,tempfile,subprocess,hashlib,json
p=Path(__file__).resolve().parent;source=p/'Package';script=(source/'touchpad-trial.sh').read_text();body=script.split('# BEGIN TRANSACTION:',1)[1];body=body[body.index('\n')+1:]
h=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
files=sorted(f for f in source.rglob('*') if f.is_file() and f.name!='PAYLOAD.sha256' and not f.name.startswith('._') and f.name!='CURRENT-TRIAL.json' and 'reports' not in f.relative_to(source).parts and 'Research' not in f.relative_to(source).parts)
(source/'PAYLOAD.sha256').write_text(''.join(h(f)+'  '+str(f.relative_to(source))+'\n' for f in files))
with tempfile.TemporaryDirectory(prefix='latitude-input-fixture-') as td:
 root=Path(td);here=root/'Tahoe-Port/Touchpad';shutil.copytree(source,here);graphics=here.parent/'Graphics';graphics.mkdir();shutil.copy2(p.parent/'Tahoe-Combined-Runtime/ocvalidate',graphics/'ocvalidate');probe=graphics/'tahoe-runtime-check';probe.write_text('#!/bin/bash\nexit 0\n');probe.chmod(0o755)
 efi=root/'EFI';(efi/'OC/ACPI').mkdir(parents=True);(efi/'OC/Kexts').mkdir();baseline=here/'Baseline'
 shutil.copy2(baseline/'config.plist',efi/'OC/config.plist');shutil.copy2(baseline/'DSDT.aml',efi/'OC/ACPI/DSDT.aml')
 for name in ['VoodooI2C.kext','VoodooI2CHID.kext']:shutil.copytree(baseline/name,efi/'OC/Kexts'/name)
 rel=Path('OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist');(efi/rel).parent.mkdir(parents=True);shutil.copy2(Path('/Volumes/EFI/EFI')/rel,efi/rel)
 def snapshot():return {str(f.relative_to(efi)):h(f) for f in efi.rglob('*') if f.is_file() and not f.name.startswith('._')}
 before=snapshot();counter=0
 def run(mode,fail=False):
  global counter
  counter+=1;report=root/('report-'+str(counter));report.mkdir();prefix='#!/bin/bash\nset -euo pipefail\nHERE='+repr(str(here))+'\nEFI='+repr(str(efi))+'\nREPORT='+repr(str(report))+'\nMODE='+mode+'\nuname(){ echo x86_64; }\n'
  b=body
  if fail:b=b.replace('# TEST_TRANSACTION_COMMIT_POINT','exit 77',1)
  f=root/'run-fixture.sh';f.write_text(prefix+b);return subprocess.run(['/bin/bash',str(f)],text=True,capture_output=True)
 for mode in ['status','apply','apply']:
  result=run(mode);assert result.returncode==0,result.stdout+result.stderr
 after=snapshot();assert after!=before
 result=run('restore');assert result.returncode==0,result.stdout+result.stderr;assert snapshot()==before
 assert run('restore').returncode==0
 result=run('apply',True);assert result.returncode==77,result.stdout+result.stderr;assert snapshot()==before,'failed transaction did not restore byte-identical input files'
 cfg=efi/'OC/config.plist';cfg.write_bytes(cfg.read_bytes()+b'\n');edited=snapshot();assert run('apply').returncode==2;assert snapshot()==edited,'edited EFI was overwritten'
 # Begin with the exact previous-version manifest and verify upgrade and restoration.
 prev=Path('/Volumes/Corsair 1TB NVMe/Dell-Tahoe-20261026/Touchpad-CompleteInput-1.0.9/Package')
 shutil.copy2(prev/'config.plist',efi/'OC/config.plist');shutil.copy2(prev/'DSDT.aml',efi/'OC/ACPI/DSDT.aml')
 for name in ['VoodooI2C.kext','VoodooI2CHID.kext']:
  shutil.rmtree(efi/'OC/Kexts'/name);shutil.copytree(prev/name,efi/'OC/Kexts'/name)
 result=run('status');assert result.returncode==0,result.stdout+result.stderr
 result=run('apply');assert result.returncode==0,result.stdout+result.stderr
 assert snapshot()==after,'previous-version upgrade did not select exact new bytes'
 result=run('restore');assert result.returncode==0,result.stdout+result.stderr;assert snapshot()==before
 result={'previousVersionUpgrade':True,'productionHardwareAndDiskGuardsMockedForFixtureOnly':True,'actualTransactionLogic':'passed','applyRestoreByteEquality':True,'idempotence':True,'interruptedCommitRollback':True,'unrelatedEditRefused':True,'baselineConfigSHA256':before['OC/config.plist']}
 (p/'TRANSACTION-TESTS.json').write_text(json.dumps(result,indent=2)+'\n');print('PASS actual trial transaction: apply/restore, byte-identical rollback, idempotence and unrelated-edit refusal; hardware/discovery mocked only in the temporary fixture.')
