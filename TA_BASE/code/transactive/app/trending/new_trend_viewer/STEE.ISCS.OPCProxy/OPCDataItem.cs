using System;
using System.Collections.Generic;
using System.Text;

namespace STEE.ISCS.OPCProxy
{
    /// <summary>
    /// Represents OPC Item of the OPC Group
    /// </summary>
    public class OPCDataItem
    {
        public object DataValue;
        public int Error;
        //key to identifing opc item.
        public int HandleClient;
        public short Quality;
        public long TimeStamp;
        public string ID;
        public int HandleServer;

        public OPCDataItem(string opcID,int handleClient)
        {
            ID = opcID;
            HandleClient = handleClient;
            HandleServer = 0;
            DataValue = OPCServerProxy.OPC_BLANK_DATAVALUE;
        }	
    }
}
