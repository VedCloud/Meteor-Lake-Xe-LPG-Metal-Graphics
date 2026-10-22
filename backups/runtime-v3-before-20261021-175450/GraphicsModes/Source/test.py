from pathlib import Path
import tempfile,plistlib,subprocess,sys,copy
sys.path.insert(0,str(Path(__file__).parent));from prepare import configure,create_script,GUID,MARKERS
root=Path(__file__).resolve().parent.parent
sample=plistlib.loads((root/'backups/input-system-sync-20261021/EFI/OC/config.plist').read_bytes())
a=configure(sample,True);b=configure(a,False)
for k in MARKERS:assert a['NVRAM']['Add'][GUID][k]=='1' and k in a['NVRAM']['Delete'][GUID] and k not in b['NVRAM']['Add'][GUID]
x=copy.deepcopy(a);x['NVRAM']=sample['NVRAM'];assert x==sample
assert configure(a,True)==a and configure(b,False)==b
with tempfile.TemporaryDirectory() as tmp:
 u=Path(tmp);port=u/'iGPU-Port';g=port/'GraphicsModes';g.mkdir(parents=True);(port/'backups').mkdir();cfg=u/'EFI/OC/config.plist';cfg.parent.mkdir(parents=True)
 for mode,d in [('accelerated',a),('recovery',b)]: (g/('base-'+mode+'.plist')).write_bytes(plistlib.dumps(d))
 script=port/'switch-graphics-mode.sh';script.write_text(create_script(g,['base']));cfg.write_bytes(plistlib.dumps(a))
 for mode in ['recovery','accelerated','accelerated']:
  c=subprocess.run(['/bin/bash',str(script),mode],capture_output=True,text=True);assert c.returncode==0,c.stdout+c.stderr;assert cfg.read_bytes()==(g/('base-'+mode+'.plist')).read_bytes()
 cfg.write_bytes(b'unknown changed file');assert subprocess.run(['/bin/bash',str(script),'recovery'],capture_output=True).returncode==2 and cfg.read_bytes()==b'unknown changed file'
 cfg.write_bytes(plistlib.dumps(a));(g/'base-recovery.plist').write_bytes(b'tampered');assert subprocess.run(['/bin/bash',str(script),'recovery'],capture_output=True).returncode==2 and cfg.read_bytes()==plistlib.dumps(a)
print('PASS actual mode switch: marker refresh each normal boot, explicit recovery, idempotence, guarded configs, tampered source refusal; no host NVRAM changed.')
