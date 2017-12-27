namespace OPCTrendLib.OPCDataInterface
{
    using System;

    [Flags]
    public enum OPC_QUALITY_MASTER : short
    {
        ERROR_QUALITY_VALUE = 0x80,
        QUALITY_BAD = 0,
        QUALITY_GOOD = 0xc0,
        QUALITY_UNCERTAIN = 0x40
    }
}
