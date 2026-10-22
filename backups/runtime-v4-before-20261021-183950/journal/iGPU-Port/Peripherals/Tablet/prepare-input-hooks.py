from pathlib import Path
import shutil,subprocess,json
root=Path(__file__).resolve().parent.parent;up=root/'research/VoodooPS2';out=root/'desktop-build/tablet-input-hooks';out.mkdir(exist_ok=True)
assert subprocess.check_output(['git','-C',str(up),'rev-parse','HEAD'],text=True).strip()=='7eab4a344d901d1e4aa6ece4189ecde5a7356287'
def replace(s,old,new):assert s.count(old)==1,old;return s.replace(old,new)
for typ,cls,gate in [('Keyboard','ApplePS2Keyboard','KeyboardGate'),('Mouse','ApplePS2Mouse','PointerGate')]:
 d=out/('VoodooPS2'+typ);shutil.copytree(up/d.name,d,dirs_exist_ok=True)
 for name in ['LatitudeInputSuppression.h','LatitudeInputRuntime.h']:shutil.copyfile(root/'tablet-sources'/name,d/name)
 hp=d/('VoodooPS2'+typ+'.h');h=hp.read_text();h=replace(h,'private:', 'private:\n    LatitudeTablet::'+gate+' _latitudeGate;\n    IOTimerEventSource* _latitudeTimer=nullptr;\n    void latitudeTick();\n    void latitudeApply(bool block);')
 h='#include "LatitudeInputRuntime.h"\n'+h
 if typ=='Keyboard':
  h=replace(h,'{ dispatchKeyboardEvent(keyCode, goingDown, *(AbsoluteTime*)&time); }','{ if (_latitudeGate.accept(keyCode,goingDown)) dispatchKeyboardEvent(keyCode, goingDown, *(AbsoluteTime*)&time); }')
 else:
  h=replace(h,'{ dispatchRelativePointerEvent(dx, dy, buttonState, *(AbsoluteTime*)&now); }','{ uint32_t filtered=0;if (_latitudeGate.pointer(buttonState,filtered)) dispatchRelativePointerEvent(dx, dy, filtered, *(AbsoluteTime*)&now); }')
  h=replace(h,'{ dispatchScrollWheelEvent(deltaAxis1, deltaAxis2, deltaAxis3, *(AbsoluteTime*)&now); }','{ if (_latitudeGate.scroll()) dispatchScrollWheelEvent(deltaAxis1, deltaAxis2, deltaAxis3, *(AbsoluteTime*)&now); }')
 hp.write_text(h);cp=d/('VoodooPS2'+typ+'.cpp');s=cp.read_text(encoding="latin1")
 anchor='    DEBUG_LOG("ApplePS2Keyboard::start leaving.\\n");' if typ=='Keyboard' else '  setProperty(kDeliverNotifications, true);'
 s=replace(s,anchor,anchor+f'''\n  // Failure leaves normal input working; no controller enable/disable writes.
  _latitudeTimer=IOTimerEventSource::timerEventSource(this,OSMemberFunctionCast(IOTimerEventSource::Action,this,&{cls}::latitudeTick));
  if(_latitudeTimer){{if(pWorkLoop->addEventSource(_latitudeTimer)!=kIOReturnSuccess){{_latitudeTimer->release();_latitudeTimer=nullptr;}}else _latitudeTimer->setTimeoutMS(500);}}
  setProperty("LatitudeTabletGateVersion","0.1.0");
''')
 stop=f'void {cls}::stop(IOService * provider)\n{{'
 s=replace(s,stop,stop+'''\n  if(_latitudeTimer){_latitudeTimer->cancelTimeout();getWorkLoop()->removeEventSource(_latitudeTimer);_latitudeTimer->release();_latitudeTimer=nullptr;}\n  latitudeApply(false);''')
 power=f'void {cls}::setDevicePowerState( UInt32 whatToDo )\n{{'
 s=replace(s,power,power+'''\n  if(whatToDo==kPS2C_DisableDevice){if(_latitudeTimer)_latitudeTimer->cancelTimeout();latitudeApply(false);}\n  if(whatToDo==kPS2C_EnableDevice&&_latitudeTimer)_latitudeTimer->setTimeoutMS(500);''')
 emit='[&](unsigned key,bool down){dispatchKeyboardEvent(key,down,*(AbsoluteTime*)&now);}' if typ=='Keyboard' else '[&](){dispatchRelativePointerEvent(0,0,0,*(AbsoluteTime*)&now);}'
 if typ=='Keyboard':
  s=replace(s,'    // codes e0f0 through e0ff can be used to call back into ACPI methods on this device','''    // Suppress before ACPI/backlight/sleep side effects, while still letting
    // Macro Inversion produce its mapped make/break packets (Copilot retained).
    if (_latitudeGate.isBlocked()) {
        if (keyCode < sizeof(_PS2ToADBMap)) _latitudeGate.accept(_PS2ToADBMap[keyCode],goingDown);
        return false;
    }
    // codes e0f0 through e0ff can be used to call back into ACPI methods on this device''')
 else:
  s=replace(s,'  buttons = middleButton(buttons, now_abs, fromMouse);','  if (!_latitudeGate.isBlocked()) buttons = middleButton(buttons, now_abs, fromMouse);')
 reset=(''' if(block!=_latitudeGate.isBlocked()){
  if(_macroTimer)_macroTimer->cancelTimeout();_macroCurrent=0;
  if(block){if(_sleepEjectTimer)_sleepEjectTimer->cancelTimeout();}else _PS2modifierState=0;
 }
''' if typ=='Keyboard' else ''' if(block&&!_latitudeGate.isBlocked()){if(_buttonTimer)_buttonTimer->cancelTimeout();_pendingbuttons=0;_mbuttonstate=STATE_NOBUTTONS;lastbuttons=0;}
''')
 s+=f'''\nvoid {cls}::latitudeApply(bool block){{
 uint64_t now=0;clock_get_uptime(&now);
{reset} _latitudeGate.transition(block,{emit});
 setProperty("LatitudeTabletInputBlocked",_latitudeGate.isBlocked());
}}
void {cls}::latitudeTick(){{
 latitudeApply(latitudeSuppressionRequested());
 if(_latitudeTimer)_latitudeTimer->setTimeoutMS(500);
}}
'''
 cp.write_text(s,encoding="latin1")
