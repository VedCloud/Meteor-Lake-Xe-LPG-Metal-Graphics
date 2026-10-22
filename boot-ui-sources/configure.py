import copy
AUDIO_PATH='PciRoot(0x0)/Pci(0x1F,0x3)'
def configure(document):
 d=copy.deepcopy(document);boot=d['Misc']['Boot'];boot.update(PickerMode='External',PickerVariant='Acidanthera\\GoldenGateExt',PickerAttributes=147,ShowPicker=True,HideAuxiliary=True)
 # ResetNvramEntry.efi registers an auxiliary entry; HideAuxiliary exposes it
 # in extended mode (Spacebar), without disabling NVRAM recovery.
 audio=d['UEFI']['Audio'];audio.update(AudioSupport=True,AudioDevice=AUDIO_PATH,AudioCodec=0,AudioOutMask=-1,PlayChime='Enabled')
 # Keep the prior conservative gain ceiling. Do not override the user's NVRAM
 # system volume or deliberately unmute a muted system on every boot.
 for name in ['AudioDxe.efi','OpenCanopy.efi']:
  matches=[e for e in d['UEFI']['Drivers'] if e['Path']==name]
  if matches:assert len(matches)==1;matches[0]['Enabled']=True
  else:d['UEFI']['Drivers'].append({'Arguments':'','Comment':'Matching OpenCore 1.0.9 '+name,'Enabled':True,'HideVerbose':False,'LoadEarly':False,'Path':name})
 return d
