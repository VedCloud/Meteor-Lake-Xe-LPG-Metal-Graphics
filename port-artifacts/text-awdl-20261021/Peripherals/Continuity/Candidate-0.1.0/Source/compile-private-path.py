"""Compile real modified Sequoia translation units with CLT, not Xcode mocks."""
from pathlib import Path
import subprocess,json
root=Path(__file__).resolve().parent.parent;w=root/'desktop-build/airport-awdl';out=root/'desktop-build/airport-awdl-objects';out.mkdir(exist_ok=True)
includeDirs=[w/'itl80211/openbsd',w/'itl80211',w/'include',root/'MacKernelSDK/Headers']
includeDirs+=sorted({p.parent for p in w.rglob('*.hpp')}|{p.parent for p in w.rglob('*.h')})
flags=['-arch','x86_64','-mmacosx-version-min=15.2','-mkernel','-fapple-kext','-nostdinc','-fno-exceptions','-fno-rtti','-fno-builtin','-fno-common','-std=gnu++14','-O1','-DKERNEL','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-DDEBUG=1','-DAIRPORT_WCL','-DAIRPORT_VTD','-DAIRPORT','-D__IO80211_TARGET=__MAC_15_2','-D__PRIVATE_SPI__','-DIO80211FAMILY_V2','-DIEEE80211_STA_ONLY','-DITLWM_VERSION="2.4.0-private-awdl"','-DGIT_COMMIT="private"','-Wno-deprecated-declarations','-Wno-nullability-completeness','-Wno-register','-include',str(w/'itlwm/PrivateSPI.pch')]
for p in includeDirs:flags+=['-I',str(p)]
sources=['AirportItlwm/AirportItlwmV2.cpp','itlwm/hal_iwx/ItlIwx.cpp']
if __name__=='__main__':
 for name in sources:
  obj=out/(Path(name).stem+'.o');log=out/(Path(name).stem+'.log')
  with log.open('w') as f:result=subprocess.run(['xcrun','clang++',*flags,'-c',str(w/name),'-o',str(obj)],stdout=f,stderr=subprocess.STDOUT)
  if result.returncode:print(log.read_text()[-12000:]);raise SystemExit(result.returncode)
  print('Compiled actual modified Intel unit:',name,flush=True)
 (out/'COMPILE.json').write_text(json.dumps({'translationUnits':sources,'arch':'x86_64','kernelHeaders':'MacKernelSDK','linkedKext':False,'installed':False,'hardwareTested':False},indent=2)+'\n')
