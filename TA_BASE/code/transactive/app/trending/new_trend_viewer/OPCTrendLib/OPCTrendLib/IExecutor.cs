namespace OPCTrendLib
{
    using System;

    public interface IExecutor
    {
        ExecuteAction Execute(IExecutorContext context);
        void Initialize(IExecutorContext context, ExprNode node);
        void Reset(IExecutorContext context);

        ExprNode Node { get; }

        OPCTrendLib.Result Result { get; }
    }
}
