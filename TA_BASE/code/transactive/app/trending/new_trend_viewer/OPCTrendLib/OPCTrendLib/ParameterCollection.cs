namespace OPCTrendLib
{
    using System;
    using System.Collections;
    using System.Reflection;

    public sealed class ParameterCollection
    {
        private Hashtable _parameters = new Hashtable();

        public void Clear()
        {
            this._parameters.Clear();
        }

        public bool Contains(string name)
        {
            return this._parameters.ContainsKey(name);
        }

        public Parameter this[string name]
        {
            get
            {
                return (Parameter) this._parameters[name];
            }
            set
            {
                if (value == null)
                {
                    this._parameters.Remove(name);
                }
                else
                {
                    this._parameters[name] = value;
                }
            }
        }
    }
}
