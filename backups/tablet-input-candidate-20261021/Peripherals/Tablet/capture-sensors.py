#!/usr/bin/env python3
"""Guided Dell live-Linux sensor capture. No typed keys, input grabs or disk installs."""
import array,datetime,fcntl,importlib.util,json,os,platform,re,subprocess,sys,time
from pathlib import Path
SPEC=importlib.util.spec_from_file_location('sensor_inventory',Path(__file__).with_name('collect-tablet-linux.py'))
INV=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(INV)

def command(args):
    try:
        p=subprocess.run(args,capture_output=True,text=True,timeout=8)
        return {'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr}
    except (OSError,subprocess.TimeoutExpired) as e:return {'error':str(e)}

def switch_devices(root=Path('/sys')):
    devices=[]
    for d in sorted((root/'class/input').glob('event*')):
        cap=INV.read(d/'device/capabilities/sw')
        try:bits=int(''.join(cap.split()),16)
        except (ValueError,AttributeError):continue
        if bits:devices.append({'event':d.name,'name':INV.read(d/'device/name'),'supportedSwitchBits':bits})
    return devices

def switches(devices):
    out=[]
    for d in devices:
        row=dict(d)
        try:
            fd=os.open('/dev/input/'+d['event'],os.O_RDONLY|os.O_NONBLOCK)
            try:
                state=array.array('B',[0]*8)
                # EVIOCGSW(8): query switch state only, never read input_event packets.
                fcntl.ioctl(fd,0x8008451b,state,True);bits=int.from_bytes(state.tobytes(),'little')
                row['stateBits']=bits
                row['tabletMode']=bool(bits&2) if d['supportedSwitchBits']&2 else None
                row['lidClosed']=bool(bits&1) if d['supportedSwitchBits']&1 else None
            finally:os.close(fd)
        except OSError as e:row['error']=str(e)
        out.append(row)
    return out

def worker(position,out):
    devs=switch_devices();out=Path(out)
    (out/(position+'-inventory.json')).write_text(json.dumps(INV.inventory(),indent=2)+'\n')
    with (out/(position+'-samples.jsonl')).open('w',buffering=1) as f:
        for i in range(10):
            f.write(json.dumps({'position':position,'sample':i,'monotonic':time.monotonic(),'iio':INV.sensors(),'switches':switches(devs)})+'\n');time.sleep(.5)
    return 0

def main():
    if platform.system()!='Linux' or os.geteuid()!=0:
        print('Run with sudo in the Dell Linux live session. No capture performed.',file=sys.stderr);return 2
    if len(sys.argv)==4 and sys.argv[1]=='--sample':return worker(sys.argv[2],sys.argv[3])
    if len(sys.argv)!=1:return 2
    root=Path(__file__).resolve().parent
    out=root/'reports'/('guided-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));out.mkdir(parents=True,exist_ok=False)
    print('Dell orientation capture. Logs stay on LATSENSORS; no OS installation.',flush=True)
    module_results={name:command(['modprobe',name]) for name in ['intel_ish_ipc','intel_ishtp_hid','hid_sensor_hub','hid_sensor_accel_3d','hid_sensor_incl_3d','hid_sensor_rotation']}
    modules={'kernel':platform.release(),'cmdline':INV.read(Path('/proc/cmdline')),'modules':module_results,'inputDevices':INV.read(Path('/proc/bus/input/devices')),'switchDevices':switch_devices()}
    modules['pci']=command(['lspci','-nnk']);modules['loadedModules']=command(['lsmod'])
    for key in ['product_name','board_name','bios_version']:modules[key]=INV.read(Path('/sys/class/dmi/id')/key)
    parameters=Path('/sys/module/intel_hid/parameters')
    modules['intelHidParameters']={p.name:INV.read(p) for p in parameters.glob('*') if p.is_file()}
    (out/'system.json').write_text(json.dumps(modules,indent=2)+'\n')
    phases=[('laptop','Normal laptop position, screen upright.'),('tablet','Fold fully into tablet mode; keep the screen landscape and upright.'),('portrait-left','While folded, rotate the screen 90 degrees counterclockwise from landscape.'),('portrait-right','While folded, rotate the screen 90 degrees clockwise from normal landscape.'),('laptop-return','Return to normal laptop position, screen upright.')]
    print('Put the Dell in normal laptop position. Press Enter once to begin.',flush=True)
    try:input()
    except EOFError:print('Interactive Terminal required; stopped before positional capture.');return 2
    results=[]
    for position,instruction in phases:
        print('\n'+instruction+' You have 10 seconds to position it, then HOLD until the next message.',flush=True)
        for seconds in [10,5]:print(str(seconds)+' seconds...',flush=True);time.sleep(5)
        print('Capturing '+position+'; keep it still.',flush=True)
        with (out/(position+'-worker.txt')).open('w') as log:
            child=subprocess.Popen([sys.executable,str(Path(__file__).resolve()),'--sample',position,str(out)],stdout=log,stderr=subprocess.STDOUT)
            try:rc=child.wait(timeout=20)
            except subprocess.TimeoutExpired:
                child.kill()
                try:child.wait(timeout=1)
                except subprocess.TimeoutExpired:pass
                rc=124
        results.append({'position':position,'exit':rc})
        print('Capture result '+str(rc),flush=True)
        if rc==124:print('Sensor query timed out; return to laptop position. Partial logs retained.',flush=True);break
    kernel=command(['dmesg','--color=never'])
    if 'stdout' in kernel:kernel['stdout']='\n'.join(l for l in kernel['stdout'].splitlines() if re.search(r'ish|sensor|tablet|intel.hid|hid.sensor|firmware',l,re.I))
    (out/'sensor-kernel.json').write_text(json.dumps(kernel,indent=2)+'\n')
    acpi=out/'ACPI';acpi.mkdir()
    for p in Path('/sys/firmware/acpi/tables').glob('*'):
        if p.is_file() and (p.name=='DSDT' or p.name.startswith('SSDT')):
            try:(acpi/p.name).write_bytes(p.read_bytes())
            except OSError:pass
    inventories=[]
    for p in out.glob('*-inventory.json'):
        try:inventories.append(json.loads(p.read_text()))
        except (OSError,ValueError):pass
    has_axes=any(all('in_accel_'+axis+'_raw' in device.get('values',{}) for axis in ['x','y','z']) for inv in inventories for device in inv.get('iio',[]))
    has_tablet=any(device['supportedSwitchBits']&2 for device in switch_devices())
    summary={'accelerometerAxesPresent':has_axes,'tabletSwitchPresent':has_tablet,'sensorDataNeedsReview':True,'phases':results,'complete':len(results)==len(phases) and all(x['exit']==0 for x in results),'scope':'Sensor descriptors, IIO values, EV_SW state and firmware tables only. No typed keys or input_event reads.'}
    if not has_axes or not has_tablet:print('Some required orientation or mode channels are absent; the saved report will show what the kernel exposed.',flush=True)
    (out/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n');os.sync()
    print('\nSAVED: '+str(out),flush=True);print('Return to laptop position, shut down Linux, and reconnect this USB to the Mac. Do not install Fedora.',flush=True)
    return 0 if summary['complete'] else 1
if __name__=='__main__':raise SystemExit(main())
