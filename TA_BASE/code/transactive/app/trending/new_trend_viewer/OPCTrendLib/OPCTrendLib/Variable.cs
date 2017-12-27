namespace OPCTrendLib
{
    using System;

    internal class Variable : Identifier
    {
        public Variable(string name) : base(name)
        {
        }

        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            if (!evaluater.VariableHolder.Exists(base.Name))
            {
                throw EvalException.VariableNotExist(base.Name);
            }
            object variable = evaluater.VariableHolder.GetVariable(base.Name);
            if (variable == null)
            {
                return new Result(null, null);
            }
            return new Result(variable.GetType(), variable);
        }

        public override string ToString()
        {
            return ("variable." + base.Name);
        }
    }
}
