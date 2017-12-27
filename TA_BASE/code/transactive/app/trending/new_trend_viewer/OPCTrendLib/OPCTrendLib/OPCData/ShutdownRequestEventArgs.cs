namespace OPCTrendLib.OPCData
{
    using System;

    public class ShutdownRequestEventArgs : EventArgs
    {
        public string shutdownReason;

        public ShutdownRequestEventArgs(string shutdownReasonp)
        {
            this.shutdownReason = shutdownReasonp;
        }
    }
}
