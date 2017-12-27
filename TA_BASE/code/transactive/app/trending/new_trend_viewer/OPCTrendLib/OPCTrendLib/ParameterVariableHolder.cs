namespace OPCTrendLib
{
    using System;

    public class ParameterVariableHolder : IVariableHolder
    {
        private ParameterCollection _parameters = new ParameterCollection();

        public bool Exists(string name)
        {
            return this._parameters.Contains(name);
        }

        private object GetVariable(string name)
        {
            return this._parameters[name].Value;
        }

        object IVariableHolder.GetVariable(string name)
        {
            return this.GetVariable(name);
        }

        public ParameterCollection Parameters
        {
            get
            {
                return this._parameters;
            }
        }
    }
}
