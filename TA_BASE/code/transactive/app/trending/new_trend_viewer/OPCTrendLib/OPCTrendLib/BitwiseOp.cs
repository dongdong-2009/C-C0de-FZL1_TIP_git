namespace OPCTrendLib
{
    using System;

    internal abstract class BitwiseOp : BinaryOp
    {
        protected BitwiseOp()
        {
        }

        protected override bool IsEvalableType(Type type)
        {
            return BinaryHelper.IsPrimitiveType(type);
        }
    }
}
