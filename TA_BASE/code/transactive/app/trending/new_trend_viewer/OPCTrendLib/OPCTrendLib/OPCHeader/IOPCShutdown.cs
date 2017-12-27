namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, ComVisible(true), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("F31DFDE1-07B6-11d2-B2D8-0060083BA1FB")]
    internal interface IOPCShutdown
    {
        void ShutdownRequest([In, MarshalAs(UnmanagedType.LPWStr)] string szReason);
    }
}
