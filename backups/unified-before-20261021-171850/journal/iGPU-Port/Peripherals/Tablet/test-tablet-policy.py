from pathlib import Path
import subprocess,tempfile,json,math,statistics,hashlib
root=Path(__file__).resolve().parent.parent
capture=root/'research/tablet-linux-20261021/guided-20261021-125359'
with tempfile.TemporaryDirectory() as tmp:
 exe=Path(tmp)/'replay'
 subprocess.run(['xcrun','clang++','-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(root/'tablet-sources/replay-tablet.cpp'),'-o',str(exe)],check=True)
 def replay(rows):
  out=subprocess.check_output([str(exe)],input=''.join(' '.join(map(str,r))+'\n' for r in rows),text=True)
  return [list(map(int,line.split())) for line in out.splitlines()]
 def row(t,mode=1,q=1,x=9300,y=0,z=-3200,stamp=None,valid=1,lock=0):return [t,mode,q,valid,x,y,z,t if stamp is None else stamp,lock]
 assert replay([row(0),row(500),row(1000)])[-1]==[2,1,1,1]
 assert replay([row(0,q=0),row(500,q=0),row(1000,q=0)])[-1][2]==0
 assert replay([row(0),row(500),row(1000),row(2601,mode=-1)])[-1][0:3:2]==[0,0]
 assert replay([row(0),row(500),row(1000),row(1001,mode=0)])[-1][:3]==[1,0,0]
 assert replay([row(1000),row(1500),row(2000),row(1)])[-1][:3]==[0,0,0]
 for args in [dict(x=0,y=0,z=0),dict(x=2147483647),dict(x=-2147483648),dict(stamp=9999),dict(stamp=0),dict(lock=1)]:
  result=replay([row(t,**args) for t in [100,600,1100,1600,2100]])
  assert all(r[1]==0 for r in result),args
 # Missing/frozen samples, diagonal chatter and a flat tablet hold accepted portrait.
 start=[row(t) for t in [0,500,1000]]
 for more in [[row(1500,x=0,y=1000,z=-9800),row(2000,x=0,y=1000,z=-9800)],
              [row(1500,x=-9300,stamp=1500),row(2000,x=-9300,stamp=1500)],
              [row(1500,x=-6900,y=6900,z=0),row(2000,x=-6900,y=6900,z=0)]]:
  assert replay(start+more)[-1][1]==1
 rows=[];groups={};proof=[]
 for position in ['laptop','tablet','portrait-left','portrait-right','laptop-return']:
  file=capture/(position+'-samples.jsonl');group=[];switches={}
  for line in file.read_text().splitlines():
   d=json.loads(line);v=next(i['values'] for i in d['iio'] if i['values']['name']=='accel_3d')
   xyz=[(float(v['in_accel_'+a+'_raw'])+float(v['in_accel_offset']))*float(v['in_accel_scale']) for a in 'xyz'];group.append(xyz)
   for sw in d['switches']:
    if sw['tabletMode'] is not None:switches.setdefault(sw['name'],set()).add(sw['tabletMode'])
   mode=next(s['tabletMode'] for s in d['switches'] if s['name']=='Intel HID switches')
   t=round(d['monotonic']*1000);rows.append(row(t,mode=int(mode),x=round(xyz[0]*1000),y=round(xyz[1]*1000),z=round(xyz[2]*1000)))
  groups[position]={'samples':len(group),'medianMS2':dict(zip('xyz',[statistics.median(v[i] for v in group) for i in range(3)])),'gravityNormRangeMS2':[min(math.dist(v,[0,0,0]) for v in group),max(math.dist(v,[0,0,0]) for v in group)],'switches':{k:sorted(v) for k,v in switches.items()}}
  proof.append({'file':file.name,'sha256':hashlib.sha256(file.read_bytes()).hexdigest()})
 result=replay(rows)
 assert len(result)==50
 for i,position in enumerate(groups):
  ending=result[i*10+9];groups[position]['replayedDecision']={'mode':['unknown','laptop','tablet'][ending[0]],'physicalPose':['landscape','portrait-left','portrait-right','inverted'][ending[1]],'suppressBuiltInIntent':bool(ending[2])}
 assert [result[i][1] for i in [9,19,29,39,49]]==[0,0,1,2,0]
 report={'version':'0.1.0','actuationEnabled':False,'macOSSensorTransportImplemented':False,'sampleCount':len(rows),'positions':groups,'inputs':proof,'notes':['Physical pose labels are not yet framebuffer/touch transform angles.','Inverted orientation was not captured on hardware.','Intent is policy output only; no keyboard or touchpad is disabled by this replay.','Intel HID switches is the measured mode source; Dell WMI switch stayed false.','The zero quaternion is invalid; acceleration is the calibrated source.']}
 out=root/'port-artifacts/tablet-policy';out.mkdir(parents=True,exist_ok=True);(out/'CAPTURE-ANALYSIS.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS native policy under ASan/UBSan: all 50 captured samples, debouncing, flat hold, stale mode fail-open, qualification, clock reset, invalid vectors, frozen samples, rotation lock and laptop input restoration intent.')
