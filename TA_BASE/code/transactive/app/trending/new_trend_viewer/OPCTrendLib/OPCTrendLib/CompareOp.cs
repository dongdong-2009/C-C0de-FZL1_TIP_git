namespace OPCTrendLib
{
    using System;

    internal class CompareOp : BinaryOp
    {
        private CompareResult _compareResult;

        public CompareOp(CompareResult compareResult)
        {
            this._compareResult = compareResult;
        }

        protected override Result DoEval(Evaluator evaluater, Result[] argArray)
        {
            int result = 0;
            if (argArray[0].IsNull() && argArray[1].IsNull())
            {
                return this.ProcessResult(0);
            }
            IComparable comparable = argArray[0].Value as IComparable;
            IComparable comparable2 = argArray[1].Value as IComparable;
            if (comparable != null)
            {
                result = comparable.CompareTo(argArray[1].Value);
            }
            else
            {
                if (comparable2 == null)
                {
                    throw new NotSupportedException();
                }
                result = -comparable2.CompareTo(argArray[0].Value);
            }
            return this.ProcessResult(result);
        }

        protected override bool IsEvalableType(Type type)
        {
            return true;
        }

        protected override void ProcessArg(Evaluator evaluater, Result[] argArray)
        {
            base.ProcessArg(evaluater, argArray);
            Type type = argArray[0].Type;
            Type type2 = argArray[1].Type;
            if ((type != null) || (type2 != null))
            {
                int num;
                if ((type == null) && type2.IsValueType)
                {
                    throw new InvalidCastException(string.Format("from null to {0}", type2.Name));
                }
                if ((type2 == null) && type.IsValueType)
                {
                    throw new InvalidCastException(string.Format("from null to {0}", type.Name));
                }
                if (BinaryHelper.ComparePrimitiveType(type, type2, out num))
                {
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

        private Result ProcessResult(int result)
        {
            if (result > 0)
            {
                if ((this._compareResult & CompareResult.Greater) != 0)
                {
                    return new Result(typeof(bool), true);
                }
                return new Result(typeof(bool), false);
            }
            if (result == 0)
            {
                if ((this._compareResult & CompareResult.Equal) != 0)
                {
                    return new Result(typeof(bool), true);
                }
                return new Result(typeof(bool), false);
            }
            if ((this._compareResult & CompareResult.Less) != 0)
            {
                return new Result(typeof(bool), true);
            }
            return new Result(typeof(bool), false);
        }

        public override string ToString()
        {
            return "compare";
        }

        public override OperatorPriority Priority
        {
            get
            {
                if (this._compareResult == CompareResult.Equal)
                {
                    return OperatorPriority.Equality;
                }
                return OperatorPriority.Relational;
            }
        }
    }
}
