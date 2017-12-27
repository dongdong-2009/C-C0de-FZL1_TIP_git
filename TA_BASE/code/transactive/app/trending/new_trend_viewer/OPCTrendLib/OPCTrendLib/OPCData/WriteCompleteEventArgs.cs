namespace OPCTrendLib.OPCData
{
    using System;

    public class WriteCompleteEventArgs : EventArgs
    {
        public int groupHandleClient;
        public int masterError;
        public OPCWriteResult[] res;
        public int transactionID;
    }
}
