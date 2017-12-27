namespace OPCTrendLib
{
    using System;

    internal class AdditiveOp : BinaryOp
    {
        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            Type type = argArray[0].Type;
            Type type2 = argArray[1].Type;
            if ((type == null) && (type2 == null))
            {
                return new Result(null, null);
            }
            if (type == typeof(string))
            {
                return new Result(ConvertHelper.ToString(argArray[0].Value) + ConvertHelper.ToString(argArray[1].Value));
            }
            double num = Convert.ToDouble(argArray[0].Value) + Convert.ToDouble(argArray[1].Value);
            return new Result(type, ConvertHelper.ChangeType(num, type));
        }

        protected override bool IsEvalableType(Type type)
        {
            return ((type == null) || (BinaryHelper.IsPrimitiveType(type) || (type == typeof(string))));
        }

        protected override void ProcessArg(Evaluator evaluater, Result[] argArray)
        {
            base.ProcessArg(evaluater, argArray);
            Type type = argArray[0].Type;
            Type type2 = argArray[1].Type;
            if ((type != null) || (type2 != null))
            {
                if (type == null)
                {
                    if (type2.IsValueType)
                    {
                        throw new InvalidCastException(string.Format("from null to {0}", type2.Name));
                    }
                    if (type2 != typeof(string))
                    {
                        throw new InvalidCastException(string.Format("from null to {0}", type2.Name));
                    }
                    argArray[0].Type = typeof(string);
                }
                else if (type2 == null)
                {
                    if (type.IsValueType)
                    {
                        throw new InvalidCastException(string.Format("from null to {0}", type.Name));
                    }
                    if (type != typeof(string))
                    {
                        throw new InvalidCastException(string.Format("from null to {0}", type.Name));
                    }
                    argArray[1].Type = typeof(string);
                }
                else
                {
                    int num;
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

        public override string ToString()
        {
            return "+";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Additive;
            }
        }
    }
}
