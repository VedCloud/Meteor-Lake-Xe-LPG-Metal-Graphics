from pathlib import Path
import importlib.util,tempfile,json,struct
from unittest.mock import patch
spec=importlib.util.spec_from_file_location('touch',Path(__file__).with_name('collect-touchpad-linux.py'));m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp)
 for event,vendor,product,bus,name in [('event0','04f3','328e','0018','ELAN Touchpad'),('event1','04f3','328e','0018','ELAN Mouse'),('event2','056a','4a19','0018','Wacom Touchpad'),('event3','04f3','328e','0003','External Touchpad'),('event4','0001','0001','0011','Keyboard')]:
  d=root/'class/input'/event/'device';(d/'id').mkdir(parents=True)
  for k,v in [('vendor',vendor),('product',product),('bustype',bus)]: (d/'id'/k).write_text(v)
  (d/'name').write_text(name)
 assert [x['event'] for x in m.discover(root)]==['event0']
metric=m.Metrics(5,{})
for slot in range(5):metric.feed((10,0,3,47,slot));metric.feed((10,0,3,57,987650+slot));metric.feed((10,0,3,53,123456789))
metric.feed((10,0,0,0,0));metric.feed((10,8000,0,0,0));r=metric.summarize();assert r['maximumConcurrentContacts']==5 and abs(r['eventGapMs']['maximum']-8)<1e-6
assert '123456789' not in json.dumps(r) and '98765' not in json.dumps(r)
metric.feed((10,9000,0,3,0));assert metric.feed((10,10000,0,0,0))=='resync';assert metric.summarize()['SYN_DROPPED']==1 and metric.summarize()['maximumConcurrentContacts'] is None
# Bad slots never cause out-of-range indexing or unbounded allocation.
metric=m.Metrics(5,{});metric.feed((1,0,3,47,1000000));metric.feed((1,0,3,57,123));assert not metric.contactReliable and metric.active=={}
def ioctl(fd,request,data,mutate):
 assert fd==99 and mutate
 if request==0x8018456f:data[:]=struct.pack('=iiiiii',2,0,4,0,0,0)
 else:
  assert request==0x8018450a and data[0]==57
  for i,v in enumerate([-1,7,-1,-1,99],1):data[i]=v
with patch.object(m.fcntl,'ioctl',side_effect=ioctl):
 assert m.absinfo(99,47)==(2,{'minimum':0,'maximum':4,'fuzz':0,'flat':0,'resolution':0})
 assert m.contacts(99,5)=={1:7,4:99}
print('PASS ELAN I2C-only selection, 5-contact frames, 8ms delivery interval, dropped-state invalidation, invalid slots, ioctl layout and absence of coordinates/tracking IDs from output.')
# Reproduce the photographed failure on FAT: sysfs controller names contain ':'.
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);debug=root/'debug';out=root/'report';out.mkdir()
 pins=debug/'pinctrl/INTC1083:00';pins.mkdir(parents=True)
 (pins/'pins').write_text('pin 1 fixture\n');(pins/'pinmux-pins').write_text('fixture\n')
 original=Path.write_text
 def fat_write(path,*args,**kwargs):
  if any(c in path.name for c in ':<>"/\\|?*'):raise OSError(22,'Invalid argument',str(path))
  if path.name.endswith('pinmux-pins.txt'):raise OSError(13,'Permission denied',str(path))
  return original(path,*args,**kwargs)
 with patch.object(Path,'write_text',fat_write):m.collect_gpio(out,debug)
 index=json.loads((out/'gpio/INDEX.json').read_text());assert len(index['files'])==1 and len(index['errors'])==1
 assert 'INTC1083%3A00' in index['files'][0]['file']
 assert (out/'gpio'/index['files'][0]['file']).read_text().strip()=='pin 1 fixture'
 assert m.portable_name('a:b')!=m.portable_name('a%3Ab') and len(m.portable_name('x'*500))<220
print('PASS FAT-safe GPIO filenames, reversible source index, optional read/write failures do not stop gesture capture.')
