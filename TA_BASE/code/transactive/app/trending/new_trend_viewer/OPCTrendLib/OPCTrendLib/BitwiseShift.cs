namespace OPCTrendLib
{
    using System;

    internal class BitwiseShift : BinaryOp
    {
        private bool _isleft;

        public BitwiseShift(bool isleft)
        {
            this._isleft = isleft;
        }

        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            int num = 0;
            if (this._isleft)
            {
                num = Convert.ToInt32(argArray[0].Value) << Convert.ToInt32(argArray[1].Value);
            }
            else
            {
                num = Convert.ToInt32(argArray[0].Value) >> Convert.ToInt32(argArray[1].Value);
            }
            return new Result(typeof(int), num);
        }

        protected override bool IsEvalableType(Type type)
        {
            return BinaryHelper.IsPrimitiveType(type);
        }

        public override string ToString()
        {
            return "shift";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Shift;
            }
        }
    }
}
