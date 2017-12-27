namespace OPCTrendLib
{
    using System;

    internal abstract class BooleanOp : BinaryOp
    {
        protected BooleanOp()
        {
        }

        protected override bool IsEvalableType(Type type)
        {
            return (type == typeof(bool));
        }

        protected override void ProcessArg(Evaluator evaluater, Result[] argArray)
        {
            base.ProcessArg(evaluater, argArray);
        }
    }
}
