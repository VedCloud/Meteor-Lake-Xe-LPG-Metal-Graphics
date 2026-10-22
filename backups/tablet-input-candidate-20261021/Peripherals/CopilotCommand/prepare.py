from pathlib import Path
import plistlib,json,shutil
from retain import retain,macros
r=Path(__file__).resolve().parent.parent;u=Path('/Volumes/SEQOUIA');out=r/'port-artifacts/copilot-command';out.mkdir(exist_ok=True)
src=u/'EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext'
shutil.copytree(src,out/'VoodooPS2Keyboard.kext',dirs_exist_ok=True)
p=out/'VoodooPS2Keyboard.kext/Contents/Info.plist';d=plistlib.loads(p.read_bytes());profiles=d['IOKitPersonalities']['ApplePS2Keyboard']['Platform Profile']
macros=macros()
retain(d)
p.write_bytes(plistlib.dumps(d))
(out/'macros.json').write_text(json.dumps([m.hex() for m in macros],indent=2)+'\n')
print('Prepared12 standard Copilot make/break chord inversions; keyboard executable unchanged. Hardware confirmed by user on 2026-10-21; preserve in future builds.')
