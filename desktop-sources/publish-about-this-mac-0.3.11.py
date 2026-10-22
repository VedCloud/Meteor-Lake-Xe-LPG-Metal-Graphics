from pathlib import Path
import hashlib, json, shutil, subprocess
r = Path(__file__).resolve().parent.parent
p = r / 'latitude-desktop'
u = Path('/Volumes/SEQOUIA')
port = u / 'iGPU-Port'
b = port / 'backups/about-this-mac-before-0.3.11'
def sha(f): return hashlib.sha256(f.read_bytes()).hexdigest()
def copy(src, dst):
    dst.parent.mkdir(parents=True, exist_ok=True)
    temp = dst.with_name(dst.name + '.branding-tmp')
    shutil.copy2(src, temp)
    temp.replace(dst)
def verify(base, sums):
    for line in sums.read_text().splitlines():
        digest, name = line.split('  ', 1)
        assert sha(base / name) == digest, name
assert u.is_mount() and not b.exists(), 'USB missing or publication already exists'
verify(port, p / 'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
efi_before = {str(f.relative_to(u/'EFI')):sha(f) for f in (u/'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
frozen = [
 'WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver',
 'WindowServerKernel/NMXGPU.kext/Contents/MacOS/NMXGPU',
 'WindowServerFramebuffer/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB',
 'WindowServerBackend/libvulkan_intel.dylib',
 'WindowServerTransport/libdrm.2.dylib']
prior = {name:sha(port/'Desktop'/name) for name in frozen}
assert prior[frozen[0]] == 'e388688d61a5c7c962a3601a944b8d7711b2f61b62e20349bdcaf7b513358e6f'
assert all(sha(p/name)==digest for name,digest in prior.items())
for name in ['Desktop/install-windowserver.sh','Desktop/README.md','Desktop/BUILD-MANIFEST.json','Desktop/SHA256SUMS.txt','SHA256SUMS.json','PORT-STATUS.md']:
    copy(port/name,b/name)
local_backup=r/'backups/about-this-mac-before-0.3.11'
assert not local_backup.exists()
shutil.copytree(b, local_backup)
for name in ['install-windowserver.sh','install-about-this-mac.sh','restore-about-this-mac.sh']:
    subprocess.run(['/bin/bash','-n',str(p/name)],check=True)
for name in ['AboutThisMac/latitude-about-this-mac','WindowServerPlugin/NVMTLDriver.bundle']:
    subprocess.run(['codesign','--verify','--strict',str(p/name)],check=True)
files=sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name!='SHA256SUMS.txt' and not f.name.startswith('._'))
(p/'SHA256SUMS.txt').write_text(''.join(sha(f)+'  '+str(f.relative_to(p))+'\n' for f in files))
for f in files+[p/'SHA256SUMS.txt']:
    dest=port/'Desktop'/f.relative_to(p)
    if not dest.exists() or sha(dest)!=sha(f): copy(f,dest)
status=port/'PORT-STATUS.md'
status.write_text('# About This Mac added to 0.3.11\n\nSame graphics installer now also stages Dell Latitude 5350 2-in-1 (13-inch, 2024) and the supplied transparent icon. Separate System-volume snapshot transaction and restore script; graphics/EFI unchanged. Target layout/icon/graphics performance remain unverified. See Desktop/AboutThisMac/README.md. Cosmetic failure returns4 with graphics still installed/armed.\n\n---\n\n'+status.read_text())
m=json.loads((port/'SHA256SUMS.json').read_text())
for f in files+[p/'SHA256SUMS.txt']:
    name='Desktop/'+str(f.relative_to(p));m[name]=sha(port/name)
m['PORT-STATUS.md']=sha(status)
(port/'SHA256SUMS.json').write_text(json.dumps(m,indent=2)+'\n')
verify(port/'Desktop',p/'SHA256SUMS.txt')
verify(port,p/'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name,digest in m.items(): assert sha(port/name)==digest, name
assert {name:sha(port/'Desktop'/name) for name in frozen}==prior
assert efi_before=={str(f.relative_to(u/'EFI')):sha(f) for f in (u/'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
for ext in ['icns','png']:
    original=Path('/Users/ved/Downloads')/('1c2ca4ab-ef91-49fd-ae6a-73c24197147d-remover-bg.'+ext)
    assert sha(original)==sha(port/'Desktop/AboutThisMac'/('DellLatitude5350.'+ext))
validation={'published':True,'package_files':len(files),'root_manifest_files':len(m),'graphics_binaries_unchanged':prior,'efi_files_unchanged':len(efi_before),'assets_exact':True,'frozen_rendering_evidence_verified':True,'target_installation_performed':False,'target_snapshot_or_name_verified':False}
(r/'port-artifacts/smoothness-0.3.11/ABOUT-THIS-MAC-VALIDATION.json').write_text(json.dumps(validation,indent=2)+'\n')
print(json.dumps(validation,indent=2))
