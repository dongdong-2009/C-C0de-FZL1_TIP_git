namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("39c13a4d-011e-11d0-9675-0020afd8adb3"), ComVisible(true)]
    internal interface IOPCServer
    {
        void AddGroup([In, MarshalAs(UnmanagedType.LPWStr)] string szName, [In, MarshalAs(UnmanagedType.Bool)] bool bActive, [In] int dwRequestedUpdateRate, [In] int hClientGroup, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0, SizeConst=1)] int[] pTimeBias, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0, SizeConst=1)] float[] pPercentDeadband, [In] int dwLCID, out int phServerGroup, out int pRevisedUpdateRate, [In] ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
        void GetErrorString([In] int dwError, [In] int dwLocale, [MarshalAs(UnmanagedType.LPWStr)] out string ppString);
        void GetGroupByName([In, MarshalAs(UnmanagedType.LPWStr)] string szName, [In] ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
        void GetStatus([MarshalAs(UnmanagedType.LPStruct)] out SERVERSTATUS ppServerStatus);
        void RemoveGroup([In] int hServerGroup, [In, MarshalAs(UnmanagedType.Bool)] bool bForce);
        [PreserveSig]
        int CreateGroupEnumerator([In] int dwScope, [In] ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
    }
}
