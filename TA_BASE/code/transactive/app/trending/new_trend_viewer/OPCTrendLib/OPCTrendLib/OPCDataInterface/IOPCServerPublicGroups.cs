namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("39c13a4e-011e-11d0-9675-0020afd8adb3"), ComVisible(true)]
    internal interface IOPCServerPublicGroups
    {
        void GetPublicGroupByName([In, MarshalAs(UnmanagedType.LPWStr)] string szName, [In] ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
        void RemovePublicGroup([In] int hServerGroup, [In, MarshalAs(UnmanagedType.Bool)] bool bForce);
    }
}
