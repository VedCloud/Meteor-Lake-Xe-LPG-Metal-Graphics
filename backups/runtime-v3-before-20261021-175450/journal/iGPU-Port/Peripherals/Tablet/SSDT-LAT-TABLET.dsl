// Scoped to the measured Dell HIDD device. No global Windows/OS identity change.
DefinitionBlock ("", "SSDT", 2, "LAT535", "LATTAB02", 0x00000002)
{
    External (\_SB.HIDD, DeviceObj)
    External (\ECRB, MethodObj)
    Scope (\_SB.HIDD)
    {
        // Original _STA is retained as XSTA in the supplied DSDT.
        Method (_STA, 0, NotSerialized) { Return (0x0F) }
        Name (LTSV, 0x02)
        Method (LTSM, 0, Serialized)
        {
            // Same firmware read used by VGBS; do not alter IUCE/VBDS or
            // enable unrelated HDSM/button-array events. Preserve the raw byte.
            Local0 = \ECRB (0x39)
            If ((Local0 >= 0xFF))
            {
                Return (Package () { 0x4C545331, Zero, Zero })
            }
            Local1 = Package () { 0x4C545331, One, Zero }
            Store (Local0, Index (Local1, 0x02))
            Return (Local1)
        }
    }
}
