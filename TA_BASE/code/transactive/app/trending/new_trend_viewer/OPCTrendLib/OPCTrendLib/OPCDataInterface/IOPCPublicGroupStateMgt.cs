namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("39c13a51-011e-11d0-9675-0020afd8adb3"), ComVisible(true)]
    internal interface IOPCPublicGroupStateMgt
    {
        void GetState([MarshalAs(UnmanagedType.Bool)] out bool pPublic);
        void MoveToPublic();
    }
}
