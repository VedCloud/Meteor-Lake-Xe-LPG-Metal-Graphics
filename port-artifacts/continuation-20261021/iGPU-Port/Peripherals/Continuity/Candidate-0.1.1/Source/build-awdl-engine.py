from pathlib import Path
import subprocess,json,hashlib
root=Path(__file__).resolve().parent.parent;source=root/'research/owl';out=root/'desktop-build/awdl-engine';out.mkdir(exist_ok=True)
commit=subprocess.check_output(['git','-C',str(source),'rev-parse','HEAD'],text=True).strip()
assert commit=='0ee3263f944ea721c355f826aa90d636d012a512'
sources=list((source/'src').glob('*.c'))+[source/'radiotap/radiotap.c']
for arch in ['arm64','x86_64']:
 d=out/arch;d.mkdir(exist_ok=True);objects=[]
 flags=['-arch',arch,'-mmacosx-version-min=15.0','-std=gnu11','-O1','-g','-I'+str(source/'src'),'-I'+str(source/'radiotap'),'-DRADIOTAP_SUPPORT_OVERRIDES']
 for src in sources:
  obj=d/(src.stem+'.o');objects.append(str(obj));subprocess.run(['xcrun','clang',*flags,'-c',str(src),'-o',str(obj)],check=True,capture_output=True)
 subprocess.run(['xcrun','libtool','-static','-o',str(d/'libLatitudeAWDLEngine.a'),*objects],check=True,capture_output=True)
 if arch=='arm64':
  test=d/'test-awdl';subprocess.run(['xcrun','clang',*flags,'-fsanitize=address,undefined',str(root/'wireless-sources/test-awdl-engine.c'),*[str(p) for p in sources],'-o',str(test)],check=True,capture_output=True)
  subprocess.run([str(test)],check=True)
(out/'PROVENANCE.json').write_text(json.dumps({'upstream':'https://github.com/seemoo-lab/owl','commit':commit,'radiotapCommit':subprocess.check_output(['git','-C',str(source/'radiotap'),'rev-parse','HEAD'],text=True).strip(),'license':'GPL-3.0-or-later; radiotap BSD','sourceSHA256':{str(p.relative_to(source)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},'radioBackend':'not connected','AWDLTransferValidated':False,'IntelCompilation':'passed','hostSanitizerTests':'passed'},indent=2)+'\n')