flags=['-arch','x86_64','-mmacosx-version-min=11.0','-mkernel','-fapple-kext','-nostdinc','-I'+str(root/'MacKernelSDK/Headers'),'-I'+str(up/'VoodooPS2Controller'),'-DKERNEL','-DKERNEL_PRIVATE','-DDRIVER_PRIVATE','-std=c++17','-fno-rtti','-fno-exceptions','-fno-builtin','-fno-common','-O2','-Wno-deprecated-declarations','-Wno-nullability-completeness','-Wno-inconsistent-missing-override','-Wno-macro-redefined']
for typ in ['Keyboard','Mouse']:
 p=out/('VoodooPS2'+typ)/('VoodooPS2'+typ+'.cpp');result=subprocess.run(['xcrun','clang++',*flags,'-c',str(p),'-o',str(out/(typ+'.o'))],capture_output=True,text=True);(out/(typ+'-compile.log')).write_text(result.stdout+result.stderr);result.check_returncode()
(out/'STATUS.json').write_text(json.dumps({'sourceCommit':'7eab4a344d901d1e4aa6ece4189ecde5a7356287','IntelSourceCompilation':'passed','activePS2BinariesReplaced':False,'activation':'requires the separate publisher InputSuppressionEnabled=true; 0.2.1 reader arms only corroborated fresh folded mode','scope':'current ApplePS2Keyboard and ApplePS2Mouse only; external devices and I2C trial unchanged','timerMS':500,'inputGate':'release/quarantine and lease tests passed; device boot validation pending'},indent=2)+'\n')
print('PASS actual PS2 keyboard/mouse sources with workloop timers, held-state release, per-device gates and stale-state restoration compile for Intel. Working USB binaries are unchanged; hooks staged only.')
# Link standalone plugin candidates against the existing controller ABI. Keep the
# Dell-confirmed platform profiles and Macro Inversion byte-for-byte. These are
# packaged for review, not substituted into the active EFI.
import plistlib,hashlib
for typ in ['Keyboard','Mouse']:
 bundle='as.acidanthera.voodoo.driver.PS2'+typ
 kmod=out/(typ+'-kmod.cpp');kmod.write_text('''#include <mach/kmod.h>
extern "C" { extern kmod_start_func_t _start; extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL('''+bundle+''',"2.3.8",_start,_stop)
kmod_start_func_t *_realmain=0;kmod_stop_func_t *_antimain=0;int _kext_apple_cc=__APPLE_CC__;
}
''')
 subprocess.run(['xcrun','clang++',*flags,'-c',str(kmod),'-o',str(out/(typ+'-kmod.o'))],check=True,capture_output=True)
 origin=Path('/Volumes/SEQOUIA/EFI/OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns')/('VoodooPS2'+typ+'.kext');k=out/origin.name
 if k.exists():shutil.rmtree(k)
 shutil.copytree(origin,k,ignore=shutil.ignore_patterns('._*','_CodeSignature'))
 binary=k/'Contents/MacOS'/('VoodooPS2'+typ)
 subprocess.run(['xcrun','clang++','-arch','x86_64','-fapple-kext','-nostdlib','-Xlinker','-kext',str(out/(typ+'.o')),str(out/(typ+'-kmod.o')),str(root/'MacKernelSDK/Library/x86_64/libkmod.a'),'-o',str(binary)],check=True,capture_output=True)
 info=k/'Contents/Info.plist';d=plistlib.loads(info.read_bytes());d['LatitudeTabletGateRevision']='0.1.0-private';info.write_bytes(plistlib.dumps(d))
 if typ=='Keyboard':
  original=plistlib.loads((origin/'Contents/Info.plist').read_bytes());assert d['IOKitPersonalities']==original['IOKitPersonalities']
 subprocess.run(['codesign','-s','-','--force',str(k)],check=True,capture_output=True);subprocess.run(['codesign','--verify','--strict',str(k)],check=True,capture_output=True)
 print('Linked/signed staged',k.name,hashlib.sha256(binary.read_bytes()).hexdigest())
status=json.loads((out/'STATUS.json').read_text());status.update(IntelPluginsLinkedAndSigned=True,KeyboardProfilesAndCopilotMacrosUnchanged=True,conditionalActivation=True);(out/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n')
