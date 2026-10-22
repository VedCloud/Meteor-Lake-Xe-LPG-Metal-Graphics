DefinitionBlock ("", "SSDT", 2, "LAT535", "BRTKEYS", 0x00000002)
{
    External (\_SB.PC00.GFX0, DeviceObj)
    External (\_SB.PC00.GFX0.XBR6, MethodObj)
    External (\_SB.PC00.GFX0.LCD, DeviceObj)
    External (\XSME, MethodObj)
    External (\GENS, MethodObj)
    External (\EV6, MethodObj)
    External (\EV19, MethodObj)
    Method (SMEE, 1, NotSerialized)
    {
        If (_OSI ("Darwin"))
        {
            Local0 = GENS (0x11, Zero, Zero)
            If (Local0 & 0x04) { EV6 (One, Zero) }
            If (Local0 & 0x02) { EV6 (0x02, Zero) }
            If (Local0 & 0x08)
            {
                Local0 = GENS (0x1D, Zero, Zero)
                EV19 (Local0, Zero)
            }
        }
        Else { XSME (Arg0) }
    }
    Scope (\_SB.PC00.GFX0)
    {
        Method (BRT6, 2, NotSerialized)
        {
            If (_OSI ("Darwin"))
            {
                If (Arg0 == One) { Notify (\_SB.PC00.GFX0.LCD, 0x86) }
                If (Arg0 & 0x02) { Notify (\_SB.PC00.GFX0.LCD, 0x87) }
            }
            Else { XBR6 (Arg0, Arg1) }
        }
    }
}
