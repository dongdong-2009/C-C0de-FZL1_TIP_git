namespace OPCTrendLib
{
    using OPCTrendLib.Properties;
    using System;

    public class ParserException : Exception
    {
        public ParserException(string msg) : base(msg)
        {
        }

        public ParserException(Error errorCode, int pos) : base(ErrorFormator.FormatError(errorCode, pos + 1))
        {
        }

        internal static ParserException InternalError()
        {
            return new ParserException(Resources.InternalError);
        }

        internal static ParserException NoParenBefore(string paren)
        {
            return new ParserException(string.Format(Resources.NoParentBefore, paren, 0x10));
        }

        internal static ParserException ParenNotMatch(char paren)
        {
            return new ParserException(string.Format(Resources.ParentNotMatch, paren, 14));
        }
    }
}
