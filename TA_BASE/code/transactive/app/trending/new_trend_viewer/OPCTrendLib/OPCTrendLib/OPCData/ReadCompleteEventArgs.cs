namespace OPCTrendLib.OPCData
{
    using System;

    public class ReadCompleteEventArgs : EventArgs
    {
        public int groupHandleClient;
        public int masterError;
        public int masterQuality;
        public OPCItemState[] sts;
        public int transactionID;
    }
}
