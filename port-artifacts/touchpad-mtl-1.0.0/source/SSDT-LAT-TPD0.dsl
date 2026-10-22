DefinitionBlock ("", "SSDT", 2, "LAT535", "TPD0FIX", 0x00000001)
{
    External (\_SB.PC00.I2C3.TPD0, DeviceObj)
    External (\_SB.PC00.I2C3.TPD0.SBFB, BuffObj)
    External (\_SB.PC00.I2C3.TPD0.SBFI, BuffObj)
    External (\TPDH, IntObj)
    Scope (\_SB.PC00.I2C3.TPD0)
    {
        Method (_DSM, 4, Serialized)
        {
            If (Arg0 == ToUUID ("3CDFF6F7-4267-4555-AD05-B30A3D8938DE"))
            {
                If (Arg1 == One)
                {
                    If (Arg2 == Zero) { Return (Buffer (One) { 0x03 }) }
                    If (Arg2 == One) { Return (\TPDH) }
                }
            }
            Return (Buffer (One) { 0x00 })
        }
        Method (_CRS, 0, Serialized)
        {
            Return (ConcatenateResTemplate (SBFB, SBFI))
        }
    }
}
