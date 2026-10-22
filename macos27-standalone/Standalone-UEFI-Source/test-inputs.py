# SPDX-License-Identifier: MIT
"""Exercise the freestanding admission code; reject malformed files without host UB."""
from pathlib import Path
import hashlib,json,os,struct,subprocess,tempfile
r=Path(__file__).resolve().parent;root=r.parent;exe=r/'test-inputs'
subprocess.run(['xcrun','clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-g',str(r/'boot_input.c'),str(r/'test-inputs.c'),'-o',str(exe)],check=True)
for n in [0,1,3,55,56,57,63,64,65,127,128,129,4096,1000000]:
 data=os.urandom(n)
 result=subprocess.run([str(exe),'sha'],input=data,capture_output=True,check=True)
 assert not result.stderr and result.stdout.decode().strip()==hashlib.sha256(data).hexdigest(),n
kernel=root/'darwin-vm/firmware/bootkc'
assert subprocess.run([str(exe),str(kernel)],capture_output=True,check=True).stdout==b'0\n'
base=bytearray(kernel.read_bytes()[:30000]);cases=[]
for name,at,val in [('bad-magic',0,0),('wrong-architecture',4,0x01000007),('wrong-filetype',12,2),('huge-count',16,0xffffffff),('huge-command-bytes',20,0xffffffff),('zero-command-size',36,0),('overflow-command-size',36,0xfffffff8)]:
 data=base.copy();struct.pack_into('<I',data,at,val);cases.append((name,data))
for n in range(33):cases.append(('truncation-'+str(n),base[:n]))
# A bounded synthetic fileset exercises command-specific validation independently
# of Apple's supplied kernel. It is never treated as a bootable image.
synthetic=bytearray(144);struct.pack_into('<8I',synthetic,0,0xfeedfacf,0x0100000c,2,12,2,112,0,0)
struct.pack_into('<II',synthetic,32,0x19,72);struct.pack_into('<QQQQ',synthetic,56,0x100000,144,0,144)
struct.pack_into('<IIQQII',synthetic,104,0x80000035,40,0x100000,0,32,0);synthetic[136:144]=b'kernel\0\0'
for name,at,fmt,val in [('segment-offset-overflow',72,'Q',0xffffffffffffffff),('segment-length-overflow',80,'Q',0xffffffffffffffff),('vm-range-overflow',64,'Q',0xffffffffffffffff),('sections-outside-command',96,'I',1),('fileset-offset-outside-file',120,'Q',144),('fileset-name-outside-command',128,'I',40)]:
 data=synthetic.copy();struct.pack_into('<'+fmt,data,at,val);cases.append((name,data))
data=synthetic.copy();data[136:144]=b'abcdefgh';cases.append(('unterminated-name',data))
with tempfile.TemporaryDirectory(dir=root/'tmp') as tmp:
 p=Path(tmp)/'valid-synthetic';p.write_bytes(synthetic)
 result=subprocess.run([str(exe),str(p)],capture_output=True);assert result.returncode==0 and result.stdout==b'0\n' and not result.stderr,result
 for name,data in cases:
  p=Path(tmp)/name;p.write_bytes(data);result=subprocess.run([str(exe),str(p)],capture_output=True)
  assert result.returncode==1 and result.stdout.strip().isdigit() and 1<=int(result.stdout)<=11 and not result.stderr,(name,result)
report={'SHA256ReferenceComparisons':14,'actual26A428Kernel':'PASS','syntheticFileset':'PASS','malformedFilesRejected':len(cases),'ASanUBSan':'PASS','kernelExecution':False}
(r/'INPUT-VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
