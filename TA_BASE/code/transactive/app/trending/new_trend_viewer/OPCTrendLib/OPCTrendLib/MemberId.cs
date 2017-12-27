namespace OPCTrendLib
{
    using System;

    internal class MemberId : Identifier
    {
        public MemberId(string name) : base(name)
        {
        }

        internal override Result Eval(Evaluator evaluater, Result[] argArray)
        {
            return new Result(typeof(string), base.Name);
        }

        public override string ToString()
        {
            return ("member." + base.Name);
        }
    }
}
