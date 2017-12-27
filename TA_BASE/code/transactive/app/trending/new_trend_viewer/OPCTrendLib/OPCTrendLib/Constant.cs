namespace OPCTrendLib
{
    using System;

    internal class Constant : Operand
    {
        private System.Type _type;
        private object _value;

        public Constant(object value)
        {
            this._value = value;
            if (this._value != null)
            {
                this._type = value.GetType();
            }
        }

        public Constant(System.Type type, object value)
        {
            this._value = value;
            this._type = type;
        }

        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            return new Result(this._type, this._value);
        }

        public override string ToString()
        {
            if (this._value == null)
            {
                return "null";
            }
            return this._value.ToString();
        }

        public System.Type Type
        {
            get
            {
                return this._type;
            }
        }
    }
}
