from pathlib import Path
import subprocess,json,sys,hashlib
r=Path(__file__).resolve().parent;w=r.parent;u=w/'framebuffer-engine';clang='/Library/Developer/CommandLineTools/usr/bin/clang'
(r/'UnicornTest.efi').unlink(missing_ok=True)
(r/'UnicornTest.elf').unlink(missing_ok=True)
(r/'BUILD.json').unlink(missing_ok=True)
proof=json.loads((r/'ENGINE-COMPILE.json').read_text())
assert subprocess.check_output(['git','rev-parse','HEAD'],cwd=u,text=True).strip()==proof['revision']
for name,digest in proof['modifiedFiles'].items():assert hashlib.sha256((u/name).read_bytes()).hexdigest()==digest,'engine rebuild required: '+name
results=json.loads((r/'COMPILE.json').read_text());assert all(x['exit']==0 for x in results);assert len({x['object'] for x in results})==len(results)
for item in results:
 assert hashlib.sha256((u/item['source']).read_bytes()).hexdigest()==item['sourceSHA256']
 assert hashlib.sha256((r/item['object']).read_bytes()).hexdigest()==item['objectSHA256']
subprocess.run([str(w/'deps/bin/ld.lld'),'-r',*[str(r/x['object']) for x in results],'-o',str(r/'UnicornA64.o')],check=True)
incs=[u/'efi/include',u/'efi/include/x86_64',u/'include',w/'edk2/MdePkg/Include',w/'edk2/MdePkg/Include/X64']
extra=['-DTEST_VMAPPLE_ENTRY'] if '--vmapple' in sys.argv else []
if '--fast' in sys.argv:extra+=['-DFAST_KERNEL_TRACE']
if '--software-compositor' in sys.argv:
 assert '--framebuffer' in sys.argv
 extra+=['-DTEST_SOFTWARE_COMPOSITOR']
if '--assert-failure' in sys.argv:extra+=['-DTEST_ASSERT_GUARD']
if '--boot-context' in sys.argv:
 assert '--hfs' in sys.argv
 extra+=['-DTEST_BOOT_CONTEXT_PROFILE']
if '--hfs' in sys.argv:
 assert '--storage' in sys.argv
 extra+=['-DTEST_HFS_PROFILE']
if '--storage' in sys.argv:
 assert '--framebuffer' in sys.argv
 extra+=['-DTEST_STORAGE_PROFILE']
if '--framebuffer' in sys.argv:
 assert '--installer' in sys.argv
 extra+=['-DTEST_FRAMEBUFFER_PROFILE']
if '--installer' in sys.argv:
 assert '--vmapple' in sys.argv and '--fast' in sys.argv
 extra+=['-DTEST_INSTALLER_PROFILE']
base=[clang,'-target','x86_64-unknown-linux-gnu','-std=gnu11','-ffreestanding','-fshort-wchar','-mno-red-zone','-fno-stack-protector','-fno-builtin','-O1','-DNO_MSABI_VA_FUNCS','-DEFIAPI=__attribute__((ms_abi))','-fPIC','-fno-asynchronous-unwind-tables']
args=base+extra+['-DDELL27_ALWAYS_ON_ASSERTS']+['-DUNICORN_FOR_EFI','-DUNICORN_FOR_EFI_INTERNAL','-nostdinc']
for i in incs:args+=['-I',str(i)]
subprocess.run(args+['-c',str(r/'harness.c'),'-o',str(r/'harness.o')],check=True)
subprocess.run(base+['-I',str(w/'edk2/MdePkg/Include'),'-I',str(w/'edk2/MdePkg/Include/X64'),'-I',str(w/'nanoprintf'),'-c',str(r/'bridge.c'),'-o',str(r/'bridge.o')],check=True)
subprocess.run(args+['-c',str(r/'boot-entry.c'),'-o',str(r/'boot-entry.o')],check=True)
subprocess.run(args+['-c',str(w/'Standalone/boot_input.c'),'-o',str(r/'boot_input.o')],check=True)
subprocess.run(args+['-c',str(r/'sprr-tests.c'),'-o',str(r/'sprr-tests.o')],check=True)
subprocess.run(args+['-c',str(r/'pac-tests.c'),'-o',str(r/'pac-tests.o')],check=True)
subprocess.run(args+['-c',str(r/'monitor-rng.c'),'-o',str(r/'monitor-rng.o')],check=True)
subprocess.run(args+['-c',str(r/'vmapple-entry.c'),'-o',str(r/'vmapple-entry.o')],check=True)
for name in ['service-observer-tests','compositor-policy-tests','cow-tests','virtio-root','virtio-root-tests','gop-framebuffer','iosfc','iosfc-tests','pl061-gpio','gpio-tests','pci-ecam','pci-tests','pl031-rtc','pl031-tests','vmapple-rtc','rtc-tests','vmapple-aes','aes-tests','vmapple-config','config-tests','qarma-tests','vmapple-bdif','utility-file','bdif-tests','pstate-tests','file-source','host-memory','sparse-ram','sparse-ram-tests','polled-uart','pl011-console','uart-tests','exception-tests','vmapple-gic','gic-tests']:
 subprocess.run(args+['-c',str(r/(name+'.c')),'-o',str(r/(name+'.o'))],check=True)
