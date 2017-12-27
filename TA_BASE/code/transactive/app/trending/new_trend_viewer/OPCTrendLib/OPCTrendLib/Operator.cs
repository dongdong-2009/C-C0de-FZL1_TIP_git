namespace OPCTrendLib
{
    internal abstract class Operator : Expression
    {
        public abstract OperatorPriority Priority { get; }
    }
}
