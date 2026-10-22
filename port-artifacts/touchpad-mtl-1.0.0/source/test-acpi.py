from pathlib import Path
import tempfile,subprocess,re
r=Path(__file__).resolve().parent.parent
fixture='''DefinitionBlock ("", "SSDT", 2, "TEST", "LATTEST", 1) {
 External (\\_SB.PC00.I2C3.TPD0._DSM, MethodObj)
 External (\\_SB.PC00.I2C3.TPD0._CRS, MethodObj)
 Name (TPDH, 0x1234)
 Scope (\\_SB) { Device (PC00) { Name (_HID, "TEST0001")
  Device (I2C3) { Name (_HID, "TEST0002")
   Device (TPD0) { Name (_HID, "TEST0003")
    Name (SBFB, ResourceTemplate () { I2cSerialBusV2 (0x15,ControllerInitiated,400000,AddressingMode7Bit,"\\\\_SB.PC00.I2C3",0,ResourceConsumer,,Exclusive,) })
    Name (SBFI, ResourceTemplate () { Interrupt(ResourceConsumer,Level,ActiveLow,Exclusive) { 35 } })
 } } } }
 Method (TEST, 0, Serialized) {
  Local0 = \\_SB.PC00.I2C3.TPD0._DSM (ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"),One,One,Package(){})
  If (Local0 != 0x1234) { Return (One) }
  Local0 = \\_SB.PC00.I2C3.TPD0._DSM (ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"),One,Zero,Package(){})
  CreateByteField(Local0,Zero,FUN0)
  If(FUN0 != 3) { Return (0x02) }
  Local0 = \\_SB.PC00.I2C3.TPD0._CRS ()
  Local1 = ConcatenateResTemplate(\\_SB.PC00.I2C3.TPD0.SBFB,\\_SB.PC00.I2C3.TPD0.SBFI)
  If(SizeOf(Local0)!=SizeOf(Local1)) {Return(0x03)}
  Local2=Zero
  While(Local2<SizeOf(Local0)) {
   If(DerefOf(Index(Local0,Local2)) != DerefOf(Index(Local1,Local2))) {Return(0x04)}
   Increment(Local2)
  }
  Return (0x55AA)
 }
}'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);f=p/'fixture.dsl';f.write_text(fixture)
 c=subprocess.run(['/opt/homebrew/bin/iasl','-p',str(p/'fixture'),str(f)],capture_output=True,text=True);assert not c.returncode,c.stdout+c.stderr
 c=subprocess.run(['/opt/homebrew/bin/acpiexec','-b','execute \\TEST',str(p/'fixture.aml'),str(r/'touchpad-candidate/ACPI/SSDT-LAT-TPD0.aml')],capture_output=True,text=True,timeout=15)
 assert not c.returncode and '00000000000055AA' in c.stdout,c.stdout+c.stderr
 print('PASS actual SSDT in ACPICA interpreter: firmware HID address returned, function support bitmap correct, _CRS exactly preserves firmware serial-bus and interrupt resources; no hardware execution')
