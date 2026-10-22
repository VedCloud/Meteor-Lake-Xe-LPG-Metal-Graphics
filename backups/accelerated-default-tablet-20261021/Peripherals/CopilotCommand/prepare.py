from pathlib import Path
import plistlib,itertools,json,shutil
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');out=r/'port-artifacts/copilot-command';out.mkdir(exist_ok=True)
src=u/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext'
shutil.copytree(src,out/'VoodooPS2Keyboard.kext',dirs_exist_ok=True)
p=out/'VoodooPS2Keyboard.kext/Contents/Info.plist';d=plistlib.loads(p.read_bytes());profiles=d['IOKitPersonalities']['ApplePS2Keyboard']['Platform Profile']
# Prefix: ignored FFFF, output packet, modifier mask/compare. Preserve existing macros.
macros=[]
for down,outcode in [(True,0x5b),(False,0xdb)]:
 seq=[bytes([1,0x2a if down else 0xaa]),bytes([2,0x5b if down else 0xdb]),bytes([1,0x6e if down else 0xee])]
 for order in itertools.permutations(seq):macros.append(bytes([0xff,0xff,2,outcode,0,0,0,0])+b''.join(order))
profiles['Default'].setdefault('Macro Inversion',[]).extend(macros)
# A selected Dell profile can override the default array: extend existing Dell arrays too.
def extend(node):
 if not isinstance(node,dict):return
 if 'Macro Inversion' in node:node['Macro Inversion'].extend(macros)
 for value in node.values():
  if isinstance(value,dict):extend(value)
extend(profiles.get('DELL',{}))
p.write_bytes(plistlib.dumps(d))
(out/'macros.json').write_text(json.dumps([m.hex() for m in macros],indent=2)+'\n')
print('Prepared12 standard Copilot make/break chord inversions; keyboard executable unchanged. Hardware sequence unverified.')
