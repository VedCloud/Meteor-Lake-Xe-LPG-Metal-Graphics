from pathlib import Path
import shutil,plistlib,hashlib,json,subprocess,copy
b=Path(__file__).resolve().parent;a=b/'Audio';a.mkdir(exist_ok=True);up=b/'Audio-build/Release/SimpleHDA-Tahoe-v1.0.0-RELEASE';g=b/'Tahoe-Graphics-Runtime'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
for name in ['SimpleHDA.kext','Licenses']:
 shutil.copytree(up/name,a/name,dirs_exist_ok=True)
# The vendor release is unsigned. Keep its zip/hash provenance and sign the local injection copy for package verification.
subprocess.run(['codesign','--force','-s','-',str(a/'SimpleHDA.kext')],check=True);subprocess.run(['codesign','--verify','--strict',str(a/'SimpleHDA.kext')],check=True)
for f in ['SimpleHDA-HAL-1.0.0.pkg','README.en.md','NOTICE.md']:shutil.copy2(up/f,a/f)
for f in ['ocvalidate','resolve-usb-efi.sh','USB-EFI-UUID.txt']:shutil.copy2(g/f,a/f)
(a/'Profiles').mkdir(exist_ok=True);mappings={};old={};new={}
for mode,p in [('base',b/'Candidate/EFI/OC/config.plist'),('accelerated',g/'Profiles/accelerated.plist'),('recovery',g/'Profiles/recovery.plist')]:
 d=plistlib.loads(p.read_bytes());original=copy.deepcopy(d);old[mode]=sha(p)
 entries=d['Kernel']['Add'];lilu=next(i for i,k in enumerate(entries) if k['BundlePath']=='Lilu.kext');idx=next(i for i,k in enumerate(entries) if k['BundlePath']=='AppleALC.kext');assert lilu<idx
 entries[idx]['MaxKernel']='25.99.99';entries[idx]['Comment']='Confirmed Dell ALC295 layout33; native AppleHDA restored on Tahoe by SimpleHDA'
 assert not any(k['BundlePath']=='SimpleHDA.kext' for k in entries)
 entries.insert(idx+1,dict(Arch='x86_64',BundlePath='SimpleHDA.kext',Comment='SimpleHDA1.0.0 native HDA restore; Darwin25 only; after AppleALC',Enabled=True,ExecutablePath='Contents/MacOS/SimpleHDA',MaxKernel='25.99.99',MinKernel='25.0.0',PlistPath='Contents/Info.plist'))
 guid='7C436110-AB2A-4BBB-A880-FE41995C9F82';args=d['NVRAM']['Add'][guid]['boot-args'].split();assert 'keepsyms=1' in args
 if not any(s.startswith('shda-lang=') for s in args):args.append('shda-lang=en_GB')
 d['NVRAM']['Add'][guid]['boot-args']=' '.join(args)
 props=d['DeviceProperties']['Add']['PciRoot(0x0)/Pci(0x1F,0x3)'];assert props['layout-id']==bytes.fromhex('21000000') and props['alc-layout-id']==bytes.fromhex('21000000') and props['device-id']==bytes.fromhex('ca510000')
 assert d['DeviceProperties']==original['DeviceProperties'] and d['ACPI']==original['ACPI'] and d['PlatformInfo']==original['PlatformInfo']
 target=a/'Profiles'/(mode+'.plist');target.write_bytes(plistlib.dumps(d,sort_keys=False));new[mode]=sha(target);mappings[old[mode]]=mode;mappings[new[mode]]=mode
 subprocess.run([str(a/'ocvalidate'),str(target)],stdout=subprocess.DEVNULL,check=True)
(a/'PROFILE-MAP.txt').write_text(''.join(h+' '+mode+'\n' for h,mode in sorted(mappings.items())))
# Overlay the graphics mode profiles/allowlist only after the audio apply transaction has copied the actual EFI kext.
over=a/'Graphics-overlay';(over/'Profiles').mkdir(parents=True,exist_ok=True)
for mode in ['accelerated','recovery']:shutil.copy2(a/'Profiles'/(mode+'.plist'),over/'Profiles'/(mode+'.plist'))
(over/'ALLOWED-CONFIG-SHA256.txt').write_text('\n'.join(sorted(mappings))+'\n')
sums={line.split(None,1)[1].strip():line.split(None,1)[0] for line in (g/'SHA256SUMS.txt').read_text().splitlines()}
changed=['Profiles/accelerated.plist','Profiles/recovery.plist','ALLOWED-CONFIG-SHA256.txt'];check={}
for name in changed:check[name]={'before':sha(g/name),'after':sha(over/name)};sums[name]=sha(over/name)
(over/'SHA256SUMS.txt').write_text(''.join(h+'  '+name+'\n' for name,h in sorted(sums.items())))
check['SHA256SUMS.txt']={'before':sha(g/'SHA256SUMS.txt'),'after':sha(over/'SHA256SUMS.txt')}
(a/'GRAPHICS-OVERLAY-HASHES.json').write_text(json.dumps(check,indent=2)+'\n')
(a/'OVERLAY-MAP.txt').write_text(''.join(v['before']+' '+v['after']+' '+name+'\n' for name,v in check.items()))
(a/'PROVENANCE.json').write_text(json.dumps({'project':'https://github.com/laobamac/SimpleHDA','release':'v1.0.0','upstreamArchiveSHA256':'89198256664078c91155d673ee7cebb08134dad1b71d5e34b0aa4ef0d4877f8f','sourceCommit':'7517b04bb3e59306550f3c678e809e6494e5dd4b','originalKextExecutableSHA256':'f74c9dbddf43266c97c5f72cd76498bce1d6b5bd3731a947be62f1378b4c7954','upstreamSignature':'unsigned','localCopy':'ad-hoc signed; original release preserved on Corsair','localKextExecutableSHA256':sha(a/'SimpleHDA.kext/Contents/MacOS/SimpleHDA'),'audioCodec':'Confirmed ALC295','layoutID':33,'controllerCompatibilityID':'51ca (retained from working Sequoia)','kernel':'Darwin25 only','OSSystemFilesChanged':False,'liveDellAudioVerified':False,'USBPublished':False},indent=2)+'\n')
shutil.copy2(b/'Audio-build/AUDIT.json',a/'KERNEL-AUDIT.json');print('Audio profiles prepared: layout33 and controller mapping retained; matching OC1.0.9 validation passed.')
