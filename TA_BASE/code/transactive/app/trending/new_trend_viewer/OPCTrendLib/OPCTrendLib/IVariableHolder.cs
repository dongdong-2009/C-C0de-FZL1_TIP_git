namespace OPCTrendLib
{
    using System;

    public interface IVariableHolder
    {
        bool Exists(string name);
        object GetVariable(string name);
    }
}
