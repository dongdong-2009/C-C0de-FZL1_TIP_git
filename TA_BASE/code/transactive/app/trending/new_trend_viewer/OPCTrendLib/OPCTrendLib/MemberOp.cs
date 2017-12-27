namespace OPCTrendLib
{
    using System;
    using System.Reflection;

    internal class MemberOp : Operator
    {
        private bool _isFunction = false;

        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            if (this._isFunction)
            {
                return this.EvalFunc(evaluater, argArray);
            }
            return this.EvalProp(evaluater, argArray);
        }

        private Result EvalFunc(Evaluator evaluater, Result[] argArray)
        {
            if (argArray.Length < 2)
            {
                throw new ArgumentException();
            }
            if ((argArray[0] == null) || (argArray[1] == null))
            {
                throw new ArgumentNullException("argArray[0]");
            }
            object target = argArray[0].Value;
            if (target == null)
            {
                throw new ArgumentException();
            }
            string name = ConvertHelper.ToString(argArray[1].Value);
            Type type = null;
            BindingFlags invokeAttr = BindingFlags.Default;
            if (target is Type)
            {
                type = (Type) target;
                target = null;
                invokeAttr = BindingFlags.Static;
            }
            else
            {
                type = target.GetType();
                invokeAttr = BindingFlags.Instance;
            }
            invokeAttr |= BindingFlags.OptionalParamBinding | BindingFlags.InvokeMethod | BindingFlags.Public;
            int num = argArray.Length - 2;
            object[] args = new object[num];
            for (int i = 0; i < num; i++)
            {
                args[i] = argArray[i + 2].Value;
            }
            Binder defaultBinder = Type.DefaultBinder;
            object obj3 = type.InvokeMember(name, invokeAttr, defaultBinder, target, args);
            if (obj3 != null)
            {
                return new Result(obj3.GetType(), obj3);
            }
            return new Result(null, null);
        }

        private Result EvalProp(Evaluator evaluater, Result[] argArray)
        {
            if (argArray.Length == 0)
            {
                throw new ArgumentException();
            }
            if (argArray.Length == 1)
            {
                return argArray[0];
            }
            string name = argArray[1].Value as string;
            if (name == null)
            {
                throw new ArgumentException();
            }
            if (argArray[0] == null)
            {
                throw new ArgumentNullException("argArray[0]");
            }
            object obj2 = argArray[0].Value;
            if (obj2 == null)
            {
                throw new ArgumentException();
            }
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
            bindingAttr |= BindingFlags.Public;
            PropertyInfo property = type.GetProperty(name, bindingAttr);
            if (property != null)
            {
                return new Result(property.PropertyType, property.GetValue(obj2, null));
            }
            FieldInfo field = type.GetField(name, bindingAttr);
            if (field == null)
            {
                throw new ArgumentException();
            }
            return new Result(field.FieldType, field.GetValue(obj2));
        }

        public override string ToString()
        {
            if (this.IsFunction)
            {
                return "function";
            }
            return "member";
        }

        public bool IsFunction
        {
            get
            {
                return this._isFunction;
            }
            set
            {
                this._isFunction = value;
            }
        }

        public override OperatorPriority Priority
        {
            get
            {
                return OperatorPriority.Member;
            }
        }
    }
}
