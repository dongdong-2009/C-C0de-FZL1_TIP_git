namespace OPCTrendLib
{
    using System;

    public class ScannerException : Exception
    {
        public ScannerException(Error errorCode)
        {
        }

        public ScannerException(Error errorCode, int pos)
        {
        }
    }
}
