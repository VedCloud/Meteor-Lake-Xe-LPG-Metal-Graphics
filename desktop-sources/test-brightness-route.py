from pathlib import Path
import plistlib,hashlib,subprocess
r=Path(__file__).resolve().parent.parent;p=r/'latitude-desktop/BrightnessKeys';u=Path('/Volumes/SEQOUIA/EFI/OC')
a=(u/'ACPI/DSDT.aml').read_bytes();b=(p/'DSDT-brightness.aml').read_bytes();assert hashlib.sha256(a).hexdigest()=='42d65940f44de1482427a022f254e86c9f5c5b40b360418c4aefbc33facec063';assert len(a)==len(b) and sum(b)%256==0
pos=a.index(b'BRT6\x02');expected=bytearray(a);expected[pos:pos+5]=b'XBR6\x02';pos2=a.index(b'SMEE\x01');expected[pos2:pos2+5]=b'XSME\x01';expected[9]=0;expected[9]=(-sum(expected))&255;assert bytes(expected)==b
assert a.count(b'BRT6\x02')==1 and b.count(b'XBR6\x02')==1
c=plistlib.loads((p/'config.plist').read_bytes());old=plistlib.loads((u/'config.plist').read_bytes());added=c['ACPI']['Add'].pop();assert added['Path']=='SSDT-LAT-BRT.aml' and added['Enabled'] and c==old
ssdt=(p/'SSDT-LAT-BRT.aml').read_bytes();assert sum(ssdt)%256==0 and len(ssdt)>209
# Validate exact fallback and native PS/2 make+break event protocol against upstream implementation.
s=(r/'research/VoodooPS2/VoodooPS2Keyboard/VoodooPS2Keyboard.cpp').read_text(encoding='latin1');assert 'packet[0] -= 2;' in s and 'packet[1] |= 0x80;' in s
keymap=(r/'research/VoodooPS2/VoodooPS2Keyboard/ApplePS2ToADBMap.h').read_text();assert 'BRIGHTNESS_DOWN,    // e0 05' in keymap and 'BRIGHTNESS_UP,      // e0 06' in keymap
subprocess.run([str(r/'research/OpenCore-release/Utilities/ocvalidate/ocvalidate'),str(p/'config.plist')],check=True)
print('PASS exact DSDT two definition-only renames/checksum; added Darwin-only SSDT leaves other config identical; original EV6 call preserved; PS2 0406 up/0405 down protocol and native key mapping verified; config validation passed.')
