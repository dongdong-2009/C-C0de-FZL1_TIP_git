namespace OPCTrendLib
{
    using System;

    public enum OperatorPriority
    {
        Lowest,
        Conditional,
        LogicalOr,
        LogicalAnd,
        BitwiseOr,
        BitwiseXor,
        BitwiseAnd,
        Equality,
        Relational,
        Shift,
        Additive,
        Multiplicative,
        Unary,
        Indexer,
        Function,
        Member
    }
}
