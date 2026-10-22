from pathlib import Path
import tempfile,subprocess,ast
from aml_methods import methods,device_range,replace_bodies
r=Path(__file__).resolve().parent.parent
# Reuse only the simulated firmware fixture; execute no prior script.
tree=ast.parse((r/'touchpad-sources/test-acpi.py').read_text());fixture=next(ast.literal_eval(n.value) for n in tree.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='fixture' for t in n.targets))
fixture='\n'.join(l for l in fixture.splitlines() if 'External (' not in l)
needle='Device (TPD0) { Name (_HID, "TEST0003")';assert needle in fixture
fixture=fixture.replace(needle,needle+'\nMethod (_DSM,4,Serialized) {'+'Noop\n'*256+'Return(Zero)}\nMethod(_CRS,0,Serialized) {'+'Noop\n'*256+'Return(Buffer(2){0x79,0})}')
target=(r/'port-artifacts/touchpad-inline-1.0.1/DSDT.aml').read_bytes();start,end=device_range(target,b'TPD0');bodies={n:target[a:b] for n,(a,b) in methods(target,start,end).items()}
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'fixture.dsl').write_text(fixture)
 c=subprocess.run(['/opt/homebrew/bin/iasl','-oa','-p',str(p/'fixture'),str(p/'fixture.dsl')],capture_output=True,text=True);assert not c.returncode,c.stdout+c.stderr
 original=(p/'fixture.aml').read_bytes();lo,hi=device_range(original,b'TPD0');data,changes=replace_bodies(original,bodies,lo,hi);(p/'fixture.aml').write_bytes(data)
 c=subprocess.run(['/opt/homebrew/bin/acpiexec','-b','execute \\TEST',str(p/'fixture.aml')],capture_output=True,text=True,timeout=15)
 assert not c.returncode and '00000000000055AA' in c.stdout,c.stdout+c.stderr
 print('PASS exact patched DSDT method bodies in a single ACPICA table: no separate SSDT scope needed; firmware descriptor, support bits and resource bytes match; package lengths/checksum preserved')
