namespace OPCTrendLib
{
    using System;

    internal abstract class Expression
    {
        protected Expression()
        {
        }

        internal abstract Result Eval(Evaluator evaluater, Result[] argArray);
    }
}
