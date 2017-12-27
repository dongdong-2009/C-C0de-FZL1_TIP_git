namespace OPCTrendLib
{
    using System;

    public class Parameter
    {
        private System.Type _type;
        private object _value;

        public Parameter()
        {
        }

        public Parameter(object value)
        {
            this._value = value;
            if (value != null)
            {
                this._type = value.GetType();
            }
        }

        public Parameter(object value, System.Type type)
        {
            this._value = value;
            this._type = type;
        }

        public override string ToString()
        {
            if (this._value != null)
            {
                return this._value.ToString();
            }
            return "(null)";
        }

        public System.Type Type
        {
            get
            {
                return this._type;
            }
            set
            {
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
            }
        }
    }
}
