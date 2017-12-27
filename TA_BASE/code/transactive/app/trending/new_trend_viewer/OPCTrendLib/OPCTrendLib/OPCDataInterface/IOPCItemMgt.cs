namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, ComVisible(true), Guid("39c13a54-011e-11d0-9675-0020afd8adb3"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IOPCItemMgt
    {
        [PreserveSig]
        int AddItems([In] int dwCount, [In] IntPtr pItemArray, out IntPtr ppAddResults, out IntPtr ppErrors);
        [PreserveSig]
        int ValidateItems([In] int dwCount, [In] IntPtr pItemArray, [In, MarshalAs(UnmanagedType.Bool)] bool bBlobUpdate, out IntPtr ppValidationResults, out IntPtr ppErrors);
        [PreserveSig]
        int RemoveItems([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, out IntPtr ppErrors);
        [PreserveSig]
        int SetActiveState([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In, MarshalAs(UnmanagedType.Bool)] bool bActive, out IntPtr ppErrors);
        [PreserveSig]
        int SetClientHandles([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phClient, out IntPtr ppErrors);
        [PreserveSig]
        int SetDatatypes([In] int dwCount, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] phServer, [In] IntPtr pRequestedDatatypes, out IntPtr ppErrors);
        [PreserveSig]
        int CreateEnumerator([In] ref Guid riid, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
    }
}
