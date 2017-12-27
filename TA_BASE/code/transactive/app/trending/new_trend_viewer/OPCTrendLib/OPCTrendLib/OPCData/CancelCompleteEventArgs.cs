namespace OPCTrendLib.OPCData
{
    using System;

    public class CancelCompleteEventArgs : EventArgs
    {
        public int groupHandleClient;
        public int transactionID;

        public CancelCompleteEventArgs(int transactionIDp, int groupHandleClientp)
        {
            this.transactionID = transactionIDp;
            this.groupHandleClient = groupHandleClientp;
        }
    }
}
