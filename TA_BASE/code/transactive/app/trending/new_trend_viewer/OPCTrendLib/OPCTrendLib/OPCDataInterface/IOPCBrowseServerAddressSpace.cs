namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("39c13a4f-011e-11d0-9675-0020afd8adb3"), ComVisible(true)]
    internal interface IOPCBrowseServerAddressSpace
    {
        void QueryOrganization([MarshalAs(UnmanagedType.U4)] out OPCNAMESPACETYPE pNameSpaceType);
        void ChangeBrowsePosition([In, MarshalAs(UnmanagedType.U4)] OPCBROWSEDIRECTION dwBrowseDirection, [In, MarshalAs(UnmanagedType.LPWStr)] string szName);
        [PreserveSig]
        int BrowseOPCItemIDs([In, MarshalAs(UnmanagedType.U4)] OPCBROWSETYPE dwBrowseFilterType, [In, MarshalAs(UnmanagedType.LPWStr)] string szFilterCriteria, [In, MarshalAs(UnmanagedType.U2)] short vtDataTypeFilter, [In, MarshalAs(UnmanagedType.U4)] OPCACCESSRIGHTS dwAccessRightsFilter, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
        void GetItemID([In, MarshalAs(UnmanagedType.LPWStr)] string szItemDataID, [MarshalAs(UnmanagedType.LPWStr)] out string szItemID);
        [PreserveSig]
        int BrowseAccessPaths([In, MarshalAs(UnmanagedType.LPWStr)] string szItemID, [MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
    }
}
