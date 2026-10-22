from pathlib import Path
import plistlib,hashlib,json,subprocess,sys
root=Path(__file__).resolve().parent.parent;sys.path.insert(0,str(root/'boot-ui-sources'));from configure import configure
out=root/'port-artifacts/unified-20261021';out.mkdir(exist_ok=True)
# Patch only the exact HIDD _STA method; retain firmware DSDT bytes so manual
# recovery can disable both the rename and its replacement SSDT.
sys.path.insert(0,str(root/'tablet-sources'));import importlib.util
spec=importlib.util.spec_from_file_location('visibility',root/'tablet-sources/prepare-visibility.py');vis=importlib.util.module_from_spec(spec);spec.loader.exec_module(vis)
patches=[]
for profile in ['Base','Touchpad']:
 data=(Path('/Volumes/SEQOUIA/iGPU-Port/Peripherals')/profile/'DSDT.aml').read_bytes();modified,at=vis.patch(data);find=data[at-2:at+18];replace=modified[at-2:at+18];assert data.count(find)==1 and len(find)==20
 patches.append((find,replace))
assert patches[0]==patches[1];find,replace=patches[0]
patch={'Base':'','BaseSkip':0,'Comment':'Latitude HIDD only: original _STA -> XSTA for scoped tablet visibility SSDT','Count':1,'Enabled':True,'Find':find,'Limit':0,'Mask':b'','OemTableId':b'\0'*8,'Replace':replace,'ReplaceMask':b'','Skip':0,'TableLength':0,'TableSignature':b'DSDT'}
(out/'HIDD-PATCH.plist').write_bytes(plistlib.dumps(patch))
for profile in ['base','touchpad']:
 for mode in ['accelerated','recovery']:
  old=Path('/Volumes/SEQOUIA/iGPU-Port/GraphicsModes')/(profile+'-'+mode+'.plist');doc=configure(plistlib.loads(old.read_bytes()));enabled=mode=='accelerated'
  assert not any(x['Path']=='SSDT-LAT-TABLET.aml' for x in doc['ACPI']['Add'])
  doc['ACPI']['Add'].append({'Comment':'Scoped HIDD visibility and firmware mode reader','Enabled':enabled,'Path':'SSDT-LAT-TABLET.aml'})
  entry=dict(patch);entry['Enabled']=enabled;doc['ACPI']['Patch'].append(entry)
  reference=next(x for x in doc['Kernel']['Add'] if x['BundlePath']=='LatitudeTabletObserver.kext');state=dict(reference);state.update(BundlePath='LatitudeTabletState.kext',ExecutablePath='Contents/MacOS/LatitudeTabletState',Comment='Live HIDD mode reader 0.2.0; no input suppression or sensor DMA',Enabled=enabled);doc['Kernel']['Add'].append(state)
  dest=out/(profile+'-'+mode+'.plist');dest.write_bytes(plistlib.dumps(doc));result=subprocess.run([str(root/'research/OpenCore-release/matching/Utilities/ocvalidate/ocvalidate'),str(dest)],capture_output=True,text=True);assert result.returncode==0,result.stdout+result.stderr
(out/'BOOT-UI-PROVENANCE.json').write_text(json.dumps({'theme':'https://github.com/HJebbour/GoldenGateExt-OpenCore-Theme','themeCommit':subprocess.check_output(['git','-C',str(root/'research/GoldenGateExt'),'rev-parse','HEAD'],text=True).strip(),'OpenCoreExactActiveBuild':'81ecf1a7e75c318dfc859ddabe5b91f4af757231','coreLoaderChanged':False,'chimeFileSHA256':hashlib.sha256((root/'research/OpenCore-release/OCEFIAudio_VoiceOver_Boot.mp3').read_bytes()).hexdigest(),'hdaController':'PciRoot(0x0)/Pci(0x1F,0x3)','allCodecOutputsEnabled':True,'gainCeilingDB':-15,'hardwarePlaybackValidated':False},indent=2)+'\n')
print('PASS all four unified profiles: exact scoped HIDD patch, live-state injection only in normal mode, theme, hidden auxiliaries and chime. Recovery disables the HIDD change/state driver; firmware DSDT bytes remain unchanged.')
