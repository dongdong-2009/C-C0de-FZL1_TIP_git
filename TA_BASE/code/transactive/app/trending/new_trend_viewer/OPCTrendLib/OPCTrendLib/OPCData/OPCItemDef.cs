namespace OPCTrendLib.OPCData
{
    using System;
    using System.Runtime.InteropServices;

    public class OPCItemDef
    {
        public string AccessPath;
        public bool Active;
        public byte[] Blob;
        public int HandleClient;
        public string ItemID;
        public VarEnum RequestedDataType;

        public OPCItemDef()
        {
            this.AccessPath = "";
            this.Blob = null;
        }

        public OPCItemDef(string id, bool activ, int hclt, VarEnum vt)
        {
            this.AccessPath = "";
            this.Blob = null;
            this.ItemID = id;
            this.Active = activ;
            this.HandleClient = hclt;
            this.RequestedDataType = vt;
        }
    }
}
