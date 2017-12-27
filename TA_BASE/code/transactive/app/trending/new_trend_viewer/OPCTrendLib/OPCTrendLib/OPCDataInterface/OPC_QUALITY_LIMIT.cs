namespace OPCTrendLib.OPCDataInterface
{
    using System;

    [Flags]
    public enum OPC_QUALITY_LIMIT
    {
        LIMIT_OK,
        LIMIT_LOW,
        LIMIT_HIGH,
        LIMIT_CONST
    }
}
