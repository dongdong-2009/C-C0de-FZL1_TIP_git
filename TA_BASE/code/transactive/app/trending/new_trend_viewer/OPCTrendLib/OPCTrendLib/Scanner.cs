namespace OPCTrendLib
{
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Text;

    internal sealed class Scanner
    {
        internal Context _context;
        internal int _curPos;
        private int _length;
        private string _text;

        public Scanner(string text)
        {
            this._text = text;
            this._curPos = 0;
            this._length = this._text.Length;
        }

        private char GetChar(int pos)
        {
            if (pos >= this._length)
            {
                return '\0';
            }
            return this._text[pos];
        }

        private static bool IsBlankSpace(char c)
        {
            switch (c)
            {
                case '\t':
                case '\v':
                case '\f':
                case ' ':
                case '\x00a0':
                    return true;
            }
            return ((c >= '\x0080') && (char.GetUnicodeCategory(c) == UnicodeCategory.SpaceSeparator));
        }

        internal static bool IsDigit(char c)
        {
            return (('0' <= c) && (c <= '9'));
        }

        internal static bool IsHexDigit(char c)
        {
            return ((IsDigit(c) || (('A' <= c) && (c <= 'F'))) || (('a' <= c) && (c <= 'f')));
        }

        private bool IsIdentifierPartChar(char c)
        {
            if (this.IsIdentifierStartChar(c))
            {
                return true;
            }
            if (('0' <= c) && (c <= '9'))
            {
                return true;
            }
            if (c >= '\x0080')
            {
                switch (char.GetUnicodeCategory(c))
                {
                    case UnicodeCategory.NonSpacingMark:
                    case UnicodeCategory.SpacingCombiningMark:
                    case UnicodeCategory.DecimalDigitNumber:
                    case UnicodeCategory.ConnectorPunctuation:
                        return true;
                }
            }
            return false;
        }

        private bool IsIdentifierStartChar(char c)
        {
            if (((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'))) || ('_' == c))
            {
                return true;
            }
            if (('0' > c) || (c > '9'))
            {
                if (c < '\x0080')
                {
                    return false;
                }
                switch (char.GetUnicodeCategory(c))
                {
                    case UnicodeCategory.UppercaseLetter:
                    case UnicodeCategory.LowercaseLetter:
                    case UnicodeCategory.TitlecaseLetter:
                    case UnicodeCategory.ModifierLetter:
                    case UnicodeCategory.OtherLetter:
                    case UnicodeCategory.LetterNumber:
                        return true;
                }
            }
            return false;
        }

        public Token NextToken()
        {
            this.SkipBlanks();
            this._context = new Context();
            this._context.startPos = this._curPos;
            Token @const = this.ScanToken();
            this._context.token = @const;
            this._context.endPos = this._curPos;
            switch (@const)
            {
                case Token.HexStart:
                    this._context.value = this.ScanHexNumber();
                    @const = Token.Const;
                    this._context.token = @const;
                    this._context.endPos = this._curPos;
                    return @const;

                case Token.CharMarker:
                    this._context.value = this.ScanChar();
                    @const = Token.Const;
                    this._context.token = @const;
                    this._context.endPos = this._curPos;
                    return @const;

                case Token.StringMarker:
                    this._context.value = this.ScanString();
                    @const = Token.Const;
                    this._context.token = @const;
                    this._context.endPos = this._curPos;
                    return @const;

                case Token.DateMarker:
                    this._context.value = this.ScanDate();
                    @const = Token.Const;
                    this._context.token = @const;
                    this._context.endPos = this._curPos;
                    return @const;

                case Token.None:
                {
                    char c = this.GetChar(this._curPos);
                    if (IsDigit(c))
                    {
                        @const = Token.Const;
                        this._context.token = @const;
                        this._context.value = this.ScanNumber();
                        this._context.endPos = this._curPos;
                        return @const;
                    }
                    if (!this.IsIdentifierStartChar(c))
                    {
                        if (c != '\0')
                        {
                            throw new ScannerException(Error.UnrecogniseChar);
                        }
                        @const = Token.EndOfFile;
                        this._context.token = @const;
                        return @const;
                    }
                    string name = this.ScanIdentifier();
                    @const = Keywords.GetToken(name);
                    switch (@const)
                    {
                        case Token.True:
                            this._context.value = true;
                            break;

                        case Token.False:
                            this._context.value = false;
                            break;

                        case Token.None:
                            @const = Token.Identifier;
                            this._context.value = name;
                            break;
                    }
                    break;
                }
                default:
                    return @const;
            }
            this._context.token = @const;
            this._context.endPos = this._curPos;
            return @const;
        }

        private char ScanChar()
        {
            char ch;
            StringToken token = this.ScanStringToken();
            switch (token)
            {
                case StringToken.None:
                    ch = this.GetChar(this._curPos);
                    if (ch == '\0')
                    {
                        throw new ScannerException(Error.StringNotEnd);
                    }
                    this._curPos++;
                    break;

                case StringToken.Erorr:
                    throw new ScannerException(Error.IllegalEscapeChar);

                case StringToken.UnicodeSequence:
                case StringToken.LongUnicodeSequence:
                {
                    int capacity = 4;
                    if (token == StringToken.LongUnicodeSequence)
                    {
                        capacity = 8;
                    }
                    StringBuilder builder = new StringBuilder(capacity);
                    while (capacity > 0)
                    {
                        char c = this.GetChar(this._curPos);
                        if (!IsHexDigit(c))
                        {
                            throw new ScannerException(Error.IllegalHexCharInChar);
                        }
                        builder.Append(c);
                        this._curPos++;
                        if (this._curPos >= this._length)
                        {
                            throw new ScannerException(Error.CharNotEnd);
                        }
                        capacity--;
                    }
                    ch = (char) int.Parse(builder.ToString(), NumberStyles.HexNumber);
                    break;
                }
                default:
                    ch = (char) ((ushort) token);
                    break;
            }
            if (this.GetChar(this._curPos) != '\'')
            {
                throw new ScannerException(Error.CharNotEnd);
            }
            this._curPos++;
            return ch;
        }

        private DateTime ScanDate()
        {
            StringBuilder builder = new StringBuilder();
            while (true)
            {
                char ch = this.GetChar(this._curPos);
                if (ch == '#')
                {
                    this._curPos++;
                    return DateTime.Parse(builder.ToString());
                }
                builder.Append(ch);
                this._curPos++;
                if (this._curPos >= this._length)
                {
                    throw new ScannerException(Error.StringNotEnd);
                }
            }
        }

        private object ScanHexNumber()
        {
            int num;
            long num2;
            StringBuilder builder = new StringBuilder();
            while (this._curPos < this._length)
            {
                char c = this.GetChar(this._curPos);
                if (IsHexDigit(c))
                {
                    builder.Append(c);
                }
                else
                {
                    break;
                }
                this._curPos++;
            }
            string s = builder.ToString();
            if (int.TryParse(s, NumberStyles.HexNumber, null, out num))
            {
                return num;
            }
            if (long.TryParse(s, NumberStyles.HexNumber, null, out num2))
            {
                return num2;
            }
            return decimal.Parse(s);
        }

        private string ScanIdentifier()
        {
            StringBuilder builder = new StringBuilder();
            do
            {
                char c = this.GetChar(this._curPos);
                if (!this.IsIdentifierPartChar(c))
                {
                    break;
                }
                builder.Append(c);
                this._curPos++;
            }
            while (this._curPos < this._length);
            return builder.ToString();
        }

        private object ScanNumber()
        {
            StringBuilder builder = new StringBuilder();
            NumberStyles integer = NumberStyles.Integer;
            while (this._curPos < this._length)
            {
                char c = this.GetChar(this._curPos);
                if (IsDigit(c))
                {
                    builder.Append(c);
                }
                else
                {
                    if ((c == '.') && IsDigit(this.GetChar(this._curPos + 1)))
                    {
                        integer = NumberStyles.Number;
                        builder.Append(c);
                        this._curPos++;
                        while (this._curPos < this._length)
                        {
                            c = this.GetChar(this._curPos);
                            if (IsDigit(c))
                            {
                                builder.Append(c);
                                this._curPos++;
                            }
                            else
                            {
                                if (c == 'f')
                                {
                                    integer = NumberStyles.Float;
                                    this._curPos++;
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
                this._curPos++;
            }
            string s = builder.ToString();
            if (integer == NumberStyles.Integer)
            {
                int num;
                long num2;
                if (int.TryParse(s, out num))
                {
                    return num;
                }
                if (long.TryParse(s, out num2))
                {
                    return num2;
                }
                return decimal.Parse(s);
            }
            if (integer == NumberStyles.Integer)
            {
                return double.Parse(s);
            }
            return float.Parse(s);
        }

        private string ScanString()
        {
            char ch;
            bool flag;
            StringBuilder builder = new StringBuilder();
        Label_0164:
            flag = true;
            StringToken token = this.ScanStringToken();
            switch (token)
            {
                case StringToken.None:
                    ch = this.GetChar(this._curPos);
                    if (ch == '\0')
                    {
                        throw new ScannerException(Error.StringNotEnd);
                    }
                    builder.Append(ch);
                    this._curPos++;
                    goto Label_0164;

                case StringToken.StringEnd:
                    return builder.ToString();

                case StringToken.Erorr:
                    throw new ScannerException(Error.IllegalEscapeChar);

                case StringToken.UnicodeSequence:
                case StringToken.LongUnicodeSequence:
                {
                    int capacity = 4;
                    if (token == StringToken.LongUnicodeSequence)
                    {
                        capacity = 8;
                    }
                    StringBuilder builder2 = new StringBuilder(capacity);
                    while (capacity > 0)
                    {
                        ch = this.GetChar(this._curPos);
                        if (!IsHexDigit(ch))
                        {
                            throw new ScannerException(Error.IllegalHexCharInString);
                        }
                        builder2.Append(ch);
                        this._curPos++;
                        if (this._curPos >= this._length)
                        {
                            throw new ScannerException(Error.StringNotEnd);
                        }
                        capacity--;
                    }
                    int num2 = int.Parse(builder2.ToString(), NumberStyles.HexNumber);
                    builder.Append((char) num2);
                    goto Label_0164;
                }
            }
            builder.Append((char) ((ushort) token));
            goto Label_0164;
        }

        private StringToken ScanStringToken()
        {
            StringToken none = StringToken.None;
            int pos = this._curPos;
            switch (this.GetChar(pos))
            {
                case '"':
                    none = StringToken.StringEnd;
                    pos++;
                    goto Label_014A;

                case '\\':
                    switch (this.GetChar(pos + 1))
                    {
                        case '"':
                            none = StringToken.DoubleQuote;
                            pos += 2;
                            goto Label_014A;

                        case '\'':
                            none = StringToken.SingleQuote;
                            pos += 2;
                            goto Label_014A;

                        case 'U':
                            none = StringToken.LongUnicodeSequence;
                            pos += 2;
                            goto Label_014A;

                        case 'n':
                            none = StringToken.Newline;
                            pos += 2;
                            goto Label_014A;

                        case 'r':
                            none = StringToken.CarriageReturn;
                            pos += 2;
                            goto Label_014A;

                        case 't':
                            none = StringToken.HorizontalTab;
                            pos += 2;
                            goto Label_014A;

                        case 'u':
                            none = StringToken.UnicodeSequence;
                            pos += 2;
                            goto Label_014A;

                        case 'v':
                            none = StringToken.VerticalQuote;
                            pos += 2;
                            goto Label_014A;

                        case 'x':
                            none = StringToken.UnicodeSequence;
                            pos += 2;
                            goto Label_014A;

                        case 'f':
                            none = StringToken.FormFeed;
                            pos += 2;
                            goto Label_014A;

                        case 'a':
                            none = StringToken.Alert;
                            pos += 2;
                            goto Label_014A;

                        case 'b':
                            none = StringToken.Backspace;
                            pos += 2;
                            goto Label_014A;

                        case '\\':
                            none = StringToken.Backslash;
                            pos += 2;
                            goto Label_014A;
                    }
                    break;

                default:
                    goto Label_014A;
            }
            none = StringToken.Erorr;
            pos++;
        Label_014A:
            if (none != StringToken.None)
            {
                this._curPos = pos;
            }
            return none;
        }

        private Token ScanToken()
        {
            char ch;
            Token none = Token.None;
            int pos = this._curPos;
            switch (this.GetChar(pos))
            {
                case '!':
                    if (this.GetChar(pos + 1) != '=')
                    {
                        none = Token.BooleanNot;
                        pos++;
                        break;
                    }
                    none = Token.Inequality;
                    pos += 2;
                    break;

                case '"':
                    none = Token.StringMarker;
                    pos++;
                    break;

                case '#':
                    none = Token.DateMarker;
                    pos++;
                    break;

                case '%':
                    none = Token.Modulus;
                    pos++;
                    break;

                case '&':
                    if (this.GetChar(pos + 1) != '&')
                    {
                        none = Token.BitwiseAnd;
                        pos++;
                        break;
                    }
                    none = Token.BooleanAnd;
                    pos += 2;
                    break;

                case '\'':
                    none = Token.CharMarker;
                    pos++;
                    break;

                case '(':
                    none = Token.LeftParen;
                    pos++;
                    break;

                case ')':
                    none = Token.RightParen;
                    pos++;
                    break;

                case '*':
                    none = Token.Multiply;
                    pos++;
                    break;

                case '+':
                    none = Token.Add;
                    pos++;
                    break;

                case ',':
                    none = Token.Comma;
                    pos++;
                    break;

                case '-':
                    none = Token.Subtract;
                    pos++;
                    break;

                case '.':
                    none = Token.Member;
                    pos++;
                    break;

                case '/':
                    none = Token.Divide;
                    pos++;
                    break;

                case '0':
                    ch = this.GetChar(pos + 1);
                    if (ch != 'x')
                    {
                        if (ch == 'X')
                        {
                            none = Token.HexStart;
                            pos += 2;
                        }
                        break;
                    }
                    none = Token.HexStart;
                    pos += 2;
                    break;

                case ':':
                    none = Token.ConditionalSemicolon;
                    pos++;
                    break;

                case '<':
                    ch = this.GetChar(pos + 1);
                    if (ch != '<')
                    {
                        if (ch == '=')
                        {
                            none = Token.LessThanOrEqual;
                            pos += 2;
                        }
                        else
                        {
                            none = Token.LessThan;
                            pos++;
                        }
                        break;
                    }
                    none = Token.BitwiseShiftLeft;
                    pos += 2;
                    break;

                case '=':
                    if (this.GetChar(pos + 1) == '=')
                    {
                        none = Token.Equality;
                        pos += 2;
                    }
                    break;

                case '>':
                    ch = this.GetChar(pos + 1);
                    if (ch != '=')
                    {
                        if (ch == '>')
                        {
                            none = Token.BitwiseShiftRight;
                            pos += 2;
                        }
                        else
                        {
                            none = Token.GreaterThan;
                            pos++;
                        }
                        break;
                    }
                    none = Token.GreaterThanOrEqual;
                    pos += 2;
                    break;

                case '?':
                    none = Token.ConditionalIf;
                    pos++;
                    break;

                case '[':
                    none = Token.LeftIndexer;
                    pos++;
                    break;

                case ']':
                    none = Token.RightIndexer;
                    pos++;
                    break;

                case '^':
                    none = Token.BitwiseXor;
                    pos++;
                    break;

                case '|':
                    if (this.GetChar(pos + 1) != '|')
                    {
                        none = Token.BitwiseOr;
                        pos++;
                        break;
                    }
                    none = Token.BooleanOr;
                    pos += 2;
                    break;

                case '~':
                    none = Token.BitwiseNot;
                    pos++;
                    break;
            }
            if (none != Token.None)
            {
                this._curPos = pos;
            }
            return none;
        }

        private void SkipBlanks()
        {
            for (char ch = this.GetChar(this._curPos); IsBlankSpace(ch); ch = this.GetChar(++this._curPos))
            {
            }
        }

        private class Keywords
        {
            private static Hashtable _dict = new Hashtable(11);

            static Keywords()
            {
                Add("true", Token.True);
                Add("false", Token.False);
                Add("null", Token.Null);
            }

            public static void Add(string name, Token tokenType)
            {
                _dict[name] = tokenType;
            }

            public static Token GetToken(string name)
            {
                if (_dict.ContainsKey(name))
                {
                    return (Token) _dict[name];
                }
                return Token.None;
            }
        }
    }
}
