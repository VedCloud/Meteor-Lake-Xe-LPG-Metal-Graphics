#!/usr/bin/env python3
"""Dell ELAN-only Linux diagnostics; no event grabbing, keyboard events or coordinate traces saved."""
import array,datetime,fcntl,json,os,platform,re,select,struct,subprocess,sys,time
from pathlib import Path
EVENT=struct.Struct('@llHHi')
VENDOR,PRODUCT=0x04f3,0x328e

def read(path,limit=1048576):
    try:
        p=subprocess.run(['head','-c',str(limit),str(path)],capture_output=True,timeout=3)
        return p.stdout.decode(errors='replace').strip() if p.returncode==0 else {'error':p.stderr.decode(errors='replace').strip()}
    except (OSError,subprocess.TimeoutExpired) as e:return {'error':str(e)}

def command(args):
    try:
        p=subprocess.run(args,capture_output=True,text=True,timeout=8);return {'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr}
    except (OSError,subprocess.TimeoutExpired) as e:return {'error':str(e)}

def discover(root=Path('/sys')):
    out=[]
    for e in sorted((root/'class/input').glob('event*')):
        d=e/'device'
        try:vendor=int((d/'id/vendor').read_text().strip(),16);product=int((d/'id/product').read_text().strip(),16);bus=int((d/'id/bustype').read_text().strip(),16);name=(d/'name').read_text().strip()
        except (OSError,ValueError):continue
        if vendor==VENDOR and product==PRODUCT and bus==0x18 and 'touchpad' in name.lower():out.append({'event':e.name,'name':name,'path':str(d.resolve()),'vendor':vendor,'product':product,'bus':bus})
    return out

def routing(target):
    result=[]
    for d in [Path(target['path']),*Path(target['path']).parents]:
        if str(d) in ['/sys','/']:break
        if not str(d).startswith('/sys/'):continue
        row={'path':str(d)}
        for name in ['name','modalias','irq','uevent','power/control','power/runtime_status','power/runtime_active_time','power/runtime_suspended_time','firmware_node/path','firmware_node/hid','firmware_node/status','firmware_node/modalias']:
            p=d/name
            if p.is_file():row[name]=read(p,65536)
        for name in ['driver','firmware_node','subsystem']:
            p=d/name
            if p.is_symlink():row[name+'Path']=str(p.resolve())
        if len(row)>1:result.append(row)
    return result

def descriptor(target):
    for d in [Path(target['path']),*Path(target['path']).parents]:
        p=d/'report_descriptor'
        if p.is_file():
            try:
                with p.open('rb') as f:return {'path':str(p),'hex':f.read(65536).hex()}
            except OSError as e:return {'error':str(e)}
    return {'error':'No HID descriptor found on the selected touchpad ancestry'}

def snapshot(out,stage):
    (out/(stage+'-interrupts.txt')).write_text(str(read(Path('/proc/interrupts')))+'\n')
    (out/(stage+'-softirqs.txt')).write_text(str(read(Path('/proc/softirqs')))+'\n')
    logs=command(['dmesg','--color=never'])
    if 'stdout' in logs:
        lines=[]
        for line in logs['stdout'].splitlines():
            if re.search(r'i2c.hid|hid.multitouch|elan|04f3|i8042|atkbd|pinctrl|gpio|i2c.designware',line,re.I) and not re.search(r'key pressed|key released|scancode|keycode',line,re.I):lines.append(line)
        logs['stdout']='\n'.join(lines)
    (out/(stage+'-kernel.json')).write_text(json.dumps(logs,indent=2)+'\n')

def absinfo(fd,code):
    buf=bytearray(24);fcntl.ioctl(fd,0x80184500+0x40+code,buf,True)
    value,minimum,maximum,fuzz,flat,resolution=struct.unpack('=iiiiii',buf)
    return value,{'minimum':minimum,'maximum':maximum,'fuzz':fuzz,'flat':flat,'resolution':resolution}

def contacts(fd,slots):
    data=array.array('i',[57]+[0]*slots);request=0x8000450a|((len(data)*data.itemsize)<<16)
    fcntl.ioctl(fd,request,data,True);return {i:v for i,v in enumerate(data[1:]) if v>=0}

class Metrics:
    def __init__(self,slots=0,initial=None):
        self.slots=slots;self.current=0;self.active=dict(initial or {});self.contactReliable=initial is not None;self.desync=False;self.frames=0;self.dropped=0;self.absEvents=0;self.pointerChanges=0;self.maxContacts=len(self.active);self.touchFrames=0;self.gaps=[];self.previous=None;self.clockResets=0;self.resyncs=0
    def feed(self,event):
        sec,usec,typ,code,value=event
        if typ==0 and code==3:self.dropped+=1;self.desync=True;self.contactReliable=False;self.previous=None;return
        if self.desync:
            if typ==0 and code==0:self.desync=False;return 'resync'
            return
        if typ==3:
            self.absEvents+=1
            if code in [0,1,53,54]:self.pointerChanges+=1
            if code==47:
                if 0<=value<self.slots:self.current=value
                else:self.contactReliable=False
            if code==57 and self.contactReliable:
                if value<0:self.active.pop(self.current,None)
                else:self.active[self.current]=value
        if typ==0 and code==0:
            stamp=sec+usec/1e6;self.frames+=1
            if self.previous is not None:
                gap=(stamp-self.previous)*1000
                if gap>=0:self.gaps.append(gap)
                else:self.clockResets+=1
            self.previous=stamp
            if self.contactReliable:self.maxContacts=max(self.maxContacts,len(self.active));self.touchFrames+=bool(self.active)
    def summarize(self):
        values=sorted(self.gaps)
        def pct(p):return values[min(len(values)-1,int((len(values)-1)*p))] if values else None
        return {'frames':self.frames,'absoluteEvents':self.absEvents,'positionChangeEvents':self.pointerChanges,'maximumConcurrentContacts':self.maxContacts if self.contactReliable else None,'contactCountsReliable':self.contactReliable,'touchFrames':self.touchFrames,'SYN_DROPPED':self.dropped,'stateResyncs':self.resyncs,'eventGapMs':{'median':pct(.5),'p95':pct(.95),'maximum':max(values) if values else None,'over100ms':sum(v>100 for v in values)},'clockResets':self.clockResets,'note':'Delivery gaps include intentional pauses; these are not WindowServer frame times. No coordinates or tracking IDs retained.'}

def capture(fd,seconds,slots):
    try:initial=contacts(fd,slots) if slots else None
    except OSError:initial=None
    metric=Metrics(slots,initial)
    if slots:
        try:
            current,_=absinfo(fd,47)
            if 0<=current<slots:metric.current=current
            else:metric.contactReliable=False
        except OSError:metric.contactReliable=False
    pending=b'';deadline=time.monotonic()+seconds
    while time.monotonic()<deadline:
        if not select.select([fd],[],[],min(.25,max(0,deadline-time.monotonic())))[0]:continue
        try:data=os.read(fd,EVENT.size*512)
        except BlockingIOError:continue
        if not data:raise OSError('Selected touchpad event device disconnected')
        pending+=data
        while len(pending)>=EVENT.size:
            packet,pending=pending[:EVENT.size],pending[EVENT.size:]
            action=metric.feed(EVENT.unpack(packet))
            if action=='resync':
                try:
                    metric.active=contacts(fd,slots);current,_=absinfo(fd,47)
                    metric.contactReliable=0<=current<slots;metric.current=current if metric.contactReliable else 0;metric.resyncs+=1
                except OSError:metric.contactReliable=False
    result=metric.summarize();result['partialEventBytes']=len(pending);return result

def main():
    if platform.system()!='Linux' or os.geteuid()!=0:print('Run with sudo in the Dell Linux live desktop.');return 2
    root=Path(__file__).resolve().parent;out=root/'reports'/('touchpad-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));out.mkdir(parents=True,exist_ok=False)
    devices=discover();(out/'discovery.json').write_text(json.dumps(devices,indent=2)+'\n')
    snapshot(out,'before')
    if len(devices)!=1:print('Expected one Dell ELAN 04f3:328e I2C touchpad; found '+str(len(devices))+'. Saved discovery/errors to '+str(out));return 2
    target=devices[0];meta={'target':target,'routing':routing(target),'hidDescriptor':descriptor(target),'kernel':platform.release(),'inputDevices':read(Path('/proc/bus/input/devices')),'libinputDevices':command(['libinput','list-devices']),'lspci':command(['lspci','-nnk'])}
    debug=Path('/sys/kernel/debug');meta['debugfsMount']=command(['findmnt','-n','-t','debugfs','-o','TARGET'])
    if meta['debugfsMount'].get('exit')!=0:meta['debugfsMountAttempt']=command(['mount','-t','debugfs','-o','ro,nosuid,nodev,noexec','debugfs',str(debug)])
    gpio=out/'gpio';gpio.mkdir()
    if (debug/'gpio').is_file():(gpio/'gpio.txt').write_text(str(read(debug/'gpio'))+'\n')
    for d in list(sorted((debug/'pinctrl').glob('*')))[:8]:
        if not d.is_dir():continue
        for name in ['pins','pinmux-pins','pinconf-pins','gpio-ranges']:
            p=d/name
            if p.is_file():(gpio/(d.name+'-'+name+'.txt')).write_text(str(read(p))+'\n')
    acpi=out/'ACPI';acpi.mkdir()
    for p in Path('/sys/firmware/acpi/tables').glob('*'):
        if p.is_file() and (p.name=='DSDT' or p.name.startswith('SSDT')):
            try:(acpi/p.name).write_bytes(p.read_bytes())
            except OSError:pass
    fd=os.open('/dev/input/'+target['event'],os.O_RDONLY|os.O_NONBLOCK)
    stages=[]
    try:
        identity=bytearray(8);fcntl.ioctl(fd,0x80084502,identity,True);bus,vendor,product,version=struct.unpack('=HHHH',identity)
        if (bus,vendor,product)!=(0x18,VENDOR,PRODUCT):raise RuntimeError('Event node identity changed; refusing to monitor it')
        namebuf=bytearray(256);fcntl.ioctl(fd,0x81004506,namebuf,True);opened_name=namebuf.split(b'\0',1)[0].decode(errors='replace')
        if 'touchpad' not in opened_name.lower():raise RuntimeError('Opened event node is not named Touchpad; refusing to monitor it')
        meta['openedName']=opened_name
        meta['openedIdentity']={'bus':bus,'vendor':vendor,'product':product,'version':version};meta['absoluteRanges']={};slots=0
        for code in [0,1,47,53,54,57]:
            try:
                _,fields=absinfo(fd,code);meta['absoluteRanges'][str(code)]=fields
                if code==47 and fields['minimum']==0 and 0<=fields['maximum']<32:slots=fields['maximum']+1
            except OSError:pass
        (out/'hardware.json').write_text(json.dumps(meta,indent=2)+'\n')
        print('Selected '+target['name']+'. Keep the laptop unfolded. No keyboard events or coordinate traces will be saved.',flush=True)
        print('Press Enter to start five short gesture phases. Then follow prompts; no more typing is needed.',flush=True);input()
        for name,prompt in [('one-finger','Move one finger smoothly on the touchpad'),('two-finger-scroll','Scroll up/down with two fingers'),('pinch','Pinch in/out with two fingers'),('three-finger','Swipe with three fingers'),('four-finger','Swipe with four fingers')]:
            print('\nLift fingers. Next: '+prompt+'. Starting in 3 seconds.',flush=True)
            capture(fd,3,slots) # Drain, without grabbing, while fingers are lifted.
            print(prompt+' NOW for 8 seconds.',flush=True)
            row={'gesture':name,**capture(fd,8,slots)};stages.append(row);(out/'gestures.json').write_text(json.dumps(stages,indent=2)+'\n');print('Phase captured.',flush=True)
    except (OSError,EOFError,RuntimeError,KeyboardInterrupt) as e:
        (out/'capture-error.txt').write_text(str(e)+'\n');print('Capture stopped; partial results retained: '+str(e),flush=True)
    finally:os.close(fd)
    snapshot(out,'after')
    summary={'completedPhases':len(stages),'complete':len(stages)==5,'scope':'ELAN I2C touchpad only. No EVIOCGRAB, configuration/power changes, key logging or coordinate traces. Read-only debugfs may be mounted for GPIO diagnostics.','report':str(out)}
    (out/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n');os.sync();print('\nSAVED: '+str(out),flush=True);print('Shut down Linux and return the SanDisk USB to the Mac.',flush=True);return 0 if summary['complete'] else 1
if __name__=='__main__':raise SystemExit(main())
