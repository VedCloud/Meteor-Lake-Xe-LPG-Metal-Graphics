def package(data,at):
 first=data[at];more=first>>6;length=(first&0x3f) if not more else first&15
 for j in range(more):length|=data[at+j+1]<<(4+8*j)
 return more+1,length

def methods(data,start=36,end=None):
 if end is None:end=len(data)
 result={}
 for i in range(start,end-8):
  if data[i]!=0x14:continue
  size,length=package(data,i+1);name=i+1+size;stop=i+1+length
  if stop<=end and name+5<=stop and data[name:name+4] in [b'_DSM',b'_CRS']:
   key=bytes(data[name:name+4]);assert key not in result,(key,result[key],i)
   result[key]=(name+4,stop)
 return result

def device_range(data,name):
 found=[]
 for i in range(36,len(data)-8):
  if data[i:i+2]!=b'\x5b\x82':continue
  size,length=package(data,i+2);start=i+2+size
  if data[start:start+4]==name:found.append((start+4,i+2+length))
 assert len(found)==1,found
 return found[0]

def replace_bodies(destination,source_bodies,start=36,end=None):
 out=bytearray(destination);slots=methods(out,start,end);changes=[]
 for name,body in source_bodies.items():
  lo,hi=slots[name];assert len(body)<=hi-lo,(name,len(body),hi-lo)
  # Preserve package sizes and namespace placement; trailing AML Noops are harmless.
  out[lo:hi]=body+bytes([0xa3])*(hi-lo-len(body));changes.append({'method':name.decode(),'flags_offset':lo,'end':hi,'replacement_bytes':len(body),'available_bytes':hi-lo})
 out[9]=0;out[9]=(-sum(out))&255
 assert len(out)==len(destination) and sum(out)%256==0
 return out,changes
