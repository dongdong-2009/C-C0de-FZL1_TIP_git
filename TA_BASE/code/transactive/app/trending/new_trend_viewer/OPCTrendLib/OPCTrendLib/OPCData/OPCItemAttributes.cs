namespace OPCTrendLib.OPCData
{
    using OPCTrendLib.OPCDataInterface;
    using System;
    using System.Runtime.InteropServices;
    using System.Text;

    public class OPCItemAttributes
    {
        public string AccessPath;
        public OPCACCESSRIGHTS AccessRights;
        public bool Active;
        public byte[] Blob;
        public VarEnum CanonicalDataType;
        public object EUInfo;
        public OPCEUTYPE EUType;
        public int HandleClient;
        public int HandleServer;
        public string ItemID;
        public VarEnum RequestedDataType;

        public override string ToString()
        {
            StringBuilder builder = new StringBuilder("OPCIAT: '", 0x200);
            builder.Append(this.ItemID);
            builder.Append("' ('");
            builder.Append(this.AccessPath);
            builder.AppendFormat("') hc=0x{0:x} hs=0x{1:x} act={2}", this.HandleClient, this.HandleServer, this.Active);
            builder.AppendFormat("\r\n\tacc={0} typr={1} typc={2}", this.AccessRights, this.RequestedDataType, this.CanonicalDataType);
            builder.AppendFormat("\r\n\teut={0} eui={1}", this.EUType, this.EUInfo);
            if (this.Blob != null)
            {
                builder.AppendFormat(" blob size={0}", this.Blob.Length);
            }
            return builder.ToString();
        }
    }
}
