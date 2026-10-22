"""Retain the Dell-confirmed Copilot→Command PS/2 profile in future EFI builds."""
from pathlib import Path
import argparse,copy,hashlib,itertools,plistlib
REL=Path('OC/Kexts/VoodooPS2Controller.kext/Contents/PlugIns/VoodooPS2Keyboard.kext/Contents/Info.plist')
def macros():
 out=[]
 for down,key in [(True,0x5b),(False,0xdb)]:
  seq=[bytes([1,0x2a if down else 0xaa]),bytes([2,0x5b if down else 0xdb]),bytes([1,0x6e if down else 0xee])]
  out.extend(bytes([255,255,2,key,0,0,0,0])+b''.join(order) for order in itertools.permutations(seq))
 return out

def retain(document,check=False):
 profiles=document['IOKitPersonalities']['ApplePS2Keyboard']['Platform Profile'];wanted=macros();targets=[profiles['Default']]
 def scan(node):
  if not isinstance(node,dict):return
  if 'Macro Inversion' in node:targets.append(node)
  for child in node.values():scan(child)
 scan(profiles.get('DELL',{}))
 for target in targets:
  existing=target.get('Macro Inversion',[])
  if check:
   if any(existing.count(m)!=1 for m in wanted):raise ValueError('Confirmed Copilot macros missing or duplicated')
  else:target['Macro Inversion']=[m for m in existing if m not in wanted]+wanted
 # The validated EFI uses Windows→Command, not Windows→Option.
 if profiles['Default'].get('Swap command and option',False):raise ValueError('Command/Option swap conflicts with validated mapping')
 return document

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('efi',type=Path);p.add_argument('--check',action='store_true');a=p.parse_args();info=a.efi/REL
 original=info.read_bytes();d=retain(plistlib.loads(original),a.check)
 if not a.check:
  if (info.parent/'_CodeSignature').exists():raise SystemExit('Signed replacement keyboard: re-sign through its build process; refusing to invalidate signature')
  info.write_bytes(plistlib.dumps(d))
 print(('Verified' if a.check else 'Retained')+' Dell-confirmed Copilot→Command profile: '+str(info))
if __name__=='__main__':main()
