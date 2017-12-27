namespace OPCTrendLib
{
    using System;
    using System.Collections;
    using System.Reflection;

    internal class IndexerOp : Operator
    {
        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            int num2;
            object obj3;
            int num3;
            if (argArray.Length < 2)
            {
                throw new ArgumentException();
            }
            object obj2 = argArray[0].Value;
            if (obj2 == null)
            {
                throw new ArgumentException();
            }
            int num = argArray.Length - 1;
            Type type = null;
            BindingFlags bindingAttr = BindingFlags.Default;
            if (obj2 is Type)
            {
                type = (Type) obj2;
                obj2 = null;
                bindingAttr = BindingFlags.Static;
            }
            else
            {
                type = obj2.GetType();
                bindingAttr = BindingFlags.Instance;
            }
            bindingAttr = BindingFlags.Public;
            Type[] types = new Type[num];
            object[] index = new object[num];
            for (num2 = 0; num2 < num; num2++)
            {
                types[num2] = argArray[num2 + 1].Type;
                index[num2] = argArray[num2 + 1].Value;
            }
            Binder defaultBinder = Type.DefaultBinder;
            PropertyInfo info = type.GetProperty("Item", bindingAttr, defaultBinder, null, types, null);
            if (info == null)
            {
                info = type.GetProperty("Items", bindingAttr, defaultBinder, null, types, null);
            }
            if (info != null)
            {
                return new Result(info.PropertyType, info.GetValue(obj2, index));
            }
            if (obj2 == null)
            {
                throw new ArgumentException();
            }
            if (obj2 is string)
            {
                string str = (string) obj2;
                num3 = (int) ConvertHelper.ChangeType(argArray[1].Value, typeof(int));
                obj3 = str[num3];
                return new Result(typeof(char), obj3);
            }
            if (obj2 is Array)
            {
                Array array = (Array) obj2;
                if (array.Rank != (argArray.Length - 1))
                {
                    throw new ArgumentException();
                }
                int[] indices = new int[num];
                for (num2 = 0; num2 < num; num2++)
                {
                    indices[num2] = (int) ConvertHelper.ChangeType(argArray[num2 + 1].Value, typeof(int));
                }
                obj3 = array.GetValue(indices);
                if (obj3 != null)
                {
                    return new Result(obj3.GetType(), obj3);
                }
                return new Result(null, null);
            }
            if (!(obj2 is IList))
            {
                throw new ArgumentException();
            }
            IList list = (IList) obj2;
            if (argArray.Length != 2)
            {
                throw new ArgumentException();
            }
            num3 = (int) ConvertHelper.ChangeType(argArray[1].Value, typeof(int));
            obj3 = list[num3];
            if (obj3 != null)
            {
                return new Result(obj3.GetType(), obj3);
            }
            return new Result(null, null);
        }

        public override string ToString()
        {
            return "indexer";
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Indexer;
            }
        }
    }
}
