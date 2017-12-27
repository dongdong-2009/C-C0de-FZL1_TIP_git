namespace OPCTrendLib
{
    using System;

    public class Parser
    {
        private Context _current;
        private Scanner _scanner;
        private Tree _tree;

        private ParserException BuildException(Error errorCode)
        {
            return new ParserException(errorCode, this._current.startPos);
        }

        private ParserException BuildException(Error errorCode, Context token)
        {
            return new ParserException(errorCode, token.startPos);
        }

        internal void CheckStartableToken(Token token)
        {
            if (!IsStartableToken(token))
            {
                throw this.BuildException(Error.IdentfierConstUnaryOrLeftParenExpected);
            }
        }

        internal static bool IsBinaryOp(Token token)
        {
            return ((token > Token.BinaryFirst) && (token < Token.BinaryLast));
        }

        internal static bool IsCanFollowConst(Token token)
        {
            if (IsBinaryOp(token))
            {
                return true;
            }
            switch (token)
            {
                case Token.LeftIndexer:
                case Token.RightIndexer:
                case Token.ConditionalIf:
                case Token.ConditionalSemicolon:
                case Token.Member:
                case Token.RightParen:
                case Token.Comma:
                case Token.EndOfFile:
                    return true;
            }
            return false;
        }

        internal static bool IsCanFollowIdentifier(Token token)
        {
            if (IsBinaryOp(token))
            {
                return true;
            }
            switch (token)
            {
                case Token.LeftIndexer:
                case Token.RightIndexer:
                case Token.ConditionalIf:
                case Token.ConditionalSemicolon:
                case Token.Member:
                case Token.LeftParen:
                case Token.RightParen:
                case Token.Comma:
                case Token.EndOfFile:
                    return true;
            }
            return false;
        }

        internal static bool IsConstTocken(Token token)
        {
            return ((token > Token.ConstFirst) && (token < Token.ConstLast));
        }

        internal static bool IsStartableToken(Token token)
        {
            if (token >= Token.Identifier)
            {
                return true;
            }
            switch (token)
            {
                case Token.BooleanNot:
                case Token.BitwiseNot:
                case Token.LeftParen:
                case Token.Subtract:
                    return true;
            }
            return false;
        }

        internal static bool IsUnaryOp(Token token)
        {
            return ((token == Token.BitwiseNot) || (token == Token.BooleanNot));
        }

        internal Token NextToken()
        {
            Token token = this._scanner.NextToken();
            this._current = this._scanner._context;
            return token;
        }

        public Tree Parse(string text)
        {
            this._scanner = new Scanner(text);
            this._tree = new Tree();
            Token token = this.NextToken();
            if (token != Token.EndOfFile)
            {
                this.CheckStartableToken(token);
                while (token != Token.EndOfFile)
                {
                    MemberOp expression;
                    if (IsConstTocken(token))
                    {
                        this.ParseConst();
                        goto Label_022A;
                    }
                    if (this._tree._isStart && ((token == Token.Subtract) || (token == Token.Add)))
                    {
                        Constant expr = new Constant(0);
                        this._tree.AddOperand(expr);
                        BinaryOp op = BinaryOp.CreateOp(token);
                        this._tree.AddOperator(op);
                        goto Label_022A;
                    }
                    if (IsBinaryOp(token))
                    {
                        this.ParseBinaryOp();
                        goto Label_022A;
                    }
                    if (IsUnaryOp(token))
                    {
                        this.ParseUnaryOp();
                        goto Label_022A;
                    }
                    switch (token)
                    {
                        case Token.LeftIndexer:
                            this.ParseIndexer();
                            goto Label_022A;

                        case Token.RightIndexer:
                            this._tree.Pop('[');
                            goto Label_022A;

                        case Token.ConditionalIf:
                            this.ParseConditional();
                            goto Label_022A;

                        case Token.ConditionalSemicolon:
                            this.ParseConditionalSemicolon();
                            goto Label_022A;

                        case Token.Member:
                            this.ParseMember();
                            goto Label_022A;

                        case Token.LeftParen:
                            if (this._tree._top.Expression is MemberOp)
                            {
                                expression = (MemberOp) this._tree._top.Expression;
                                if (expression.IsFunction)
                                {
                                    throw new ParserException("()()not allowed");
                                }
                                break;
                            }
                            goto Label_01E3;

                        case Token.RightParen:
                            this._tree.Pop('(');
                            goto Label_022A;

                        case Token.Comma:
                            this._tree.Pop(',');
                            this._tree.Push(',');
                            goto Label_022A;

                        case Token.Identifier:
                            this.ParseIdentifier();
                            goto Label_022A;

                        default:
                            throw this.BuildException(Error.InternalError);
                    }
                    expression.IsFunction = true;
                Label_01E3:
                    this._tree.Push('(');
                Label_022A:
                    token = this.NextToken();
                }
                this._tree.Complete();
            }
            return this._tree;
        }

        private void ParseBinaryOp()
        {
            Token token = this.PeekToken();
            this.CheckStartableToken(token);
            BinaryOp expr = BinaryOp.CreateOp(this._current.token);
            this._tree.AddOperator(expr);
        }

        private void ParseConditional()
        {
            Token token = this.PeekToken();
            this.CheckStartableToken(token);
            ConditionalOp expr = new ConditionalOp();
            this._tree.AddOperator(expr);
            this._tree.Push('?');
        }

        private void ParseConditionalSemicolon()
        {
            Token token = this.PeekToken();
            this.CheckStartableToken(token);
            this._tree.Pop('?');
            this._tree.Push(':');
            this._tree.Pop(':');
        }

        private void ParseConst()
        {
            if (!IsCanFollowConst(this.PeekToken()))
            {
                throw this.BuildException(Error.BinaryCommaRightParenRightIndexerExpected);
            }
            Constant expr = new Constant(this._current.value);
            this._tree.AddOperand(expr);
        }

        private void ParseIdentifier()
        {
            if (!IsCanFollowIdentifier(this.PeekToken()))
            {
                throw this.BuildException(Error.BinaryCommaMemberParenIndexerExpected);
            }
            if (this._tree._top.Expression is MemberOp)
            {
                MemberId expr = new MemberId(this._current.StringValue);
                this._tree.AddOperand(expr);
            }
            else
            {
                Variable variable = new Variable(this._current.StringValue);
                this._tree.AddOperand(variable);
            }
        }

        private void ParseIndexer()
        {
            Token token = this.PeekToken();
            this.CheckStartableToken(token);
            IndexerOp expr = new IndexerOp();
            this._tree.AddOperator(expr);
            this._tree.Push('[');
        }

        private void ParseMember()
        {
            Token token = this.PeekToken();
            if ((token != Token.Identifier) && (token != Token.LeftParen))
            {
                throw this.BuildException(Error.IndentiferExpected);
            }
            MemberOp expr = new MemberOp();
            this._tree.AddOperator(expr);
            if (token == Token.LeftParen)
            {
                this.NextToken();
                this._tree.Push('(');
            }
        }

        private void ParseUnaryOp()
        {
            Token token = this.PeekToken();
            this.CheckStartableToken(token);
            UnaryOp expr = UnaryOp.CreateOp(this._current.token);
            this._tree.AddOperator(expr);
        }

        internal Token PeekToken()
        {
            int num = this._scanner._curPos;
            Context context = this._scanner._context;
            Token none = Token.None;
            try
            {
                none = this._scanner.NextToken();
            }
            finally
            {
                this._scanner._curPos = num;
                this._scanner._context = context;
            }
            return none;
        }
    }
}
