from pathlib import Path
import subprocess,concurrent.futures
r=Path(__file__).resolve().parent.parent
common=['xcrun','clang','-bundle','-fobjc-arc','-O1','-DNVMTL_RELEASE=1','-Wno-protocol','-Wno-incomplete-implementation','-I','Vulkan-Headers/include']
variants=[('NVMTLDriver-0.3.18',['-arch','x86_64','-mmacosx-version-min=15.0']),('NVMTLDriver-release-policy-test-0.3.18',[]),('NVMTLDriver-fault-test-0.3.18',['-DNVMTL_TEST_RECORD_FAILURE=1'])]
def build(item):
 name,flags=item
 with (r/'desktop-build'/('build-'+name+'.log')).open('w') as log:
  subprocess.run(common+flags+['-o','desktop-build/'+name,'NullMoth/plugin/NVMTLDevice.m','-framework','Foundation','-framework','Metal','-framework','IOSurface','-framework','IOKit'],cwd=r,stdout=log,stderr=subprocess.STDOUT,check=True)
 return name
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
 for name in executor.map(build,variants):print('Compiled',name)
