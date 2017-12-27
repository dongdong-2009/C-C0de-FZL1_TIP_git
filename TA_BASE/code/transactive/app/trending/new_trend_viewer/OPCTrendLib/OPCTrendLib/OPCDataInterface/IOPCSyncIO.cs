namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true), Guid("39c13a52-011e-11d0-9675-0020afd8adb3")]
    internal interface IOPCSyncIO
    {
        [PreserveSig]
        int Read([In, MarshalAs(UnmanagedType.U4)] OPCDATASOURCE dwSource, [In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=1)] int[] phServer, out IntPtr ppItemValues, out IntPtr ppErrors);
        [PreserveSig]
        int Write([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] object[] pItemValues, out IntPtr ppErrors);
    }
}
