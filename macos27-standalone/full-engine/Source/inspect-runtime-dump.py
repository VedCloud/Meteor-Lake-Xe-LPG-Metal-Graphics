# SPDX-License-Identifier: MIT
"""Read-only ELF/PE/sparse-RAM inspection of the owned firmware test dump.
Does not read or print PAC key material. It exports only mapped instruction bytes.
"""
from pathlib import Path
import mmap,struct,json,subprocess
R=Path(__file__).resolve().parent
D=Path('/Volumes/Corsair 1TB NVMe/Dell-macOS27-Research/RuntimeDumps/20261023T231822Z-61816-PAC-failure.elf')
f=D.open('rb');m=mmap.mmap(f.fileno(),0,access=mmap.ACCESS_READ)
assert m[:6]==b'\x7fELF\x02\x01';phoff=struct.unpack_from('<Q',m,32)[0];phsize,phnum=struct.unpack_from('<HH',m,54)
segments=[]
for i in range(phnum):
 typ,flags,off,va,pa,filesz,memsz,align=struct.unpack_from('<IIQQQQQQ',m,phoff+i*phsize)
 if typ==1:segments.append((pa,off,filesz))
def read(pa,n):
 for base,off,size in segments:
  if base<=pa and pa+n<=base+size:return m[off+pa-base:off+pa-base+n]
 raise ValueError('outer physical range missing '+hex(pa)+' '+hex(n))
def u64(p):return struct.unpack('<Q',read(p,8))[0]
def phys(off):
 return next(base+off-start for base,start,n in segments if start<=off<start+n)
pe=(R/'FirmwareRuns/20261023T231822Z-61816/executed.efi').read_bytes();needle=b'VMAPPLE adaptive host commit ceiling=';at=pe.index(needle);peoff=struct.unpack_from('<I',pe,60)[0];nsec=struct.unpack_from('<H',pe,peoff+6)[0];optsz=struct.unpack_from('<H',pe,peoff+20)[0]
sections=[]
for i in range(nsec):
 s=peoff+24+optsz+40*i;vsize,rva,rawsize,raw=struct.unpack_from('<IIII',pe,s+8);sections.append((rva,raw,rawsize))
needle_rva=next(rva+at-raw for rva,raw,n in sections if raw<=at<raw+n);candidates=[];offset=0
while True:
 hit=m.find(needle,offset)
 if hit<0:break
 offset=hit+1;base=phys(hit)-needle_rva
 try:
  if read(base,2)!=b'MZ':continue
  ram=base+0x1da460;uc,guest_base,guest_size,limit,committed=struct.unpack('<QQQQQ',read(ram,40))
  if guest_base==0x800000000 and guest_size==0x400000000:candidates.append((base,ram,uc,guest_base,guest_size,limit,committed))
 except (ValueError,struct.error):continue
assert len(candidates)==1,candidates
base,ram,uc,guest_base,guest_size,limit,committed=candidates[0];mapped=u64(uc+0x760);count=struct.unpack('<I',read(uc+0x768,4))[0];assert count<1024
regions=[]
for i in range(count):
 mr=u64(mapped+8*i)
 if read(mr,1)!=b'\x01':continue
 rb=u64(mr+8);size=u64(mr+0x30);addr=u64(mr+0x40);host=u64(rb+8);length=u64(rb+0x18);assert size<=length
 regions.append((addr,size,host))
def armread(pa,n):
 for addr,size,host in regions:
  if addr<=pa and pa+n<=addr+size:return read(host+pa-addr,n)
 raise ValueError('ARM physical range missing '+hex(pa)+' '+hex(n))
cpu=u64(uc+0x190);env=cpu+0x9790;pc=u64(env+0x140);ttbr0=u64(env+0x308);ttbr1=u64(env+0x328);tcr=u64(env+0x358)
def translate(va):
 upper=bool(va>>63);tsz=(tcr>>16)&63 if upper else tcr&63;granule=(tcr>>30)&3 if upper else (tcr>>14)&3
 assert granule==(1 if upper else 2),(granule,hex(tcr))
 top=64-tsz;level=4-((top-14+10)//11);table=(ttbr1 if upper else ttbr0)&0x0000ffffffffc000
 while level<=3:
  shift=14+11*(3-level);index=(va>>shift)&0x7ff;pte=struct.unpack('<Q',armread(table+8*index,8))[0];assert pte&1,('invalid PTE',hex(va),level,hex(pte))
  if level==3 or not pte&2:
   mask=(1<<shift)-1;return (pte&0x0000ffffffffc000&~mask)|(va&mask)
  table=pte&0x0000ffffffffc000;level+=1
 raise ValueError('no leaf')
def virtual(va,n):
 out=bytearray()
 while n:
  part=min(n,0x4000-(va&0x3fff));out+=armread(translate(va),part);va+=part;n-=part
 return bytes(out)
print({'imageBase':hex(base),'UC':hex(uc),'CPU':hex(cpu),'env':hex(env),'PC':hex(pc),'TTBR0':hex(ttbr0),'TTBR1':hex(ttbr1),'TCR':hex(tcr),'mappedCount':count,'regions':len(regions)},flush=True)
fault=0x195772c54;start=fault-64;code=virtual(start,160);(R/'References/PAC-failure-guest-code.bin').write_bytes(code)
from capstone import Cs,CS_ARCH_ARM64,CS_MODE_ARM
text='\n'.join(f'{x.address:#x} {x.mnemonic} {x.op_str}' for x in Cs(CS_ARCH_ARM64,CS_MODE_ARM).disasm(code,start));(R/'References/PAC-failure-guest-code.txt').write_text(text+'\n')
report={'dump':str(D),'EFIImageBase':hex(base),'SparseRamGlobal':hex(ram),'committedBytes':committed,'ARMPC':hex(pc),'ARMRegions':len(regions),'TCR_EL1':hex(tcr),'TTBR0_EL1':hex(ttbr0),'TTBR1_EL1':hex(ttbr1),'faultPC':hex(fault),'codePhysical':hex(translate(start)),'keyFieldsDecodedOrExported':False};(R/'References/PAC-RUNTIME-DUMP-INSPECTION.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));print(text)
