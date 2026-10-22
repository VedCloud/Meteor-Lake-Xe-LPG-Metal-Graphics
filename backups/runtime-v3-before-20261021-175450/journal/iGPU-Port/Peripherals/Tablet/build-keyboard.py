from pathlib import Path
import subprocess,plistlib
root=Path(__file__).resolve().parent.parent;app=root/'desktop-build/Latitude Tablet Keyboard.app';binary=app/'Contents/MacOS/latitude-tablet-keyboard';binary.parent.mkdir(parents=True,exist_ok=True)
subprocess.run(['xcrun','clang++','-arch','x86_64','-mmacosx-version-min=15.0','-std=c++17','-fobjc-arc','-Wall','-Wextra','-Werror','-framework','Cocoa','-framework','ApplicationServices','-framework','IOKit',str(root/'tablet-sources/latitude-tablet-keyboard.mm'),'-o',str(binary)],check=True)
(app/'Contents/Info.plist').write_bytes(plistlib.dumps({'CFBundleName':'Latitude Tablet Keyboard','CFBundleDisplayName':'Latitude Tablet Keyboard','CFBundleIdentifier':'com.ved.LatitudeTabletKeyboard','CFBundleExecutable':'latitude-tablet-keyboard','CFBundleVersion':'0.2.0','CFBundleShortVersionString':'0.2.0','CFBundlePackageType':'APPL','LSUIElement':True,'LSMinimumSystemVersion':'15.0','NSHighResolutionCapable':True}))
subprocess.run(['codesign','--force','--sign','-',str(app)],check=True);subprocess.run(['codesign','--verify','--strict',str(app)],check=True)
print('PASS signed x86_64 tablet keyboard app; manual panel and gated automatic focus path compiled. Not run or installed on host.')
