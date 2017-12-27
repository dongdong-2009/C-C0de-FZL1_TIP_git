namespace OPCTrendLib
{
    using System;

    internal abstract class ArithmeticOp : BinaryOp
    {
        protected ArithmeticOp()
        {
        }

        protected override bool IsEvalableType(Type type)
        {
            return ((type == null) || BinaryHelper.IsPrimitiveType(type));
        }

        protected override void ProcessArg(Evaluator evaluater, Result[] argArray)
        {
            int num;
            base.ProcessArg(evaluater, argArray);
            Type type = argArray[0].Type;
            Type type2 = argArray[1].Type;
            if ((type == null) || (type2 == null))
            {
                throw new InvalidCastException();
            }
            if (!BinaryHelper.ComparePrimitiveType(type, type2, out num))
            {
                throw new InvalidCastException(string.Format("from {1} to {0}", type.Name, type2.Name));
            }
            if (num > 0)
            {
                argArray[1].Value = ConvertHelper.ChangeType(argArray[1].Value, type);
            }
            else if (num < 0)
            {
                argArray[0].Value = ConvertHelper.ChangeType(argArray[0].Value, type2);
            }
        }
    }
}
