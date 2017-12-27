namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true), Guid("F31DFDE2-07B6-11d2-B2D8-0060083BA1FB")]
    internal interface IOPCCommon
    {
        void SetLocaleID([In] int dwLcid);
        void GetLocaleID(out int pdwLcid);
        [PreserveSig]
        int QueryAvailableLocaleIDs(out int pdwCount, out IntPtr pdwLcid);
        void GetErrorString([In] int dwError, [MarshalAs(UnmanagedType.LPWStr)] out string ppString);
        void SetClientName([In, MarshalAs(UnmanagedType.LPWStr)] string szName);
    }
}
