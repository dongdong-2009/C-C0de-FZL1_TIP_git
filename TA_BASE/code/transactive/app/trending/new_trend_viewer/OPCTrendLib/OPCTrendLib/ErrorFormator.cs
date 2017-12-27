namespace OPCTrendLib
{
    using OPCTrendLib.Properties;
    using System;

    internal static class ErrorFormator
    {
        internal static string FormatError(Error code, int pos)
        {
            return string.Format(Resources.ErrorFormat, code.ToString(), pos, (int) code);
        }
    }
}
