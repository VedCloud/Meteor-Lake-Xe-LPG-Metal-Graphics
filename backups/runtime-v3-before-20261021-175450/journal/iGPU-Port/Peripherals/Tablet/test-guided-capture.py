from pathlib import Path
import importlib.util,tempfile,json
from unittest.mock import patch
spec=importlib.util.spec_from_file_location('guided',Path(__file__).with_name('capture-sensors.py'));m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp)
 for event,cap in [('event0','0'),('event1','2'),('event2','1')]:
  d=root/'class/input'/event/'device';(d/'capabilities').mkdir(parents=True);(d/'capabilities/sw').write_text(cap);(d/'name').write_text(event)
 devices=m.switch_devices(root);assert [x['event'] for x in devices]==['event1','event2']
 def ioctl(fd,request,state,mutate):
  assert fd==99 and request==0x8008451b and mutate is True
  state[0]=2
 with patch.object(m.os,'open',return_value=99),patch.object(m.os,'close'),patch.object(m.fcntl,'ioctl',side_effect=ioctl):
  states=m.switches(devices)
 assert states[0]['tabletMode'] is True and states[0]['lidClosed'] is None
 assert states[1]['tabletMode'] is None and states[1]['lidClosed'] is False
 output=root/'report';output.mkdir()
 with patch.object(m,'switch_devices',return_value=[]),patch.object(m.INV,'inventory',return_value={'iio':[]}),patch.object(m.INV,'sensors',return_value=[]),patch.object(m.time,'sleep'):
  assert m.worker('tablet',output)==0
 records=[json.loads(l) for l in (output/'tablet-samples.jsonl').read_text().splitlines()]
 assert len(records)==10 and [x['sample'] for x in records]==list(range(10))
 assert all(x['position']=='tablet' and x['switches']==[] for x in records)
print('PASS actual capture worker, 10 flushed samples, switch-only capability selection and EVIOCGSW state decoding. No hardware or input-event reads.')
