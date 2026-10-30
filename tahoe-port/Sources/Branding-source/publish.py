from pathlib import Path
import json,shutil,hashlib,subprocess
p=Path(__file__).resolve().parent;project=p.parent;source=p/'Package';h=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();before=p/'Before';before.mkdir(exist_ok=True)
block='''# Branding is an independent, backed-up cosmetic transaction after graphics commits.
# A branding failure is reported without rolling back the installed graphics runtime.
if [ "$MODE" = full ] && [ -f "$HERE/Branding/install-about-this-mac.sh" ];then
 echo 'Installing the bundled Dell About This Mac identity/icon...'
 set +e
 /bin/bash "$HERE/Branding/install-about-this-mac.sh" > >(tee "$REPORT/branding.txt") 2>&1
 branding_result=$?
 set -e
 printf 'branding-exit=%s\\n' "$branding_result" >> "$REPORT/RESULT.txt"
 if [ "$branding_result" -ne 0 ];then
  echo "Graphics installed successfully; About This Mac branding failed with exit=$branding_result. See $REPORT/branding.txt."
  exit "$branding_result"
 fi
fi
'''
(p/'INSTALLER-CALLBACK.sh').write_text(block)
roots=[project/'Tahoe-Combined-Runtime',project/'Candidate/Tahoe-Port/Graphics',Path('/Volumes/macOS Tahoe/Tahoe-Port/Graphics')]
for index,root in enumerate(roots):
 installer=root/'install-windowserver.sh';text=installer.read_text();assert '# Branding is an independent' not in text;shutil.copy2(installer,before/('install-windowserver-'+str(index)+'.sh'));assert text.count('success=1\n')==1;text=text.replace('success=1\n','success=1\n'+block,1);installer.write_text(text)
 shutil.copytree(source,root/'Branding',dirs_exist_ok=True)
 status=json.loads((root/'STATUS.json').read_text());status['AboutThisMac']={'name':'Dell Latitude 5350 2-in-1','detail':'13-inch, 2024','newIconSHA256':h(source/'DellLatitude5350.icns'),'includedInFullInstaller':True,'hardwareVerified':False};(root/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n')
 manifest=root/'SHA256SUMS.txt';paths={line.split('  ',1)[1] for line in manifest.read_text().splitlines() if line.strip()};paths.update(str(f.relative_to(root)) for f in (root/'Branding').rglob('*') if f.is_file() and not f.name.startswith('._'));manifest.write_text(''.join(h(root/f)+'  '+f+'\n' for f in sorted(paths)))
 subprocess.run(['/bin/bash','-n',str(installer)],check=True)
for root in [project/'Candidate/Tahoe-Port',Path('/Volumes/macOS Tahoe/Tahoe-Port')]:
 collector=root/'collect-tahoe.sh';s=collector.read_text();anchor='section plugin-info 5 ';pos=s.index(anchor);s=s[:pos]+'if [ -x "$HERE/Graphics/Branding/latitude-about-this-mac" ];then section about-this-mac 20 "$HERE/Graphics/Branding/latitude-about-this-mac" diagnose "$HERE/Graphics/Branding";fi\n'+s[pos:];collector.write_text(s);subprocess.run(['/bin/bash','-n',str(collector)],check=True)
 f=root/'CANDIDATE-0.3.21.md';s=f.read_text();s+='\n## Custom Dell About This Mac\n\nThe full graphics installer now also applies the supplied TahoeDell ICNS, Dell Latitude 5350 2-in-1 name and 13-inch, 2024 detail. Graphics keeps the live driver name with shared memory underneath. The branding transaction keeps backups and requires a reboot; diagnose output is included in the collector. A branding failure returns its own error after preserving the completed graphics installation. See Graphics/Branding/README.md for standalone install and restore.\n';f.write_text(s)
status={'iconSHA256':h(source/'DellLatitude5350.icns'),'helperSHA256':h(source/'latitude-about-this-mac'),'publishedGraphicsRoots':list(map(str,roots)),'name':'Dell Latitude 5350 2-in-1','detail':'13-inch, 2024','normalPenDriver':'1.0.4','optInTouchpadDriverWithPenFixes':'1.0.5','nibOnDellVerified':False,'graphicsValueSource':'unchanged ANV-backed identity/heap reporting','hardwareBrandingVerified':False,'hostInstalled':False};(p/'PUBLISHED.json').write_text(json.dumps(status,indent=2)+'\n');print('Published custom icon/name bundle and installer/collector integration to the Tahoe USB.')
