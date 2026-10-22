#!/usr/bin/env python3
"""Read-only Linux sensor inventory. No input grabbing or key-event capture."""
import argparse,datetime,json,os,platform,re,time
from pathlib import Path

def read(p):
    try:return p.read_text(errors='replace')[:65536].strip()
    except OSError as e:return {'error':str(e)}

def sensors(root=Path('/sys')):
    result=[]
    for base in sorted((root/'bus/iio/devices').glob('iio:device*')):
        values={}
        for p in base.iterdir():
            if p.is_file() and (p.name=='name' or re.fullmatch(r'in_(accel|angl|rot|incli|magn|gyro).*_(raw|input|scale|offset)',p.name) or 'mount_matrix' in p.name):values[p.name]=read(p)
        result.append({'path':str(base.resolve()),'values':values})
    return result

def inventory(root=Path('/sys')):
    hid=[]
    for base in sorted((root/'bus/hid/devices').glob('*')):
        meta=read(base/'uevent')
        if isinstance(meta,str) and ('hid-ishtp' in meta or '001F:8087:0AC2' in base.name):
            item={'path':str(base.resolve()),'uevent':meta}
            try:
                with (base/'report_descriptor').open('rb') as f:item['report_descriptor_hex']=f.read(65536).hex()
            except OSError as e:item['descriptor_error']=str(e)
            hid.append(item)
    return {'sensor_hid':hid,'iio':sensors(root)}

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('position',choices=['laptop','tablet','portrait-left','portrait-right','upside-down']);args=p.parse_args()
    if platform.system()!='Linux':p.error('Run this on the Dell under Linux; it does not change macOS.')
    out=Path(__file__).resolve().parent/'reports'/('linux-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S')+'-'+args.position)
    out.mkdir(parents=True,exist_ok=False)
    data={'position':args.position,'kernel':platform.release(),'inventory':inventory(),'input_devices':read(Path('/proc/bus/input/devices')),'samples':[]}
    print('Hold the Dell in '+args.position+' position for 5 seconds.',flush=True)
    for _ in range(10):data['samples'].append({'monotonic':time.monotonic(),'iio':sensors()});time.sleep(.5)
    (out/'sensors.json').write_text(json.dumps(data,indent=2)+'\n');print('Saved '+str(out))
if __name__=='__main__':main()
