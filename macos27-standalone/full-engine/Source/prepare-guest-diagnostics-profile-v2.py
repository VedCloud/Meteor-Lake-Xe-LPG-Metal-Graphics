# SPDX-License-Identifier: MIT
"""Stage an optional guest-observer profile without repointing the active build."""
from pathlib import Path
import json,subprocess,mmap,hashlib
root=Path(__file__).resolve().parent;src=(root/'BootContextExperiment').resolve();d=src.parent/'BootContextExperiment-GuestDiagnostics-v2';assert not d.exists();d.mkdir();meta=json.loads((src/'VMAPPLE-REFERENCE-BOOT.json').read_text());observer=json.loads((root/'PENDING-DIAGNOSTICS-IMAGE.json').read_text());assert observer['filesystemCheck']['exitCode']==0 and observer['originalUnchanged'];image=Path(observer['image']);capture=d/'vmapple-reference-boot-state.bin';subprocess.run(['cp','-c',str(src/capture.name),str(capture)],check=True)
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
assert sha(image)==observer['imageSHA256'] and sha(capture)==meta['sha256'];at=meta['recoveryImagePhysical']-meta['guestBase'];size=meta['recoveryImageBytes'];assert image.stat().st_size==size;changed=0
with capture.open('r+b') as f:
 im=mmap.mmap(f.fileno(),0);before=hashlib.sha256()
 for offset in range(0,size,1024*1024):before.update(im[at+offset:at+min(offset+1024*1024,size)])
 assert before.hexdigest()==meta['recoveryImageSHA256']
 with image.open('rb') as inp:
  for offset in range(0,size,1024*1024):
   b=inp.read(min(1024*1024,size-offset))
   if im[at+offset:at+offset+len(b)]!=b:im[at+offset:at+offset+len(b)]=b;changed+=len(b)
 im.flush();digest=hashlib.sha256(im).hexdigest();im.close()
meta.update(sha256=digest,inputRelativePath='EngineProbe/vmapple-'+digest+'.bin',recoveryImageSHA256=observer['imageSHA256'],parentCaptureSHA256=meta['sha256'],guestObserverAdded=True,guestObserverScope='Read-only process/framebuffer/crash-directory collector in Data template LaunchDaemons; no runtime or policy bypass')
h=(src/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0];h+='static const uint8_t vmref_sha256[32]={'+','.join(str(x) for x in bytes.fromhex(digest))+'};\n#define VMREF_INPUT_PATH L'+json.dumps('\\'+meta['inputRelativePath'].replace('/','\\'))+'\n';(d/'vmapple_reference_boot.h').write_text(h);(d/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(meta,indent=2)+'\n');(d/'GUEST-OBSERVER-PREPARATION.json').write_text(json.dumps(observer,indent=2)+'\n');(d/'BOOT-CONTEXT-PREPARATION.json').write_bytes((src/'BOOT-CONTEXT-PREPARATION.json').read_bytes());print(json.dumps({'stagedProfile':str(d),'changedCaptureBytes':changed,'SHA256':digest,'activeBuildRepointed':False},indent=2))
