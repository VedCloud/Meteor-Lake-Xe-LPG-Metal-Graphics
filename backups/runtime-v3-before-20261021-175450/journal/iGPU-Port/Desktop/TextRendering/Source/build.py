from pathlib import Path
import subprocess,json,hashlib
root=Path(__file__).resolve().parent.parent;s=root/'subpixel-sources';ft=root/'desktop-build/freetype-static';out=root/'desktop-build/latitude-lcd';out.mkdir(exist_ok=True)
for arch in ['arm64','x86_64']:
 flags=['-arch',arch,'-mmacosx-version-min=15.0','-std=c++17','-O2','-Wall','-Wextra','-Werror','-I'+str(ft/'include'),'-I'+str(s)]
 binary=out/('test-lcd-'+arch);subprocess.run(['xcrun','clang++',*flags,*(['-fsanitize=address,undefined','-g'] if arch=='arm64' else []),str(s/'LatitudeLCD.cpp'),str(s/'test-lcd.cpp'),str(ft/arch/'libfreetype.a'),'-o',str(binary)],check=True)
 if arch=='arm64':subprocess.run([str(binary),'/System/Library/Fonts/Supplemental/Arial.ttf'],check=True)
 obj=out/('LatitudeLCD-'+arch+'.o');subprocess.run(['xcrun','clang++',*flags,'-c',str(s/'LatitudeLCD.cpp'),'-o',str(obj)],check=True);subprocess.run(['xcrun','libtool','-static','-o',str(out/('libLatitudeLCD-'+arch+'.a')),str(obj),str(ft/arch/'libfreetype.a')],check=True,capture_output=True)
for arch in ['arm64','x86_64']:
 flags=['-arch',arch,'-mmacosx-version-min=15.0','-std=c++17','-O2','-Wall','-Wextra','-Werror','-fobjc-arc','-I'+str(ft/'include')]
 libs=[str(ft/arch/'libfreetype.a'),'-framework','Foundation','-framework','CoreText','-framework','CoreGraphics']
 target=out/('libLatitudeLCDCoreText-'+arch+'.dylib');subprocess.run(['xcrun','clang++',*flags,'-dynamiclib',str(s/'LatitudeLCD.cpp'),str(s/'LatitudeLCDCoreText.mm'),*libs,'-o',str(target)],check=True)
 subprocess.run(['codesign','--force','--sign','-',str(target)],check=True);subprocess.run(['codesign','--verify','--strict',str(target)],check=True)
 if arch=='arm64':
  test=out/'test-coretext';subprocess.run(['xcrun','clang++',*flags,'-fsanitize=address,undefined',str(s/'LatitudeLCD.cpp'),str(s/'LatitudeLCDCoreText.mm'),str(s/'test-coretext.mm'),*libs,'-o',str(test)],check=True);subprocess.run([str(test)],check=True)
(out/'VALIDATION.json').write_text(json.dumps({'FreeType':'2.14.3','FTLCDFilterDefault':True,'cpuReferenceTests':'passed ASan/UBSan on ARM host','x86Compilation':'passed','glyphAtlasBytes':4194304,'MetalShader':'source supplied; target compiler/runtime validation pending','CoreTextShapedGlyphFrontend':'implemented; actual CoreText runs passed ASan/UBSan on ARM host','systemwideInterception':'not implemented','macOSSystemAppsLCD':'not established'},indent=2)+'\n')
