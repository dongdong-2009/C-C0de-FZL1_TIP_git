namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Pack=2)]
    internal class OPCITEMRESULTintern
    {
        public int hServer = 0;
        public short vtCanonicalDataType = 0;
        public short wReserved = 0;
        [MarshalAs(UnmanagedType.U4)]
        public OPCACCESSRIGHTS dwAccessRights = 0;
        public int dwBlobSize = 0;
        public int pBlob = 0;
    }
}
