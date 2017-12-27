namespace OPCTrendLib
{
    using System;

    internal enum StringToken
    {
        Alert = 7,
        Backslash = 0x5c,
        Backspace = 8,
        CarriageReturn = 10,
        DoubleQuote = 0x22,
        Erorr = 400,
        FormFeed = 12,
        HorizontalTab = 9,
        LongUnicodeSequence = 300,
        Newline = 13,
        None = 0,
        SingleQuote = 0x27,
        StringEnd = 100,
        UnicodeSequence = 200,
        VerticalQuote = 11
    }
}
