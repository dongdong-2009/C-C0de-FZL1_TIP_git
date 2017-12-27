namespace OPCTrendLib
{
    using System;

    internal class ArithmeticModulus : ArithmeticOp
    {
        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            Type type = typeof(int);
            int num = Convert.ToInt32(argArray[0].Value) % Convert.ToInt32(argArray[1].Value);
            return new Result(type, ConvertHelper.ChangeType(num, type));
        }

        public override string ToString()
        {
            return "%";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Multiplicative;
            }
        }
    }
}
