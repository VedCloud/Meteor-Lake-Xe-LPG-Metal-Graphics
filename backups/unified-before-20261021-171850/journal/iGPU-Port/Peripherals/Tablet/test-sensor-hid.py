from pathlib import Path
import importlib.util,json,random
source=Path(__file__).with_name('decode-sensor-hid.py');spec=importlib.util.spec_from_file_location('decoder',source);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
root=source.parent.parent;inventory=json.loads((root/'research/tablet-linux-20261021/guided-20261021-125359/laptop-inventory.json').read_text())
for device in inventory['sensor_hid']:
 decoded=m.decode(bytes.fromhex(device['report_descriptor_hex']))
 assert decoded['fields'] and all(r['wireByteLength']<=8193 for r in decoded['reports'])
 if device['path'].endswith('.0006'):
  axes=[f for f in decoded['fields'] if f['type']=='input' and f['usage'] in [0x200453,0x200454,0x200455]]
  assert [(f['reportID'],f['bitOffset'],f['bitWidth'],f['unitExponent']) for f in axes]==[(3,208,32,-6),(3,240,32,-6),(3,272,32,-6)]
# Main item offsets separate by report type and ID; PUSH/POP, signed range/exponent.
fixture=bytes.fromhex('05200973a10085031580257f7508950109538102a48504751009548102b409558102550eb102c0')
d=m.decode(fixture);assert [(f['type'],f['reportID'],f['bitOffset'],f['bitWidth']) for f in d['fields']]==[('input',3,0,8),('input',4,0,16),('input',3,8,8),('feature',3,0,8)]
assert d['fields'][-1]['unitExponent']==-2 and d['fields'][0]['logicalMin']==-128
for bad in [b'\xfe',b'\xfe\x03\x00\x00',b'\x77',b'\xb4',b'\xc0',bytes.fromhex('8500'),bytes.fromhex('750895ff8102')*100,bytes.fromhex('05200973a100')]:
 try:m.decode(bad)
 except m.DescriptorError:pass
 else:raise AssertionError(bad[:16])
rng=random.Random(5350)
for _ in range(2500):
 try:m.decode(rng.randbytes(rng.randrange(128)))
 except m.DescriptorError:pass
print('PASS four captured descriptors, exact signed XYZ layout, independent input/feature/report offsets, global stack, signed exponents, malformed/truncated limits and 2500 seeded malformed inputs.')
