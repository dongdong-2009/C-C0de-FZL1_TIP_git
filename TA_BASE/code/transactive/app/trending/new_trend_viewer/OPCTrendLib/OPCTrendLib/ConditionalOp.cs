namespace OPCTrendLib
{
    using System;

    internal class ConditionalOp : Operator
    {
        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            if ((bool) argArray[0].Value)
            {
                return argArray[1];
            }
            return argArray[2];
        }

        public override string ToString()
        {
            return "conditional";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Conditional;
            }
        }
    }
}
