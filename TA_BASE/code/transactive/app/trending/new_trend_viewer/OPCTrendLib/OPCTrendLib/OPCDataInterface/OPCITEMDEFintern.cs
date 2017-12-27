namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode, Pack=2)]
    internal class OPCITEMDEFintern
    {
        [MarshalAs(UnmanagedType.LPWStr)]
        public string szAccessPath;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string szItemID;
        [MarshalAs(UnmanagedType.Bool)]
        public bool bActive;
        public int hClient;
        public int dwBlobSize;
        public IntPtr pBlob;
        public short vtRequestedDataType;
        public short wReserved;
    }
}
