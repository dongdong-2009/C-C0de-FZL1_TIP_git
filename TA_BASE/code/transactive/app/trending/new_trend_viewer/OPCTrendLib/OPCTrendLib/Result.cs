namespace OPCTrendLib
{
    using System;

    public class Result
    {
        private System.Type _type;
        private object _value;

        public Result(object value)
        {
            this._value = value;
            this._type = value.GetType();
        }

        public Result(System.Type type, object value)
        {
            this._value = value;
            this._type = type;
        }

        public bool IsNull()
        {
            return (this._value == null);
        }

        public override string ToString()
        {
            if (this._value == null)
            {
                return "(null)";
            }
            return this._value.ToString();
        }

        public System.Type Type
        {
            get
            {
                return this._type;
            }
            set
            {
                if (this._value != null)
                {
                    throw new InvalidOperationException();
                }
                this._type = value;
            }
        }

        public object Value
        {
            get
            {
                return this._value;
            }
            set
            {
                this._value = value;
                if (this._value == null)
                {
                    this._type = null;
                }
                else
                {
                    this._type = this._value.GetType();
                }
            }
        }
    }
}
