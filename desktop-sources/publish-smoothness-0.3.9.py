from pathlib import Path
import hashlib, json, shutil, subprocess

r = Path(__file__).resolve().parent.parent
p = r / 'latitude-desktop'
u = Path('/Volumes/SEQOUIA')
port = u / 'iGPU-Port'
backup = port / 'backups/smoothness-before-0.3.9'

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def copy(src, dst):
    dst.parent.mkdir(parents=True, exist_ok=True)
    temp = dst.with_name(dst.name + '.publish-tmp')
    shutil.copy2(src, temp)
    temp.replace(dst)

def verify(root, manifest):
    for line in manifest.read_text().splitlines():
        digest, name = line.split('  ', 1)
        assert sha(root / name) == digest, name

verify(port, p / 'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
old_plugin = port / 'Desktop/WindowServerPlugin/NVMTLDriver.bundle/Contents/MacOS/NVMTLDriver'
assert sha(old_plugin) == 'bb3d4644787e3fd101975420da3eb35eaee0f6eff28e349c4880f7dbdbcdc425'
saved_input = port / 'backups/backend4g-touchpad-before-20261020/EFI/OC'
for name in ['config.plist', 'ACPI/DSDT.aml']:
    assert (u / 'EFI/OC' / name).read_bytes() == (saved_input / name).read_bytes()
assert not (u / 'EFI/OC/Kexts/LatitudeI2CMeteorLake.kext').exists()
efi_before = {str(f.relative_to(u / 'EFI')): sha(f) for f in (u / 'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}

for name in ['WindowServerPlugin', 'Library/NullMoth/load-panel-framebuffer.sh', 'install-windowserver.sh', 'README.md', 'BUILD-MANIFEST.json', 'SHA256SUMS.txt']:
    src = port / 'Desktop' / name
    dst = backup / 'Desktop' / name
    assert not dst.exists(), str(dst)
    if src.is_dir():
        shutil.copytree(src, dst)
    else:
        copy(src, dst)
for name in ['PORT-STATUS.md', 'SHA256SUMS.json']:
    copy(port / name, backup / name)

for path in [p / 'WindowServerPlugin/NVMTLDriver.bundle']:
    subprocess.run(['codesign', '--verify', '--strict', str(path)], check=True)
for path in [p / 'install-windowserver.sh', p / 'Library/NullMoth/load-panel-framebuffer.sh', r / 'desktop-sources/collect-desktop-motion.sh']:
    subprocess.run(['/bin/bash', '-n', str(path)], check=True)

files = sorted(f for f in p.rglob('*') if f.is_file() and not f.is_symlink() and 'reports' not in f.relative_to(p).parts and f.name != 'SHA256SUMS.txt' and not f.name.startswith('._'))
(p / 'SHA256SUMS.txt').write_text(''.join(sha(f) + '  ' + str(f.relative_to(p)) + '\n' for f in files))
for f in files + [p / 'SHA256SUMS.txt']:
    dst = port / 'Desktop' / f.relative_to(p)
    if not dst.exists() or sha(dst) != sha(f):
        copy(f, dst)
for src, name in [(r / 'desktop-sources/collect-desktop-motion.sh', 'collect-desktop-motion.sh'), (r / 'research/smoothness-0.3.9.md', 'smoothness-0.3.9.md'), (r / 'port-artifacts/smoothness-0.3.9/VALIDATION.json', 'smoothness-0.3.9-validation.json')]:
    copy(src, port / name)
status = port / 'PORT-STATUS.md'
status.write_text((r / 'research/smoothness-0.3.9.md').read_text() + '\n---\n\n' + status.read_text())
manifest = port / 'SHA256SUMS.json'
m = json.loads(manifest.read_text())
for f in files + [p / 'SHA256SUMS.txt']:
    name = 'Desktop/' + str(f.relative_to(p))
    m[name] = sha(port / name)
for name in ['collect-desktop-motion.sh', 'smoothness-0.3.9.md', 'smoothness-0.3.9-validation.json', 'PORT-STATUS.md']:
    m[name] = sha(port / name)
manifest.write_text(json.dumps(m, indent=2) + '\n')
verify(port / 'Desktop', p / 'SHA256SUMS.txt')
verify(port, p / 'WINDOWSERVER-BASELINE-SHA256SUMS.txt')
for name, digest in m.items():
    assert sha(port / name) == digest, name
efi_after = {str(f.relative_to(u / 'EFI')): sha(f) for f in (u / 'EFI').rglob('*') if f.is_file() and not f.name.startswith('._')}
assert efi_after == efi_before
print('Published smoothness0.3.9; signatures/manifests/frozen rendering evidence verified. EFI and restored input unchanged. No target installation, arming or reboot.')
