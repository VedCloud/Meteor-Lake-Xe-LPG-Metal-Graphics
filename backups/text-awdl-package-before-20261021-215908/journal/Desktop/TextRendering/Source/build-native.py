"""Build real CTLineDraw interposition; run only our own host fixture."""
from pathlib import Path
import subprocess,os,json,hashlib
root=Path(__file__).resolve().parent.parent;s=root/'subpixel-sources';ft=root/'desktop-build/freetype-static';out=root/'desktop-build/latitude-lcd';out.mkdir(exist_ok=True)
common=['-mmacosx-version-min=15.0','-std=c++17','-O1','-g','-Wall','-Wextra','-Werror','-fobjc-arc','-I'+str(ft/'include')]
frameworks=['-framework','Foundation','-framework','CoreText','-framework','CoreGraphics']
sources=[str(s/n) for n in ['LatitudeLCD.cpp','LatitudeLCDCoreText.mm','LatitudeLCDInterpose.mm']]
for arch in ['arm64','x86_64']:
 for sanitizer in ([True,False] if arch=='arm64' else [False]):
  flags=['-arch',arch,*common,*(['-fsanitize=address,undefined'] if sanitizer else [])]
  suffix=arch+('-asan' if sanitizer else '');library=out/('libLatitudeLCDNative-'+suffix+'.dylib')
  subprocess.run(['xcrun','clang++',*flags,'-dynamiclib',*sources,str(ft/arch/'libfreetype.a'),*frameworks,'-o',str(library)],check=True)
  subprocess.run(['codesign','--force','--sign','-',str(library)],check=True);subprocess.run(['codesign','--verify','--strict',str(library)],check=True)
  test=out/('test-interpose-'+suffix)
  subprocess.run(['xcrun','clang++',*flags,str(s/'test-interpose.mm'),*frameworks,'-o',str(test)],check=True)
  if arch=='arm64':
   env=dict(os.environ);env['DYLD_INSERT_LIBRARIES']=str(library);subprocess.run([str(test)],env=env,check=True)
report={'nativeCTLineDrawInterception':'passed in ordinary host caller, sanitized and release','clipAndContextLifetime':'passed','IntelBuild':'compiled and signed; Dell execution pending','scope':'process-local opaque sRGB CPU bitmap contexts only','systemAppsIntegrated':False,'transparentCoreAnimationLayersIntegrated':False,'globalInjectionInstalled':False}
report['sha256']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in out.glob('*') if p.name in ['libLatitudeLCDNative-x86_64.dylib','test-interpose-x86_64']}
(out/'NATIVE-VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n')
