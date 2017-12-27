namespace OPCTrendLib
{
    using System;

    internal class BitwiseXor : BitwiseOp
    {
        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            return new Result(typeof(int), Convert.ToInt32(argArray[0].Value) ^ Convert.ToInt32(argArray[1].Value));
        }

        public override string ToString()
        {
            return "^";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.BitwiseOr;
            }
        }
    }
}
