from pathlib import Path
import subprocess,tempfile,sys,re
root=Path(__file__).resolve().parent.parent
sys.path.insert(0,str(root/'tablet-sources'));from importlib import util
spec=util.spec_from_file_location('prep',root/'tablet-sources/prepare-visibility.py');m=util.module_from_spec(spec);spec.loader.exec_module(m)
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp)
 for raw in [0,1,2,255,256]:
  source='''DefinitionBlock ("", "DSDT", 2, "LAT535", "TESTMODE", 1) {
   Method (ECRB, 1, Serialized) { If (Arg0 == 0x39) { Return (RAW_VALUE) } Return (0xFF) }
   Scope (\\_SB) { Device (HIDD) {Name (_HID, "INTC1077") Method (_STA, 0, Serialized) {If (OIDE () < 1) {Return (0)} Return (15)} Method (OIDE,0) {Return(0)} } }
  }'''.replace('RAW_VALUE',str(raw))
  dsl=p/'base.dsl';dsl.write_text(source);subprocess.run(['/opt/homebrew/bin/iasl',str(dsl)],check=True,capture_output=True)
  modified,at=m.patch((p/'base.aml').read_bytes());(p/'patched.aml').write_bytes(modified)
  result=subprocess.run(['/opt/homebrew/bin/acpiexec','-b','evaluate \\_SB.HIDD._STA; evaluate \\_SB.HIDD.XSTA; evaluate \\_SB.HIDD.LTSM',str(p/'patched.aml'),str(root/'port-artifacts/tablet-mode-v2/SSDT-LAT-TABLET.aml')],capture_output=True,text=True,timeout=15)
  output=result.stdout+result.stderr
  assert result.returncode==0 and 'AE_ALREADY_EXISTS' not in output and 'AE_NOT_FOUND' not in output,output
  assert '000000000000000F' in output and '000000004C545331' in output,output
  section=output.split('Evaluating \\_SB.HIDD.LTSM',1)[1]
  numbers=[int(x,16) for x in re.findall(r'\[Integer\] = ([0-9A-F]{16})',section)]
  assert numbers[:3]==[0x4c545331,int(raw<255),raw if raw<255 else 0],output
print('PASS scoped AML rename, original method retained, SSDT namespace loading, _STA=15 and firmware result package in real ACPICA interpreter for normal/folded/error fixture values. No hardware access.')
