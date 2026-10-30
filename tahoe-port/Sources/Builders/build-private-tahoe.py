"""Rebuild the complete pinned Sequoia kext with CLT, without installing it."""
from pathlib import Path
import runpy,subprocess,json,concurrent.futures,hashlib,plistlib,shutil,os
env=runpy.run_path(str(Path(__file__).with_name('compile-tahoe.py')))
root,w,out,flags=[env[k] for k in ['root','w','out','flags']]
shutil.copy2(root/'wireless-sources/LatitudeAWDLFrame.h',w/'include/LatitudeAWDLFrame.h')
subprocess.run(['python3',str(w/'scripts/zlib_compress_fw.py'),str(w/'include/FwBinary.cpp'),str(w/'itlwm/firmware')],check=True)
project=json.loads(subprocess.check_output(['plutil','-convert','json','-o','-',str(w/'itlwm.xcodeproj/project.pbxproj')]))
o=project['objects'];paths={}
def walk(id,parent):
 d=o[id];path=(w if d.get('sourceTree')=='SOURCE_ROOT' else parent)/d.get('path','');paths[id]=path
 for child in d.get('children',[]):walk(child,path)
walk(o[project['rootObject']]['mainGroup'],w)
target=next(d for d in o.values() if d.get('isa')=='PBXNativeTarget' and d.get('name')=='AirportItlwm-Tahoe')
sources=[];empty=[]
for phase in target['buildPhases']:
 if o[phase]['isa']=='PBXSourcesBuildPhase':
  for id in o[phase]['files']:
   if 'fileRef' not in o[id]:empty.append(id);continue
   ref=o[id]['fileRef'];path=paths.get(ref)
   if path is None:
    candidates=list(w.rglob(o[ref]['path']));assert len(candidates)==1;path=candidates[0]
   assert path.is_file(),path;sources.append(path)
assert len(sources)==74 and empty==['F8D94CCE2B9ABFE20081A3C4']
objects=[];failure=[]
def compile(source):
 key=hashlib.sha256(str(source.relative_to(w)).encode()).hexdigest()[:12]+'-'+source.stem
 obj=out/(key+'.o');log=out/(key+'.log')
 with log.open('w') as f:r=subprocess.run(['xcrun','clang++',*flags,'-x','c++','-c',str(source),'-o',str(obj)],stdout=f,stderr=subprocess.STDOUT)
 if r.returncode:return source,log,None
 return source,log,obj
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
 for i,(source,log,obj) in enumerate(pool.map(compile,sources),1):
  if obj:objects.append(obj)
  else:failure.append(log);print('FAILED',source.relative_to(w),flush=True)
  if i%15==0:print('Compiled',i,'/',len(sources),'units',flush=True)
if failure:
 for log in failure:print(log,log.read_text()[-5000:])
 raise SystemExit(1)
kmod=out/'kmod.cpp';kmod.write_text('''#include <mach/kmod.h>
extern "C" {extern kmod_start_func_t _start;extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL(com.zxystd.AirportItlwm,"2.4.0",_start,_stop)
kmod_start_func_t *_realmain=0;kmod_stop_func_t *_antimain=0;int _kext_apple_cc=__APPLE_CC__;}
''')
ko=out/'kmod.o';subprocess.run(['xcrun','clang++',*flags,'-c',str(kmod),'-o',str(ko)],check=True)
k=out/'AirportItlwm.kext';(k/'Contents/MacOS').mkdir(parents=True,exist_ok=True);binary=k/'Contents/MacOS/AirportItlwm'
subprocess.run(['xcrun','clang++','-arch','x86_64','-fapple-kext','-nostdlib','-Xlinker','-kext',*[str(p) for p in objects],str(ko),str(root/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(binary)],check=True)
d=plistlib.loads((w/'AirportItlwm/WCL/Info.plist').read_bytes())
substitutions={'DEVELOPMENT_LANGUAGE':'en','EXECUTABLE_NAME':'AirportItlwm','PRODUCT_BUNDLE_IDENTIFIER':'com.zxystd.AirportItlwm','PRODUCT_NAME':'AirportItlwm','PRODUCT_BUNDLE_PACKAGE_TYPE':'KEXT','MODULE_VERSION':'2.4.0'}
def expand(x):
 if isinstance(x,str):
  for a,b in substitutions.items():x=x.replace('$('+a+')',b)
  assert '$(' not in x;return x
 if isinstance(x,dict):return {k:expand(v) for k,v in x.items()}
 if isinstance(x,list):return [expand(v) for v in x]
 return x
d=expand(d);d['LatitudeAWDLPrivateRevision']=os.environ.get('LATITUDE_AIRPORT_REVISION','0.1.0-current-channel-action');d['LatitudeAWDLAirDropQualified']=False;d['LatitudeTargetDarwin']='25.x';d['LatitudeTahoeHardwareQualified']=False
(k/'Contents/Info.plist').write_bytes(plistlib.dumps(d))
subprocess.run(['codesign','--force','--sign','-',str(k)],check=True);subprocess.run(['codesign','--verify','--strict',str(k)],check=True)
symbols=subprocess.check_output(['xcrun','nm',str(binary)],text=True)
assert 'latitudeAWDLCurrentChannel' in symbols and 'latitudeAWDLRawGated' in symbols
report={'base':'d695c971420cc8af0daa286dcd3297e8236c0a63','MTLfixes':'05b19dc81db6aedcf4058525981f7326f57c04e2','unitsCompiled':len(sources),'emptyUpstreamBuildReference':empty,'privateRevision':d['LatitudeAWDLPrivateRevision'],'signature':'verified','binarySHA256':hashlib.sha256(binary.read_bytes()).hexdigest(),'experimentalGate':'latitude-awdl-current-channel=1','automaticRadioTransmission':False,'installed':False,'hardwareValidated':False,'target':'macOS Tahoe 26 / Darwin25 x86_64','abi':'__MAC_26_0','AirDropFunctional':False,'remaining':['firmware qualification of current-channel action path','Dell qualification of correlated firmware TX status and reset cleanup; receive demultiplexing','off-channel scheduling and secondary MAC/stations','native Skywalk peer data, IPv6/mDNS, AirDrop transfer']}
if d['LatitudeAWDLPrivateRevision']=='0.1.4-native-backend-integration':
 report.update({
  'experimentalGate':'latitude-awdl-native=1',
  'nativeBackendCompiled':True,
  'nativeFrontendConnected':False,
  'nativeEntryPointCallerPresent':False,
  'remaining':[
   'Dell qualification of the selected 0.1.2 root radio bridge',
   'native AWDL interface factory and typed control handlers',
   'validated native action-frame input metadata and peer discovery',
   'native packet dequeue worker and completion-status adapter',
   'runtime grant/deadline timers and peer channel negotiation',
   'firmware qualification of shared secondary PHY/P2P/listener contexts',
   'bidirectional native IPv6/mDNS and a verified AirDrop file transfer']})
(out/'BUILD.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS complete private Intel kext compiled, linked and signed; default radio gate off, not installed.',flush=True)
