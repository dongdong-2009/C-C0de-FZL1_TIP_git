namespace OPCTrendLib
{
    using System;

    internal abstract class BinaryOp : Operator
    {
        public static BinaryOp CreateOp(Token type)
        {
            switch (type)
            {
                case Token.BooleanOr:
                    return new BooleanOr();

                case Token.BooleanAnd:
                    return new BooleanAnd();

                case Token.Inequality:
                    return new CompareOp(CompareResult.Less | CompareResult.Greater);

                case Token.Equality:
                    return new CompareOp(CompareResult.Equal);

                case Token.LessThan:
                    return new CompareOp(CompareResult.Less);

                case Token.LessThanOrEqual:
                    return new CompareOp(CompareResult.Less | CompareResult.Equal);

                case Token.GreaterThan:
                    return new CompareOp(CompareResult.Greater);

                case Token.GreaterThanOrEqual:
                    return new CompareOp(CompareResult.Equal | CompareResult.Greater);

                case Token.BitwiseOr:
                    return new BitwiseOr();

                case Token.BitwiseAnd:
                    return new BitwiseAnd();

                case Token.BitwiseXor:
                    return new BitwiseXor();

                case Token.BitwiseShiftLeft:
                    return new BitwiseShift(true);

                case Token.BitwiseShiftRight:
                    return new BitwiseShift(false);

                case Token.Add:
                    return new AdditiveOp();

                case Token.Subtract:
                    return new ArithmeticSubtract();

                case Token.Multiply:
                    return new ArithmeticMultiply();

                case Token.Divide:
                    return new ArithmeticDivide();

                case Token.Modulus:
                    return new ArithmeticModulus();
            }
            throw new InvalidOperationException("Not a BinayOP");
        }

        protected abstract Result DoEval(Evaluator evaluater, Result[] argArray);
        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            this.ProcessArg(evaluater, argArray);
            return this.DoEval(evaluater, argArray);
        }

        protected abstract bool IsEvalableType(Type type);
        protected virtual void ProcessArg(Evaluator evaluater, Result[] argArray)
        {
            if (argArray.Length != 2)
            {
                throw new ArgumentException();
            }
            foreach (Result result in argArray)
            {
                if (!this.IsEvalableType(result.Type))
                {
                    throw new NotSupportedException();
                }
            }
        }
    }
}
