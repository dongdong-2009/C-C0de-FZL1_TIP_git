namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true), Guid("39c13a71-011e-11d0-9675-0020afd8adb3")]
    internal interface IOPCAsyncIO2
    {
        [PreserveSig]
        int Read([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In] int dwTransactionID, out int pdwCancelID, out IntPtr ppErrors);
        [PreserveSig]
        int Write([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] object[] pItemValues, [In] int dwTransactionID, out int pdwCancelID, out IntPtr ppErrors);
        void Refresh2([In, MarshalAs(UnmanagedType.U4)] OPCDATASOURCE dwSource, [In] int dwTransactionID, out int pdwCancelID);
        void Cancel2([In] int dwCancelID);
        void SetEnable([In, MarshalAs(UnmanagedType.Bool)] bool bEnable);
        void GetEnable([MarshalAs(UnmanagedType.Bool)] out bool pbEnable);
    }
}
