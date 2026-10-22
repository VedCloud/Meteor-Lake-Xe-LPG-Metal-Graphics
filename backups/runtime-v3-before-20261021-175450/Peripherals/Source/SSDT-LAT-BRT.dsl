DefinitionBlock ("", "SSDT", 2, "LAT535", "BRTKEYS", 0x00000005)
{
    External (\_SB.AMW0, DeviceObj)
    External (\_SB.PC00.LPCB.PS2K, DeviceObj)
    External (\_SB.PC00.GFX0, DeviceObj)
    External (\_SB.PC00.GFX0.XBR6, MethodObj)
    External (\_SB.PC00.GFX0.LCD, DeviceObj)
    External (\XSME, MethodObj)
    External (\GENS, MethodObj)
    External (\EV6, MethodObj)
    External (\EV19, MethodObj)
    External (\_SB.PC00.LPCB.ECDV, DeviceObj)
    External (\_SB.PC00.LPCB.ECDV.XQ66, MethodObj)
    Name (LWMI, Zero)
    Scope (\_SB.AMW0)
    {
        Method (LBRG, 1, Serialized)
        {
            If (_OSI ("Darwin")) { \LWMI = Arg0 }
            Return (\LWMI)
        }
    }
    Name (LQCT, Zero)
    Name (LSCT, Zero)
    Name (LSFL, Zero)
    Name (LBCT, Zero)
    Name (LBAR, Zero)
    Scope (\_SB.PC00.LPCB.ECDV)
    {
        Method (_Q66, 0, NotSerialized)
        {
            If (_OSI ("Darwin")) { Increment (\LQCT) }
            XQ66 ()
        }
    }
    Scope (\_SB.PC00.GFX0.LCD)
    {
        Method (LDBG, 0, NotSerialized)
        {
            Return (Package () { \LQCT, \LSCT, \LSFL, \LBCT, \LBAR })
        }
    }
    Method (SMEE, 1, NotSerialized)
    {
        If (_OSI ("Darwin"))
        {
            Increment (\LSCT)
            Local0 = GENS (0x11, Zero, Zero)
            \LSFL = Local0
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
                Increment (\LBCT)
                \LBAR = Arg0
                If (\LWMI == Zero)
                {
                If (Arg0 == One) { Notify (\_SB.PC00.LPCB.PS2K, 0x0366) }
                If (Arg0 & 0x02) { Notify (\_SB.PC00.LPCB.PS2K, 0x0365) }
                }
            }
            Else { XBR6 (Arg0, Arg1) }
        }
    }
}
