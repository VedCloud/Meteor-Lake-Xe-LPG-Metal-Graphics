from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;out=r/'desktop-build';source=(r/'desktop-sources/SSDT-LAT-BRT.dsl').read_text()
assert 'XQ66 ()' in source and 'GENS (0x11, Zero, Zero)' in source
mock=r'''DefinitionBlock ("", "DSDT", 2, "LAT535", "KEYMOCK", 1){
 External (SMEE, MethodObj)
 External (\_SB.PC00.GFX0.LCD.LDBG, MethodObj)
 External (\_SB.PC00.LPCB.ECDV._Q66, MethodObj)
 External (LQCT, IntObj) External (LSCT, IntObj) External (LSFL, IntObj) External (LBCT, IntObj) External (LBAR, IntObj)
 External (\_SB.PC00.GFX0.BRT6, MethodObj)
 Name (OLDN, Zero)
 Scope (\_SB){Device(AMW0){Name(_HID,"PNP0C14")}}
 Method (GENS,3){Return(4)}
 Method (EV6,2){\_SB.PC00.GFX0.BRT6(Arg0,Arg1)}
 Method (EV19,2){}
 Method (XSME,1){Increment(OLDN)}
 Scope (\_SB){Device(PC00){Name(_HID,"TEST0001")
  Device(GFX0){Name(_HID,"TEST0002") Device(LCD){Name(_ADR,0x0400)} Method(XBR6,2){Increment(OLDN)}}
  Device(LPCB){Name(_HID,"TEST0003") Device(PS2K){Name(_HID,"PNP0303")} Device(ECDV){Name(_HID,"PNP0C09") Method(XQ66,0){SMEE(Zero)}}}
 }}
 Method(TEST,0){\_SB.PC00.LPCB.ECDV._Q66()
  If(LQCT!=1){Return(0xB001)} If(LSCT!=1){Return(0xB002)} If(LSFL!=4){Return(0xB003)} If(LBCT!=1){Return(0xB004)} If(LBAR!=1){Return(0xB005)} Local0=\_SB.PC00.GFX0.LCD.LDBG() If(SizeOf(Local0)!=5){Return(0xB008)} If(DerefOf(Index(Local0,0))!=1){Return(0xB009)} \_SB.PC00.GFX0.BRT6(2,Zero) Return(0x600D)
 }
 Method(FALL,0){\_SB.PC00.LPCB.ECDV._Q66() If(OLDN!=1){Return(0xB006)} If(LQCT!=1){Return(0xB007)} Return(0x600E)}
}'''
f=out/'hotkey-mock.dsl';f.write_text(mock)
for src,prefix in [(f,out/'hotkey-mock'),(r/'desktop-sources/SSDT-LAT-BRT.dsl',out/'hotkey-counters')]:
 subprocess.run(['iasl','-p',str(prefix),str(src)],check=True,capture_output=True)
a=(out/'hotkey-counters.aml').read_bytes();assert sum(a)%256==0
result=subprocess.run(['acpiexec','-b','osi install Darwin;execute TEST;osi remove Darwin;execute FALL',str(out/'hotkey-mock.aml'),str(out/'hotkey-counters.aml')],capture_output=True,text=True,timeout=10)
s=result.stdout+result.stderr
assert '000000000000600D' in s and '000000000000600E' in s,s
assert 'AE_NOT_FOUND' not in s,s
assert 'PS2K' in s and 'Value 0x366 ' in s and 'Value 0x365 ' in s,s
assert 'Notify (\\_SB.PC00.GFX0.LCD,' not in source
assert 'Notify (\\_SB.PC00.LPCB.PS2K, 0x0366)' in source and 'Notify (\\_SB.PC00.LPCB.PS2K, 0x0365)' in source
print('PASS actual AML with ACPICA: Q66 -> original XQ66 -> SMEE -> EV6 -> BRT6, five counters, PS2 make/break notifications for brightness up/down, Darwin branch and original non-Darwin fallback. No hardware accessed.')
