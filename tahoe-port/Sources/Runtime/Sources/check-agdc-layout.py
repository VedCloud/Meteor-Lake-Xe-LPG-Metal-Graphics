from pathlib import Path
import sys,capstone,json
b=Path(__file__).resolve().parent;r=Path('/Volumes/Corsair 1TB NVMe/iGPU Port');sys.path.insert(0,str(r/'NullMoth/tools'));from vtdump import load,vtables
rel='System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/AppleGraphicsDeviceControl.kext/Contents/MacOS/AppleGraphicsDeviceControl'
rows=[]
for root in [r/'research/kdk-24H32-expanded/KDK.pkg/Payload',b/'KDK25G241-expanded/KDK.pkg/Payload']:
 p=root/rel;d,s,ext,fo,by=load(p);n='__ZN26AppleGraphicsDeviceControl9MetaClassC1Ev';addr=next(v for name,t,ss,v in s if name==n);offset=fo(addr);md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64);code=list(md.disasm(d[offset:offset+96],addr));size=next(int(i.op_str.split(',')[1].strip(),0) for i in code if i.mnemonic=='mov' and i.op_str.startswith('ecx, '));slots=len(vtables(p)['26AppleGraphicsDeviceControl']);rows.append({'path':str(p),'size':size,'slots':slots})
assert rows[0]['size']==rows[1]['size']==0x110 and rows[0]['slots']==rows[1]['slots']
(b/'AGDC-LAYOUT.json').write_text(json.dumps(rows,indent=2)+'\n');print('AGDC base size0x110 and',rows[1]['slots'],'vtable slots match.')
