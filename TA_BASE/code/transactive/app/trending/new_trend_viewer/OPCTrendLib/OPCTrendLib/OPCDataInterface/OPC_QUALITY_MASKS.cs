namespace OPCTrendLib.OPCDataInterface
{
    using System;

    [Flags]
    public enum OPC_QUALITY_MASKS : short
    {
        LIMIT_MASK = 3,
        MASTER_MASK = 0xc0,
        STATUS_MASK = 0xfc
    }
}
