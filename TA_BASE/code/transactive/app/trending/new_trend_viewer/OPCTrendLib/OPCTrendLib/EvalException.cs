namespace OPCTrendLib
{
    using OPCTrendLib.Properties;
    using System;

    public class EvalException : Exception
    {
        public EvalException(string msg) : base(msg)
        {
        }

        internal static EvalException VariableNotExist(string varName)
        {
            return new EvalException(string.Format(Resources.VariableNotExist, varName));
        }
    }
}
