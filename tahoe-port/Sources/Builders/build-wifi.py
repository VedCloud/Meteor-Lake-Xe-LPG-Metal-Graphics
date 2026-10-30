from pathlib import Path
import shutil,hashlib,json,subprocess
r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');out=Path(__file__).resolve().parent
src=r/'desktop-build/airport-awdl-radio';w=out/'Network-source';w.mkdir(exist_ok=True)
shutil.copytree(src,w,dirs_exist_ok=True,ignore=shutil.ignore_patterns('.git','build','DerivedData','MacKernelSDK'))
# Preserve the local Meteor Lake firmware/PNVM and root-radio changes; select real Tahoe ABI and target.
script=(r/'wireless-sources/build-private-radio.py').read_text()
script=script.replace("env=runpy.run_path(str(Path(__file__).with_name('compile-private-path.py')))","env=runpy.run_path(str(Path(__file__).with_name('compile-tahoe.py')))")
script=script.replace("'AirportItlwm-Sequoia'","'AirportItlwm-Tahoe'").replace("w/'AirportItlwm/WCL/Sequoia-Info.plist'","w/'AirportItlwm/WCL/Info.plist'")
script=script.replace("d['LatitudeAWDLAirDropQualified']=False","d['LatitudeAWDLAirDropQualified']=False;d['LatitudeTargetDarwin']='25.x';d['LatitudeTahoeHardwareQualified']=False")
script=script.replace("'hardwareValidated':False","'hardwareValidated':False,'target':'macOS Tahoe 26 / Darwin25 x86_64','abi':'__MAC_26_0'")
(out/'build-private-tahoe.py').write_text(script)
compile=(r/'wireless-sources/compile-private-path.py').read_text().replace("root=Path(__file__).resolve().parent.parent;", "root=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');")
compile=compile.replace("-mmacosx-version-min=15.2","-mmacosx-version-min=26.0").replace("-D__IO80211_TARGET=__MAC_15_2","-D__IO80211_TARGET=__MAC_26_0").replace("-DITLWM_VERSION=\"2.4.0-private-awdl\"","-DITLWM_VERSION=\"2.4.0-latitude-tahoe\"")
(out/'compile-tahoe.py').write_text(compile)
import os
os.environ.update(LATITUDE_AIRPORT_SOURCE=str(w),LATITUDE_AIRPORT_OUTPUT=str(out/'Network-build'),LATITUDE_AIRPORT_REVISION='0.1.2-tahoe-root-radio-bridge')
subprocess.run(['python3',str(out/'build-private-tahoe.py')],check=True,env=os.environ)
print('Tahoe Wi-Fi compilation and ad-hoc signature checks passed.',flush=True)
