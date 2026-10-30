# SPDX-License-Identifier: MIT
"""Build a disposable exFAT test disk; never format a physical device."""
from pathlib import Path
import subprocess,plistlib,json,hashlib,shutil,sys
r=Path(__file__).resolve().parent;w=r.parent
if '--boot-context' in sys.argv:r=r/'BootContextExperiment'
elif '--hfs' in sys.argv:r=r/'HFSExperiment'
elif '--storage' in sys.argv:r=r/'StorageExperiment'
elif '--framebuffer' in sys.argv:r=r/'FramebufferExperiment'
elif '--installer' in sys.argv:r=r/'InstallerExperiment'
image=r/'test-inputs.raw';tools=w/'exfat-tools'
assert not image.exists(), 'Refusing to overwrite an existing test image; remove it only after its VM has stopped.'
if not (tools/'mkexfatfs').exists():
 subprocess.run(['clang','-O2','-D_FILE_OFFSET_BITS=64','-DVERSION="1.4.0"','-I'+str(tools/'libexfat'),*[str(p) for p in sorted((tools/'libexfat').glob('*.c'))],*[str(p) for p in sorted((tools/'mkfs').glob('*.c'))],'-o',str(tools/'mkexfatfs')],check=True)
with image.open('wb') as f:f.truncate(max(600*1024**2,((r/'vmapple-reference-boot-state.bin').stat().st_size//(1024**2)+65)*1024**2))
subprocess.run([str(tools/'mkexfatfs'),'-n','D27INPUT',str(image)],check=True)
mount=plistlib.loads(subprocess.check_output(['hdiutil','attach','-plist','-imagekey','diskimage-class=CRawDiskImage',str(image)]));point=Path(next(e['mount-point'] for e in mount['system-entities'] if 'mount-point' in e))
try:
 relative=Path(json.loads((r/'VMAPPLE-REFERENCE-BOOT.json').read_text())['inputRelativePath']);(point/relative).parent.mkdir(parents=True,exist_ok=True);shutil.copy2(r/'vmapple-reference-boot-state.bin',point/relative)
finally:subprocess.run(['hdiutil','detach',str(point)],check=True)
(r/'VirtualUSB/EFI/Drivers').mkdir(parents=True,exist_ok=True);shutil.copy2(w/'InstallerInputs/exfat_x64.efi',r/'VirtualUSB/EFI/Drivers/exfat_x64.efi')
(r/'EXFAT-TEST-IMAGE.json').write_text(json.dumps({'path':str(image),'bytes':image.stat().st_size,'source':'https://github.com/relan/exfat','revision':subprocess.check_output(['git','rev-parse','HEAD'],cwd=tools,text=True).strip(),'filesystem':'exFAT superfloppy','inputSHA256':json.loads((r/'VMAPPLE-REFERENCE-BOOT.json').read_text())['sha256']},indent=2)+'\n')
