namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("13486D50-4821-11D2-A494-3CB306C10000"), ComVisible(true)]
    internal interface IOPCServerList
    {
        void EnumClassesOfCategories([In] int cImplemented, [In] ref Guid catidImpl, [In] int cRequired, [In] ref Guid catidReq, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
        void GetClassDetails([In] ref Guid clsid, [MarshalAs(UnmanagedType.LPWStr)] out string ppszProgID, [MarshalAs(UnmanagedType.LPWStr)] out string ppszUserType);
        void CLSIDFromProgID([In, MarshalAs(UnmanagedType.LPWStr)] string szProgId, out Guid clsid);
    }
}
