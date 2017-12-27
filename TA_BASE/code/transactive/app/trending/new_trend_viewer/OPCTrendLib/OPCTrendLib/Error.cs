namespace OPCTrendLib
{
    using System;

    public enum Error
    {
        NoError,
        InternalError,
        UnrecogniseChar,
        NotHexChar,
        ErrorFloatFormat,
        ErrorIntFormat,
        ErrorDoubleFormat,
        ErrorDateTimeFormat,
        PoundExpected,
        EmptyCharacterLiteral,
        StringNotEnd,
        IdentfierConstUnaryOrLeftParenExpected,
        BinaryCommaMemberParenIndexerExpected,
        BinaryCommaRightParenRightIndexerExpected,
        ParenNotMatch,
        IndentiferExpected,
        NoParenBefore,
        NotSupportGlobalFunction,
        VariableNotExist,
        IllegalChar,
        IllegalEscapeChar,
        IllegalHexCharInString,
        CharNotEnd,
        IllegalHexCharInChar,
        IllegalTockenSeq,
        ParenthesisRightExpected,
        IndexingEndExpected,
        BinaryOpNotEnd,
        MemberNameExpected
    }
}
