namespace OPCTrendLib.OPCDataInterface
{
    using System;

    [Flags]
    public enum OPC_QUALITY_STATUS : short
    {
        BAD = 0,
        COMM_FAILURE = 0x18,
        CONFIG_ERROR = 4,
        DEVICE_FAILURE = 12,
        EGU_EXCEEDED = 0x54,
        LAST_KNOWN = 20,
        LAST_USABLE = 0x44,
        LOCAL_OVERRIDE = 0xd8,
        NOT_CONNECTED = 8,
        OK = 0xc0,
        OUT_OF_SERVICE = 0x1c,
        SENSOR_CAL = 80,
        SENSOR_FAILURE = 0x10,
        SUB_NORMAL = 0x58,
        UNCERTAIN = 0x40
    }
}
