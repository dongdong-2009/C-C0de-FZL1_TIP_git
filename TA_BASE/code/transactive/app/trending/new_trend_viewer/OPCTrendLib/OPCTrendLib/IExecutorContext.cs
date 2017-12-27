namespace OPCTrendLib
{
    using System;

    public interface IExecutorContext
    {
        IExecutor CreateExecutor(ExprNode node);
        Result Execute(ExprNode node);
        void Pop();
        void Push(IExecutor executor);

        object Tag { get; }

        IExecutor Top { get; }
    }
}
