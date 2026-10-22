from pathlib import Path
import subprocess
r = Path(__file__).resolve().parent.parent
out = r / 'latitude-desktop/AboutThisMac/latitude-about-this-mac'
subprocess.run(['xcrun', 'clang', '-arch', 'x86_64', '-arch', 'arm64', '-mmacosx-version-min=15.0', '-fobjc-arc', '-fblocks', '-Wall', '-Wextra', '-Werror', '-framework', 'Foundation', '-framework', 'IOKit', '-framework', 'ImageIO', '-framework', 'CoreGraphics', str(r / 'desktop-sources/latitude-about-this-mac.m'), '-o', str(out)], check=True)
subprocess.run(['codesign', '--force', '--sign', '-', str(out)], check=True)
subprocess.run(['codesign', '--verify', '--strict', str(out)], check=True)
for prefix in [[], ['arch', '-x86_64']]:
    subprocess.run(prefix + [str(out), '--self-test'], check=True)
