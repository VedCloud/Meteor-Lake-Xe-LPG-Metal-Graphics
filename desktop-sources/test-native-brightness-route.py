from pathlib import Path
import re,subprocess
r=Path(__file__).resolve().parent.parent
s=(r/'desktop-sources/SSDT-LAT-BRT.dsl').read_text()
assert 'Notify (\\_SB.PC00.GFX0.LCD, 0x86)' in s and 'Notify (\\_SB.PC00.GFX0.LCD, 0x87)' in s
assert '0x0406' not in s and '0x0405' not in s
assert 'Else { XBR6 (Arg0, Arg1) }' in s and 'Else { XSME (Arg0) }' in s
assert s.count('GENS (0x11, Zero, Zero)')==1
upstream=(r/'research/BrightnessKeys/BrightnessKeys/BrightnessKeys.cpp').read_text()
assert '_panel->registerInterest(gIOGeneralInterest, _panelNotification, this)' in upstream
assert 'case kIOACPIMessageBrightnessUp:' in upstream and 'case kIOACPIMessageBrightnessDown:' in upstream
assert 'self->setProperty(kBrightnessPanel' in upstream
# Firmware already exposes the standard LCD address used by BrightnessKeys.
firmware=(r/'desktop-build/brightness-dsdt.dsl').read_text();lcd=firmware.index('Device (LCD)',firmware.index('Method (XBR6')-15000)
assert 'Return (0x0400)' in firmware[lcd:lcd+600]
subprocess.run(['iasl','-p',str(r/'desktop-build/native-brightness-test'),str(r/'desktop-sources/SSDT-LAT-BRT.dsl')],check=True)
b=(r/'desktop-build/native-brightness-test.aml').read_bytes();assert len(b)==329 and sum(b)%256==0
print('PASS native panel brightness route: standard LCD address, 0x86/0x87 notifications, existing BrightnessKeys listener, Darwin-only routing, firmware fallback and single event-flag read preserved; AML compiles/checksum valid. Physical keys require target validation.')
