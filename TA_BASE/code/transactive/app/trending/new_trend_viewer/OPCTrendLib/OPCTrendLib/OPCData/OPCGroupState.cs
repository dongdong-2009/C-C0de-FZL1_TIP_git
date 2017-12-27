namespace OPCTrendLib.OPCData
{
    using System;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential)]
    public struct OPCGroupState
    {
        public string Name;
        public bool Public;
        public int UpdateRate;
        public bool Active;
        public int TimeBias;
        public float PercentDeadband;
        public int LocaleID;
        public int HandleClient;
        public int HandleServer;
    }
}
