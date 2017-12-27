namespace OPCTrendLib.OPCData
{
    using OPCTrendLib.OPCDataInterface;
    using System;
    using System.Runtime.InteropServices;

    public class OPCItemResult
    {
        public OPCACCESSRIGHTS AccessRights;
        public byte[] Blob;
        public VarEnum CanonicalDataType;
        public int Error;
        public int HandleServer;
    }
}
