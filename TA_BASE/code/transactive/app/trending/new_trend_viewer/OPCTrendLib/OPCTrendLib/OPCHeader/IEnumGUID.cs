namespace OPCTrendLib.OPCHeader
{
    using System;
    using System.Runtime.InteropServices;

    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown), ComVisible(true), Guid("0002E000-0000-0000-C000-000000000046")]
    internal interface IEnumGUID
    {
        void Next([In] int celt, [In] IntPtr rgelt, out int pceltFetched);
        void Skip([In] int celt);
        void Reset();
        void Clone([MarshalAs(UnmanagedType.IUnknown)] out object ppUnk);
    }
}
