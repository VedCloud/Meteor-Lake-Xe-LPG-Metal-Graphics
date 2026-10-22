from pathlib import Path
import plistlib,json,hashlib,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop/Audio';u=Path('/Volumes/SEQOUIA/EFI/OC')
old=plistlib.loads((u/'config.plist').read_bytes());new=plistlib.loads((p/'config.plist').read_bytes());entry=new['Kernel']['Add'].pop();props=new['DeviceProperties']['Add'].pop('PciRoot(0x0)/Pci(0x1F,0x3)');assert new==old
assert entry['Enabled'] and entry['BundlePath']=='AppleALC.kext' and entry['ExecutablePath']=='Contents/MacOS/AppleALC'
assert props=={'device-id':bytes.fromhex('ca510000'),'layout-id':(33).to_bytes(4,'little'),'alc-layout-id':(33).to_bytes(4,'little')}
resources=plistlib.loads((r/'research/applealc-evidence/alc295.plist').read_bytes());assert resources['CodecID']==0x295
layout=next(x for x in resources['Files']['Layouts'] if x['Id']==33);assert 'Latitude 7210' in layout['Comment']
controllers=plistlib.loads((r/'research/applealc-evidence/controllers.plist').read_bytes());assert any(x.get('Vendor')=='Intel' and x.get('Device')==0x51ca for x in controllers)
release=json.loads((r/'research/applealc-evidence/release.json').read_text());asset=next(a for a in release['assets'] if a['name'].endswith('-RELEASE.zip'));assert asset['digest']=='sha256:'+hashlib.sha256((r/'research/applealc-evidence'/asset['name']).read_bytes()).hexdigest()
subprocess.run(['codesign','--verify','--strict',str(p/'AppleALC.kext')],check=True)
subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(p/'config.plist')],check=True)
print('PASS official AppleALC release digest/signature; existing Lilu; ALC295 layout33 resource/controller51CA supported by upstream; only audio properties and one kext entry change. Physical codec supplied by user; sound/pin routing unverified.')
