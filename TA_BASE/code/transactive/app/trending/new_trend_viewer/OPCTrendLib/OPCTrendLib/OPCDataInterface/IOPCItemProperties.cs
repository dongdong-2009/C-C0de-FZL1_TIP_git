namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, Guid("39c13a72-011e-11d0-9675-0020afd8adb3"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true)]
    internal interface IOPCItemProperties
    {
        void QueryAvailableProperties([In, MarshalAs(UnmanagedType.LPWStr)] string szItemID, out int dwCount, out IntPtr ppPropertyIDs, out IntPtr ppDescriptions, out IntPtr ppvtDataTypes);
        [PreserveSig]
        int GetItemProperties([In, MarshalAs(UnmanagedType.LPWStr)] string szItemID, [In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=1)] int[] pdwPropertyIDs, out IntPtr ppvData, out IntPtr ppErrors);
        [PreserveSig]
        int LookupItemIDs([In, MarshalAs(UnmanagedType.LPWStr)] string szItemID, [In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=1)] int[] pdwPropertyIDs, out IntPtr ppszNewItemIDs, out IntPtr ppErrors);
    }
}