objects=[r/'service-observer-tests.o',r/'compositor-policy-tests.o',r/'cow-tests.o',r/'virtio-root.o',r/'virtio-root-tests.o',r/'gop-framebuffer.o',r/'iosfc.o',r/'iosfc-tests.o',r/'pl061-gpio.o',r/'gpio-tests.o',r/'pci-ecam.o',r/'pci-tests.o',r/'pl031-rtc.o',r/'pl031-tests.o',r/'vmapple-rtc.o',r/'rtc-tests.o',r/'vmapple-aes.o',r/'aes-tests.o',r/'vmapple-config.o',r/'config-tests.o',r/'qarma-tests.o',r/'vmapple-bdif.o',r/'utility-file.o',r/'bdif-tests.o',r/'pstate-tests.o',r/'file-source.o',r/'host-memory.o',r/'vmapple-gic.o',r/'gic-tests.o',r/'pl011-console.o',r/'exception-tests.o',r/'polled-uart.o',r/'uart-tests.o',r/'sparse-ram.o',r/'sparse-ram-tests.o',r/'monitor-rng.o',r/'vmapple-entry.o',r/'pac-tests.o',r/'sprr-tests.o',r/'boot-entry.o',r/'boot_input.o',r/'UnicornA64.o' ,r/'bridge.o',r/'SetJump.o',r/'LongJump.o',r/'harness.o']
subprocess.run([str(w/'deps/bin/ld.lld'),'-static','--no-relax','-Bsymbolic','--emit-relocs','--no-dynamic-linker','-z','notext','-z','common-page-size=0x1000','--defsym=PECOFF_HEADER_SIZE=0x228','-T',str(w/'edk2/BaseTools/Scripts/GccBase.lds'),'-e','efi_main',*[str(p) for p in objects],'-o',str(r/'UnicornTest.elf')],check=True)
subprocess.run([str(w/'edk2/BaseTools/Source/C/bin/GenFw'),'-e','UEFI_APPLICATION','-o',str(r/'UnicornTest.efi'),str(r/'UnicornTest.elf')],check=True)
print('Linked and converted full engine: UnicornTest.efi')

(r/'BUILD.json').write_text(json.dumps({'mode':'assert-failure-probe' if '--assert-failure' in sys.argv else 'vmapple-boot-context-hfs-storage-framebuffer-block-trace' if '--boot-context' in sys.argv else 'vmapple-hfs-storage-framebuffer-block-trace' if '--hfs' in sys.argv else 'vmapple-storage-framebuffer-block-trace' if '--storage' in sys.argv else 'vmapple-framebuffer-block-trace' if '--framebuffer' in sys.argv else 'vmapple-installer-block-trace' if '--installer' in sys.argv else 'vmapple-block-trace' if '--fast' in sys.argv else 'vmapple' if extra else 'diagnostic','experimentalCompatibility':['exact-build-no-Metal-timeout-native-cleanup'] if '--software-compositor' in sys.argv else [],'EFI_SHA256':hashlib.sha256((r/'UnicornTest.efi').read_bytes()).hexdigest(),'engineCompile':proof,'sourceSHA256':{str(p.relative_to(r)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(list(r.iterdir())+list((r/'Performance').glob('*.inc'))+list((r/'InstallerExperiment').glob('*.h'))+list((r/'FramebufferExperiment').glob('*.h'))+list((r/'StorageExperiment').glob('*.h'))+list((r/'HFSExperiment').glob('*.h'))+list((r/'BootContextExperiment').glob('*.h'))) if p.suffix in ['.c','.h','.inc']}},indent=2)+'\n')
