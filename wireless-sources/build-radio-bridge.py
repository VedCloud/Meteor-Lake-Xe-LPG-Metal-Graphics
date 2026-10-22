from pathlib import Path
import subprocess,json,hashlib,os
r=Path(__file__).resolve().parent.parent;s=r/'wireless-sources';owl=r/'research/owl';out=r/'desktop-build/awdl-radio-helper';out.mkdir(exist_ok=True)
assert subprocess.check_output(['git','-C',str(owl),'rev-parse','HEAD'],text=True).strip()=='0ee3263f944ea721c355f826aa90d636d012a512'
subprocess.run(['python3',str(s/'prepare-radio-engine.py')],check=True)
owl=r/'desktop-build/awdl-radio-engine-source'

common=['-mmacosx-version-min=15.0','-std=c++17','-O1','-g','-Wall','-Wextra','-I'+str(s),'-I'+str(owl/'src'),'-I'+str(owl/'radiotap')]
for arch in ['arm64','x86_64']:
 d=out/arch;d.mkdir(exist_ok=True);objects=[]
 for source in list((owl/'src').glob('*.c'))+[owl/'radiotap/radiotap.c']:
  obj=d/(source.stem+'.o');objects.append(str(obj))
  subprocess.run(['xcrun','clang','-arch',arch,'-mmacosx-version-min=15.0','-std=gnu11','-O1','-I'+str(owl/'src'),'-I'+str(owl/'radiotap'),'-DRADIOTAP_SUPPORT_OVERRIDES','-c',str(source),'-o',str(obj)],check=True,capture_output=True)
 subprocess.run(['xcrun','libtool','-static','-o',str(d/'libLatitudeAWDLRadioEngine.a'),*objects],check=True,capture_output=True)
 binary=out/('latitude-awdl-radio-'+arch)
 subprocess.run(['xcrun','clang++','-arch',arch,*common,str(s/'latitude-awdl-radio.cpp'),str(out/arch/'libLatitudeAWDLRadioEngine.a'),'-framework','IOKit','-framework','CoreFoundation','-o',str(binary)],check=True)
 subprocess.run(['codesign','--force','--sign','-',str(binary)],check=True)
 subprocess.run(['codesign','--verify','--strict',str(binary)],check=True)
 # Build OWL itself with sanitizers so malformed peer input is also checked.
d=out/'sanitized-objects';d.mkdir(exist_ok=True);objects=[]
for source in list((owl/'src').glob('*.c'))+[owl/'radiotap/radiotap.c']:
 obj=d/(source.stem+'.o');objects.append(str(obj))
 subprocess.run(['xcrun','clang','-arch','arm64','-mmacosx-version-min=15.0','-std=gnu11','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(owl/'src'),'-I'+str(owl/'radiotap'),'-DRADIOTAP_SUPPORT_OVERRIDES','-c',str(source),'-o',str(obj)],check=True,capture_output=True)
test=out/'test-radio-asan'
subprocess.run(['xcrun','clang++','-arch','arm64',*common,'-fsanitize=address,undefined','-fno-sanitize-recover=all',str(s/'test-awdl-radio.cpp'),*objects,'-o',str(test)],check=True)
subprocess.run([str(test)],env=dict(os.environ,UBSAN_OPTIONS='halt_on_error=1'),check=True)
# Bad invocation must return without opening any host controller.
for args in [[],['bogus'],['listen','-1'],['send-psf'],['listen','301']]:
 assert subprocess.run([str(out/'latitude-awdl-radio-arm64'),*args],capture_output=True).returncode==2
report={'helper':'current-channel root-only IOUserClient to real Intel HAL','IntelCompilation':'passed and signed','ASanUBSan':'actual queue, parser, PSF/MIF engine-to-parser, peer lifecycle and truncation sweep passed','upstreamWireFix':'memcpy-based unaligned reads/writes and overflow-safe bounds; strict sanitizer recovery disabled','automaticTransmission':False,'hostRadioOpened':False,'DellHardwareValidated':False,'AirDropFunctional':False,'sourceSHA256':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in s.glob('*') if p.is_file() and p.name in ['LatitudeAWDLRadio.h','LatitudeAWDLEngineBridge.h','latitude-awdl-radio.cpp','LatitudeAWDLUserClient.inc','test-awdl-radio.cpp']}}
(out/'VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n')
