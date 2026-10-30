from owned_monitor import owned_monitor
# SPDX-License-Identifier: MIT
"""Capture the actual owned outer-firmware display; never synthesize guest UI."""
from pathlib import Path
import datetime,json,socket,subprocess,hashlib,collections
r=Path(__file__).resolve().parent
a=json.loads((r/'ACTIVE-valid.json').read_text());d=Path(a['run']);args=json.loads((d/'valid/COMMAND.json').read_text())
monitor=args[args.index('-monitor')+1].split('unix:',1)[1].split(',')[0]
stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ');ppm=d/('display-'+stamp+'.ppm');png=ppm.with_suffix('.png');assert not ppm.exists()
with owned_monitor(d) as s:
 s.connect(monitor);s.settimeout(.2)
 def receive():
  out=b''
  while True:
   try:
    piece=s.recv(65536)
    if not piece:break
    out+=piece
   except socket.timeout:break
  return out.decode(errors='replace')
 receive();s.sendall(('screendump '+str(ppm)+'\n').encode());reply=receive()
assert ppm.is_file(),reply
b=ppm.read_bytes();magic,geom,limit,pixels=b.split(b'\n',3);assert magic==b'P6' and limit==b'255';w,h=map(int,geom.split());assert len(pixels)==w*h*3
counts=collections.Counter(pixels[i:i+3] for i in range(0,len(pixels),3))
subprocess.run(['/usr/bin/sips','-s','format','png',str(ppm),'--out',str(png)],check=True,capture_output=True)
report={'UTC':stamp,'run':str(d),'EFI_SHA256':a['EFI_SHA256'],'ppm':str(ppm),'png':str(png),'width':w,'height':h,'PPM_SHA256':hashlib.sha256(b).hexdigest(),'uniqueRGBColors':len(counts),'nonblackPixels':w*h-counts[b'\0\0\0'],'topColors':[(c.hex(),n) for c,n in counts.most_common(8)],'installerReached':False,'interpretation':'Actual display capture. Pixel statistics alone do not prove installer UI; inspect the PNG.'}
ppm.with_suffix('.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
