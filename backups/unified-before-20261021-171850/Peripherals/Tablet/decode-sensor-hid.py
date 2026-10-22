#!/usr/bin/env python3
"""Bounded offline HID report-layout decoder. Never opens hardware or sends reports."""
import json,hashlib,sys
from pathlib import Path
class DescriptorError(ValueError):pass

def decode(data):
 if len(data)>65536:raise DescriptorError('descriptor too large')
 g={'page':0,'size':0,'count':0,'id':0,'logicalMin':0,'logicalMax':0,'unit':0,'exponent':0};stack=[];local=[];minimum=None;collections=[];offsets={};fields=[];i=0
 while i<len(data):
  prefix=data[i];i+=1
  if prefix==0xfe:
   if i+2>len(data):raise DescriptorError('truncated long header')
   length=data[i];i+=2
   if i+length>len(data):raise DescriptorError('truncated long item')
   i+=length;continue
  size=[0,1,2,4][prefix&3];kind=(prefix>>2)&3;tag=prefix>>4
  if i+size>len(data):raise DescriptorError('truncated short item')
  raw=data[i:i+size];i+=size;v=int.from_bytes(raw,'little');signed=int.from_bytes(raw,'little',signed=True)
  if kind==1:
   if tag==0:g['page']=v
   elif tag==1:g['logicalMin']=signed
   elif tag==2:g['logicalMax']=signed if g['logicalMin']<0 else v
   elif tag==5:
    exponent=v&15;g['exponent']=exponent-16 if exponent&8 else exponent
   elif tag==6:g['unit']=v
   elif tag==7:
    if v>8192:raise DescriptorError('oversized field')
    g['size']=v
   elif tag==8:
    if not 1<=v<=255:raise DescriptorError('invalid report ID')
    g['id']=v
   elif tag==9:
    if v>4096:raise DescriptorError('oversized report count')
    g['count']=v
   elif tag==10:
    if len(stack)>=32:raise DescriptorError('global stack overflow')
    stack.append(g.copy())
   elif tag==11:
    if not stack:raise DescriptorError('global stack underflow')
    g=stack.pop()
  elif kind==2:
   usage=v if size==4 else (g['page']<<16)|v
   if tag==0:
    local.append(usage)
    if len(local)>4096:raise DescriptorError('too many usages')
   elif tag==1:minimum=usage
   elif tag==2:
    if minimum is None or usage<minimum or usage-minimum>4095:raise DescriptorError('bad usage range')
    local.extend(range(minimum,usage+1));minimum=None
    if len(local)>4096:raise DescriptorError('too many usages')
   elif tag==10 and v:raise DescriptorError('usage delimiters unsupported')
  elif kind==0:
   if tag==10:
    if len(collections)>=32:raise DescriptorError('collection stack overflow')
    collections.append(local[0] if local else None)
   elif tag==12:
    if not collections:raise DescriptorError('collection stack underflow')
    collections.pop()
   elif tag in (8,9,11):
    report={8:'input',9:'output',11:'feature'}[tag];key=(report,g['id']);start=offsets.get(key,0);bits=g['size']*g['count']
    if start+bits>65536 or len(fields)+g['count']>16384:raise DescriptorError('report limits exceeded')
    offsets[key]=start+bits
    for j in range(g['count']):
     usage=local[min(j,len(local)-1)] if local and v&2 else None
     fields.append({'type':report,'reportID':g['id'],'bitOffset':start+j*g['size'],'bitWidth':g['size'],'usage':usage,'collectionPath':collections.copy(),'flags':v,'constant':bool(v&1),'variable':bool(v&2),'logicalMin':g['logicalMin'],'logicalMax':g['logicalMax'],'unit':g['unit'],'unitExponent':g['exponent']})
   local=[];minimum=None
 if collections or stack:raise DescriptorError('unclosed stack')
 return {'sha256':hashlib.sha256(data).hexdigest(),'byteLength':len(data),'offsetConvention':'bit offsets exclude leading report-ID byte; wireByteLength includes it when nonzero','reports':[{'type':key[0],'id':key[1],'payloadBits':bits,'wireByteLength':(bits+7)//8+bool(key[1])} for key,bits in sorted(offsets.items())],'fields':fields}

def main():
 inventory=Path(sys.argv[1]);out=Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True);summary=[]
 for device in json.loads(inventory.read_text())['sensor_hid']:
  data=bytes.fromhex(device['report_descriptor_hex']);decoded=decode(data);name='sensor-'+device['path'].rsplit('.',1)[-1]
  (out/(name+'.json')).write_text(json.dumps(decoded,indent=2)+'\n')
  accel=[f for f in decoded['fields'] if f['type']=='input' and not f['constant'] and f['usage'] in [0x200453,0x200454,0x200455] and 0x200073 in f['collectionPath']]
  summary.append({'sourcePath':device['path'],'layoutFile':name+'.json','descriptorSHA256':decoded['sha256'],'accelerometerInputFields':accel})
 (out/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n')
 print('Decoded',len(summary),'real sensor descriptors; accelerometer fields:',sum(len(s['accelerometerInputFields']) for s in summary))
if __name__=='__main__':main()
