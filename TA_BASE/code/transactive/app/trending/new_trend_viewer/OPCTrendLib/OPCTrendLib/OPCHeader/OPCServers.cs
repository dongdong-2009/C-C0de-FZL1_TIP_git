namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;
    using System.Text;

    [StructLayout(LayoutKind.Sequential)]
    public class OPCServers
    {
        public string programID;
        public string serverName;
        public Guid classID;
        public override string ToString()
        {
            StringBuilder builder = new StringBuilder("OPCServer: ", 0xfa0);
            builder.AppendFormat("'{0}' ID={1} [{2}]", this.serverName, this.programID, this.classID);
            return builder.ToString();
        }
    }
}
