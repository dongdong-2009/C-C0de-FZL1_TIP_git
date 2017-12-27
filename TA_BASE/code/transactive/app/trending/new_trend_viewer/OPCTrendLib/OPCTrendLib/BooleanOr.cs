namespace OPCTrendLib
{
    using System;

    internal class BooleanOr : BooleanOp
    {
        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            return new Result(typeof(bool), ((bool) argArray[0].Value) || ((bool) argArray[1].Value));
        }

        public override string ToString()
        {
            return "||";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.LogicalOr;
            }
        }
    }
}
