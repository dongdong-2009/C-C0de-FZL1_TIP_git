namespace OPCTrendLib
{
    using System;

    internal abstract class UnaryOp : Operator
    {
        public static UnaryOp CreateOp(Token type)
        {
            switch (type)
            {
                case Token.BooleanNot:
                    return new BooleanNotOp();

                case Token.BitwiseNot:
                    return new BitwiseNotOp();
            }
            return null;
        }

        public override string ToString()
        {
            return "UnaryOp";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Unary;
            }
        }

        private class BitwiseNotOp : UnaryOp
        {
            internal override Result Eval(Evaluator evaluater, Result[] argArray)
            {
                return new Result(typeof(int), ~((uint) ConvertHelper.ChangeType(argArray[0].Value, typeof(uint))));
            }
        }

        private class BooleanNotOp : UnaryOp
        {
            internal override Result Eval(Evaluator evaluater, Result[] argArray)
            {
                return new Result(typeof(bool), !((bool) argArray[0].Value));
            }
        }
    }
}
