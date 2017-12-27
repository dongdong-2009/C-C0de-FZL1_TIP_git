namespace OPCTrendLib.OPCDataInterface
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true), Guid("39c13a55-011e-11d0-9675-0020afd8adb3")]
    internal interface IEnumOPCItemAttributes
    {
        void Next([In] int celt, out IntPtr ppItemArray, out int pceltFetched);
        void Skip([In] int celt);
        void Reset();
        void Clone([MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
    }
}
