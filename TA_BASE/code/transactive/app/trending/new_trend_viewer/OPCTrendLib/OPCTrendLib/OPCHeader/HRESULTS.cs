namespace OPCTrendLib.OPCHeader
{
    using System;

    public class HRESULTS
    {
        public const int CONNECT_E_ADVISELIMIT = -2147220991;
        public const int CONNECT_E_NOCONNECTION = -2147220992;
        public const int E_ABORT = -2147467260;
        public const int E_FAIL = -2147467259;
        public const int E_INVALIDARG = -2147024809;
        public const int E_NOINTERFACE = -2147467262;
        public const int E_NOTIMPL = -2147467263;
        public const int E_OUTOFMEMORY = -2147024882;
        public const int OPC_E_BADRIGHTS = -1073479674;
        public const int OPC_E_BADTYPE = -1073479676;
        public const int OPC_E_DUPLICATENAME = -1073479668;
        public const int OPC_E_INVALID_PID = -1073479165;
        public const int OPC_E_INVALIDCONFIGFILE = -1073479664;
        public const int OPC_E_INVALIDFILTER = -1073479671;
        public const int OPC_E_INVALIDHANDLE = -1073479679;
        public const int OPC_E_INVALIDITEMID = -1073479672;
        public const int OPC_E_NOTFOUND = -1073479663;
        public const int OPC_E_PUBLIC = -1073479675;
        public const int OPC_E_RANGE = -1073479669;
        public const int OPC_E_UNKNOWNITEMID = -1073479673;
        public const int OPC_E_UNKNOWNPATH = -1073479670;
        public const int OPC_S_CLAMP = 0x4000e;
        public const int OPC_S_INUSE = 0x4000f;
        public const int OPC_S_UNSUPPORTEDRATE = 0x4000d;
        public const int S_FALSE = 1;
        public const int S_OK = 0;

        public static bool Failed(int hresultcode)
        {
            return (hresultcode < 0);
        }

        public static bool Succeeded(int hresultcode)
        {
            return (hresultcode >= 0);
        }
    }
}
