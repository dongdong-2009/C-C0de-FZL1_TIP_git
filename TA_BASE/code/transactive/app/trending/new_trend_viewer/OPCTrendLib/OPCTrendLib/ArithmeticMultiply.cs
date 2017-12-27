namespace OPCTrendLib
{
    using System;

    internal class ArithmeticMultiply : ArithmeticOp
    {
        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            Type type = argArray[0].Type;
            double num = Convert.ToDouble(argArray[0].Value) * Convert.ToDouble(argArray[1].Value);
            return new Result(type, ConvertHelper.ChangeType(num, type));
        }

        public override string ToString()
        {
            return "*";
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
